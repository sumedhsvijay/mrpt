/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include <CTraitsTest.h>
#include <gtest/gtest.h>
#include <mrpt/math/transform_gaussian.h>
#include <mrpt/poses/CPose3D.h>
#include <mrpt/poses/CPose3DPDFGaussian.h>
#include <mrpt/poses/CPose3DQuatPDFGaussian.h>
#include <mrpt/random.h>

#include <Eigen/Dense>

using namespace mrpt;
using namespace mrpt::poses;
using namespace mrpt::math;
using namespace std;

template class mrpt::CTraitsTest<CPose3DPDFGaussian>;

class Pose3DPDFGaussTests : public ::testing::Test
{
   protected:
	void SetUp() override {}
	void TearDown() override {}
	static CPose3DPDFGaussian generateRandomPose3DPDF(
		double x, double y, double z, double yaw, double pitch, double roll,
		double std_scale)
	{
		CMatrixDouble61 r;
		mrpt::random::getRandomGenerator().drawGaussian1DMatrix(
			r, 0, std_scale);
		CMatrixDouble66 cov;
		cov.matProductOf_AAt(r);  // random semi-definite positive matrix:
		for (int i = 0; i < 6; i++)
			cov(i, i) += 1e-7;
		CPose3DPDFGaussian p6pdf(CPose3D(x, y, z, yaw, pitch, roll), cov);
		return p6pdf;
	}

	void testToQuatPDFAndBack(
		double x, double y, double z, double yaw, double pitch, double roll,
		double std_scale)
	{
		CPose3DPDFGaussian p6pdf =
			generateRandomPose3DPDF(x, y, z, yaw, pitch, roll, std_scale);
		// cout << "p6pdf: " << p6pdf << endl;
		CPose3DQuatPDFGaussian p7pdf = CPose3DQuatPDFGaussian(p6pdf);
		// cout << "p7pdf: " << p7pdf << endl;
		CPose3DPDFGaussian p6pdf_recov = CPose3DPDFGaussian(p7pdf);
		// cout << "p6pdf_recov: " << p6pdf_recov  << endl;

		const double val_mean_error =
			(p6pdf_recov.mean.asVectorVal() - p6pdf.mean.asVectorVal())
				.sum_abs();
		const double cov_mean_error = (p6pdf_recov.cov - p6pdf.cov).sum_abs();
		// cout << "cov err: " << cov_mean_error << " " << "val_mean_error: " <<
		// val_mean_error << endl;
		EXPECT_TRUE(val_mean_error < 1e-8);
		EXPECT_TRUE(cov_mean_error < 1e-8);
	}

	static void func_compose(
		const CVectorFixedDouble<12>& x, [[maybe_unused]] const double& dummy,
		CVectorFixedDouble<6>& Y)
	{
		const CPose3D p1(x[0], x[1], x[2], x[3], x[4], x[5]);
		const CPose3D p2(
			x[6 + 0], x[6 + 1], x[6 + 2], x[6 + 3], x[6 + 4], x[6 + 5]);
		const CPose3D p = p1 + p2;
		for (int i = 0; i < 6; i++)
			Y[i] = p[i];
	}

	static void func_inv_compose(
		const CVectorFixedDouble<2 * 6>& x,
		[[maybe_unused]] const double& dummy, CVectorFixedDouble<6>& Y)
	{
		const CPose3D p1(x[0], x[1], x[2], x[3], x[4], x[5]);
		const CPose3D p2(
			x[6 + 0], x[6 + 1], x[6 + 2], x[6 + 3], x[6 + 4], x[6 + 5]);
		const CPose3D p = p1 - p2;
		for (int i = 0; i < 6; i++)
			Y[i] = p[i];
	}

	// Test "+" & "+=" operator
	void testPoseComposition(
		double x, double y, double z, double yaw, double pitch, double roll,
		double std_scale, double x2, double y2, double z2, double yaw2,
		double pitch2, double roll2, double std_scale2)
	{
		CPose3DPDFGaussian p6pdf1 =
			generateRandomPose3DPDF(x, y, z, yaw, pitch, roll, std_scale);
		CPose3DPDFGaussian p6pdf2 = generateRandomPose3DPDF(
			x2, y2, z2, yaw2, pitch2, roll2, std_scale2);

		// With "+" operators:
		CPose3DPDFGaussian p6_comp = p6pdf1 + p6pdf2;

		// Numeric approximation:
		CVectorFixedDouble<6> y_mean;
		CMatrixFixed<double, 6, 6> y_cov;
		{
			CVectorFixedDouble<12> x_mean;
			for (int i = 0; i < 6; i++)
				x_mean[i] = p6pdf1.mean[i];
			for (int i = 0; i < 6; i++)
				x_mean[6 + i] = p6pdf2.mean[i];

			CMatrixFixed<double, 12, 12> x_cov;
			x_cov.insertMatrix(0, 0, p6pdf1.cov);
			x_cov.insertMatrix(6, 6, p6pdf2.cov);

			double DUMMY = 0;
			CVectorFixedDouble<12> x_incrs;
			x_incrs.fill(1e-6);
			transform_gaussian_linear(
				x_mean, x_cov, func_compose, DUMMY, y_mean, y_cov, x_incrs);
		}
		// Compare mean:
		EXPECT_NEAR(0, (y_mean - p6_comp.mean.asVectorVal()).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;

		// Compare cov:
		EXPECT_NEAR(0, (y_cov - p6_comp.cov).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;

		// Test +=
		p6_comp = p6pdf1;
		p6_comp += p6pdf2;

		// Compare mean:
		EXPECT_NEAR(0, (y_mean - p6_comp.mean.asVectorVal()).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;

		// Compare cov:
		EXPECT_NEAR(0, (y_cov - p6_comp.cov).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;
	}

	void testCompositionJacobian(
		double x, double y, double z, double yaw, double pitch, double roll,
		double x2, double y2, double z2, double yaw2, double pitch2,
		double roll2)
	{
		const CPose3D q1(x, y, z, yaw, pitch, roll);
		const CPose3D q2(x2, y2, z2, yaw2, pitch2, roll2);

		// Theoretical Jacobians:
		CMatrixDouble66 df_dx(UNINITIALIZED_MATRIX),
			df_du(UNINITIALIZED_MATRIX);
		CPose3DPDF::jacobiansPoseComposition(
			q1,	 // x
			q2,	 // u
			df_dx, df_du);

		// Numerical approximation:
		CMatrixDouble66 num_df_dx(UNINITIALIZED_MATRIX),
			num_df_du(UNINITIALIZED_MATRIX);
		{
			CVectorFixedDouble<2 * 6> x_mean;
			for (int i = 0; i < 6; i++)
				x_mean[i] = q1[i];
			for (int i = 0; i < 6; i++)
				x_mean[6 + i] = q2[i];

			double DUMMY = 0;
			CVectorFixedDouble<2 * 6> x_incrs;
			x_incrs.fill(1e-7);
			CMatrixDouble numJacobs;
			mrpt::math::estimateJacobian(
				x_mean,
				std::function<void(
					const CVectorFixedDouble<12>& x, const double& dummy,
					CVectorFixedDouble<6>& Y)>(&func_compose),
				x_incrs, DUMMY, numJacobs);

			num_df_dx = numJacobs.block<6, 6>(0, 0);
			num_df_du = numJacobs.block<6, 6>(0, 6);
		}

		// Compare:
		EXPECT_NEAR(0, (df_dx - num_df_dx).sum_abs(), 3e-3)
			<< "q1: " << q1 << endl
			<< "q2: " << q2 << endl
			<< "Numeric approximation of df_dx: " << endl
			<< num_df_dx << endl
			<< "Implemented method: " << endl
			<< df_dx << endl
			<< "Error: " << endl
			<< df_dx - num_df_dx << endl;

		EXPECT_NEAR(0, (df_du - num_df_du).sum_abs(), 3e-3)
			<< "q1: " << q1 << endl
			<< "q2: " << q2 << endl
			<< "Numeric approximation of df_du: " << endl
			<< num_df_du << endl
			<< "Implemented method: " << endl
			<< df_du << endl
			<< "Error: " << endl
			<< df_du - num_df_du << endl;
	}

	// Test the "-" & "-=" operator
	void testPoseInverseComposition(
		double x, double y, double z, double yaw, double pitch, double roll,
		double std_scale, double x2, double y2, double z2, double yaw2,
		double pitch2, double roll2, double std_scale2)
	{
		CPose3DPDFGaussian p6pdf1 =
			generateRandomPose3DPDF(x, y, z, yaw, pitch, roll, std_scale);
		CPose3DPDFGaussian p6pdf2 = generateRandomPose3DPDF(
			x2, y2, z2, yaw2, pitch2, roll2, std_scale2);

		// With the "-" operator
		CPose3DPDFGaussian p6_comp = p6pdf1 - p6pdf2;

		// Numeric approximation:
		CVectorFixedDouble<6> y_mean;
		CMatrixFixed<double, 6, 6> y_cov;
		{
			CVectorFixedDouble<2 * 6> x_mean;
			for (int i = 0; i < 6; i++)
				x_mean[i] = p6pdf1.mean[i];
			for (int i = 0; i < 6; i++)
				x_mean[6 + i] = p6pdf2.mean[i];

			CMatrixFixed<double, 12, 12> x_cov;
			x_cov.insertMatrix(0, 0, p6pdf1.cov);
			x_cov.insertMatrix(6, 6, p6pdf2.cov);

			double DUMMY = 0;
			CVectorFixedDouble<2 * 6> x_incrs;
			x_incrs.fill(1e-6);
			transform_gaussian_linear(
				x_mean, x_cov, func_inv_compose, DUMMY, y_mean, y_cov, x_incrs);
		}
		// Compare mean:
		EXPECT_NEAR(0, (y_mean - p6_comp.mean.asVectorVal()).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;

		// Compare cov:
		EXPECT_NEAR(0, (y_cov - p6_comp.cov).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;

		// With the "-=" operator
		p6_comp = p6pdf1;
		p6_comp -= p6pdf2;

		// Compare mean:
		EXPECT_NEAR(0, (y_mean - p6_comp.mean.asVectorVal()).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;

		// Compare cov:
		EXPECT_NEAR(0, (y_cov - p6_comp.cov).sum_abs(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "p2 mean: " << p6pdf2.mean << endl;
	}

	// Test the unary "-" operator
	void testPoseInverse(
		double x, double y, double z, double yaw, double pitch, double roll,
		double std_scale)
	{
		CPose3DPDFGaussian p6pdf2 =
			generateRandomPose3DPDF(x, y, z, yaw, pitch, roll, std_scale);
		CPose3DPDFGaussian p6_zero(
			CPose3D(0, 0, 0, 0, 0, 0), CMatrixDouble66());	// COV=All zeros

		// Unary "-":
		const CPose3DPDFGaussian p6_inv = -p6pdf2;

		// Compare to the binary "-" operator:
		const CPose3DPDFGaussian p6_comp = p6_zero - p6pdf2;

		// Compare mean:
		EXPECT_NEAR(
			0,
			(p6_inv.mean.asVectorVal() - p6_comp.mean.asVectorVal())
				.array()
				.abs()
				.sum(),
			1e-2)
			<< "p mean: " << p6pdf2.mean << endl;

		// Compare cov:
		EXPECT_NEAR(0, (p6_inv.cov - p6_comp.cov).sum_abs(), 1e-2)
			<< "p mean: " << p6pdf2.mean << endl;

		// Compare to the "inverse()" method:
		CPose3DPDFGaussian p6_inv2;
		p6pdf2.inverse(p6_inv2);

		// Compare mean:
		EXPECT_NEAR(
			0,
			(p6_inv2.mean.asVectorVal() - p6_comp.mean.asVectorVal())
				.array()
				.abs()
				.sum(),
			1e-2)
			<< "p mean: " << p6pdf2.mean << endl
			<< "p6_inv2 mean: " << p6_inv2.mean << endl
			<< "p6_comp mean: " << p6_comp.mean << endl;

		// Compare cov:
		EXPECT_NEAR(0, (p6_inv2.cov - p6_comp.cov).sum_abs(), 1e-2)
			<< "p mean: " << p6pdf2.mean << endl
			<< "p6_inv2 mean: " << p6_inv2.mean << endl
			<< "p6_comp mean: " << p6_comp.mean << endl;
	}

	// Test all operators
	void testAllPoseOperators(
		double x, double y, double z, double yaw, double pitch, double roll,
		double std_scale, double x2, double y2, double z2, double yaw2,
		double pitch2, double roll2, double std_scale2)
	{
		// +, +=
		testPoseComposition(
			x, y, z, yaw, pitch, roll, std_scale, x2, y2, z2, yaw2, pitch2,
			roll2, std_scale2);
		// -, -=, unary "-"
		testPoseInverseComposition(
			x, y, z, yaw, pitch, roll, std_scale, x2, y2, z2, yaw2, pitch2,
			roll2, std_scale2);
		// unitary "-" & ".inverse()"
		testPoseInverse(x, y, z, yaw, pitch, roll, std_scale);
	}

	void testChangeCoordsRef(
		double x, double y, double z, double yaw, double pitch, double roll,
		double std_scale, double x2, double y2, double z2, double yaw2,
		double pitch2, double roll2)
	{
		CPose3DPDFGaussian p6pdf1 =
			generateRandomPose3DPDF(x, y, z, yaw, pitch, roll, std_scale);

		const CPose3D new_base = CPose3D(x2, y2, z2, yaw2, pitch2, roll2);
		const CPose3DPDFGaussian new_base_pdf(
			new_base, CMatrixDouble66());  // COV = Zeros

		const CPose3DPDFGaussian p6_new_base_pdf = new_base_pdf + p6pdf1;
		p6pdf1.changeCoordinatesReference(new_base);

		// Compare:
		EXPECT_NEAR(
			0, (p6_new_base_pdf.cov - p6pdf1.cov).array().abs().mean(), 1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "new_base: " << new_base << endl;
		EXPECT_NEAR(
			0,
			(p6_new_base_pdf.mean.asVectorVal() - p6pdf1.mean.asVectorVal())
				.array()
				.abs()
				.mean(),
			1e-2)
			<< "p1 mean: " << p6pdf1.mean << endl
			<< "new_base: " << new_base << endl;
	}
};

TEST_F(Pose3DPDFGaussTests, ToQuatGaussPDFAndBack)
{
	testToQuatPDFAndBack(0, 0, 0, 0.0_deg, 0.0_deg, 0.0_deg, 0.1);
	testToQuatPDFAndBack(0, 0, 0, 0.0_deg, 0.0_deg, 0.0_deg, 0.2);

	testToQuatPDFAndBack(6, -2, -3, 0.0_deg, 0.0_deg, 0.0_deg, 0.1);
	testToQuatPDFAndBack(6, -2, -3, 0.0_deg, 0.0_deg, 0.0_deg, 0.2);

	testToQuatPDFAndBack(6, -2, -3, 10.0_deg, 40.0_deg, 5.0_deg, 0.1);
	testToQuatPDFAndBack(6, -2, -3, 10.0_deg, 40.0_deg, 5.0_deg, 0.2);

	testToQuatPDFAndBack(6, -2, -3, -50.0_deg, 87.0_deg, 20.0_deg, 0.1);
	testToQuatPDFAndBack(6, -2, -3, -50.0_deg, 87.0_deg, 20.0_deg, 0.2);

	testToQuatPDFAndBack(6, -2, -3, -50.0_deg, -87.0_deg, 20.0_deg, 0.1);
	testToQuatPDFAndBack(6, -2, -3, -50.0_deg, -87.0_deg, 20.0_deg, 0.2);
}

TEST_F(Pose3DPDFGaussTests, CompositionJacobian)
{
	testCompositionJacobian(
		0, 0, 0, 2.0_deg, 0.0_deg, 0.0_deg, 0, 0, 0, 0.0_deg, 0.0_deg, 0.0_deg);
	testCompositionJacobian(
		0, 0, 0, 2.0_deg, 0.0_deg, 0.0_deg, 0, 0, 0, 0.0_deg, 0.0_deg, 0.0_deg);
	testCompositionJacobian(
		1, 2, 3, 2.0_deg, 0.0_deg, 0.0_deg, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg);
	testCompositionJacobian(
		1, -2, 3, 2.0_deg, 0.0_deg, 0.0_deg, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg);
	testCompositionJacobian(
		1, 2, -3, 2.0_deg, 0.0_deg, 0.0_deg, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg);
	testCompositionJacobian(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, -8, 45, 10, 50.0_deg, -10.0_deg,
		30.0_deg);
	testCompositionJacobian(
		1, 2, 3, 20.0_deg, -80.0_deg, 70.0_deg, -8, 45, 10, 50.0_deg, -10.0_deg,
		30.0_deg);
	testCompositionJacobian(
		1, 2, 3, 20.0_deg, 80.0_deg, -70.0_deg, -8, 45, 10, 50.0_deg, -10.0_deg,
		30.0_deg);
	testCompositionJacobian(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, -8, 45, 10, -50.0_deg, -10.0_deg,
		30.0_deg);
	testCompositionJacobian(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, -8, 45, 10, 50.0_deg, 10.0_deg,
		30.0_deg);
	testCompositionJacobian(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, -8, 45, 10, 50.0_deg, -10.0_deg,
		-30.0_deg);
}

// Test the +, -, +=, -=, "-" operators
TEST_F(Pose3DPDFGaussTests, AllOperators)
{
	testAllPoseOperators(
		0, 0, 0, 0.0_deg, 0.0_deg, 0.0_deg, 0.1, 0, 0, 0, 0.0_deg, 0.0_deg,
		0.0_deg, 0.1);
	testAllPoseOperators(
		1, 2, 3, 0.0_deg, 0.0_deg, 0.0_deg, 0.1, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg, 0.1);

	testAllPoseOperators(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, 0.1, -8, 45, 10, 50.0_deg,
		-10.0_deg, 30.0_deg, 0.1);
	testAllPoseOperators(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, 0.2, -8, 45, 10, 50.0_deg,
		-10.0_deg, 30.0_deg, 0.2);

	testAllPoseOperators(
		1, 2, 3, 10.0_deg, 0.0_deg, 0.0_deg, 0.1, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg, 0.1);
	testAllPoseOperators(
		1, 2, 3, 0.0_deg, 10.0_deg, 0.0_deg, 0.1, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg, 0.1);
	testAllPoseOperators(
		1, 2, 3, 0.0_deg, 0.0_deg, 10.0_deg, 0.1, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg, 0.1);
	testAllPoseOperators(
		1, 2, 3, 0.0_deg, 0.0_deg, 0.0_deg, 0.1, -8, 45, 10, 10.0_deg, 0.0_deg,
		0.0_deg, 0.1);
	testAllPoseOperators(
		1, 2, 3, 0.0_deg, 0.0_deg, 0.0_deg, 0.1, -8, 45, 10, 0.0_deg, 10.0_deg,
		0.0_deg, 0.1);
	testAllPoseOperators(
		1, 2, 3, 0.0_deg, 0.0_deg, 0.0_deg, 0.1, -8, 45, 10, 0.0_deg, 0.0_deg,
		10.0_deg, 0.1);
}

TEST_F(Pose3DPDFGaussTests, ChangeCoordsRef)
{
	testChangeCoordsRef(
		0, 0, 0, 0.0_deg, 0.0_deg, 0.0_deg, 0.1, 0, 0, 0, 0.0_deg, 0.0_deg,
		0.0_deg);
	testChangeCoordsRef(
		1, 2, 3, 0.0_deg, 0.0_deg, 0.0_deg, 0.1, -8, 45, 10, 0.0_deg, 0.0_deg,
		0.0_deg);

	testChangeCoordsRef(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, 0.1, -8, 45, 10, 50.0_deg,
		-10.0_deg, 30.0_deg);
	testChangeCoordsRef(
		1, 2, 3, 20.0_deg, 80.0_deg, 70.0_deg, 0.2, -8, 45, 10, 50.0_deg,
		-10.0_deg, 30.0_deg);
}
