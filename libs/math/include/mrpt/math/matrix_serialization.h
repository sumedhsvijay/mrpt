/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */
#pragma once

#include <mrpt/math/CMatrixD.h>
#include <mrpt/math/CMatrixDynamic.h>
#include <mrpt/math/CMatrixF.h>
#include <mrpt/math/CMatrixFixed.h>
#include <mrpt/serialization/CArchive.h>

/** \file matrix_serialization.h
 * This file implements matrix/vector text and binary serialization */
namespace mrpt::math
{
/** \addtogroup container_ops_grp
 *  @{ */

/** @name Operators for binary streaming of MRPT matrices
	@{ */

/** Read operator from a CStream. The format is compatible with that of CMatrixF
 * & CMatrixD */
template <size_t NROWS, size_t NCOLS>
mrpt::serialization::CArchive& operator>>(
	mrpt::serialization::CArchive& in, CMatrixFixed<float, NROWS, NCOLS>& M)
{
	CMatrixF aux;
	in.ReadObject(&aux);
	ASSERTMSG_(
		M.cols() == aux.cols() && M.rows() == aux.rows(),
		format(
			"Size mismatch: deserialized is %ux%u, expected is %ux%u",
			(unsigned)aux.rows(), (unsigned)aux.cols(), (unsigned)NROWS,
			(unsigned)NCOLS));
	M = aux;
	return in;
}
/** Read operator from a CStream. The format is compatible with that of CMatrixF
 * & CMatrixD */
template <size_t NROWS, size_t NCOLS>
mrpt::serialization::CArchive& operator>>(
	mrpt::serialization::CArchive& in, CMatrixFixed<double, NROWS, NCOLS>& M)
{
	CMatrixD aux;
	in.ReadObject(&aux);
	ASSERTMSG_(
		M.cols() == aux.cols() && M.rows() == aux.rows(),
		format(
			"Size mismatch: deserialized is %ux%u, expected is %ux%u",
			(unsigned)aux.rows(), (unsigned)aux.cols(), (unsigned)NROWS,
			(unsigned)NCOLS));
	M = aux;
	return in;
}

/** Write operator for writing into a CStream. The format is compatible with
 * that of CMatrixF & CMatrixD */
template <size_t NROWS, size_t NCOLS>
mrpt::serialization::CArchive& operator<<(
	mrpt::serialization::CArchive& out,
	const CMatrixFixed<float, NROWS, NCOLS>& M)
{
	CMatrixF aux;
	aux = M;
	out.WriteObject(&aux);
	return out;
}
/** Write operator for writing into a CStream. The format is compatible with
 * that of CMatrixF & CMatrixD */
template <size_t NROWS, size_t NCOLS>
mrpt::serialization::CArchive& operator<<(
	mrpt::serialization::CArchive& out,
	const CMatrixFixed<double, NROWS, NCOLS>& M)
{
	CMatrixD aux;
	aux = M;
	out.WriteObject(&aux);
	return out;
}

/** @} */  // end MRPT matrices stream operators

/** @name Operators for text streaming of MRPT matrices
	@{ */

/** Binary serialization of symmetric matrices, saving the space of duplicated
 * values. \sa serializeSymmetricMatrixTo() */
template <typename MAT>
void deserializeSymmetricMatrixFrom(MAT& m, mrpt::serialization::CArchive& in)
{
	ASSERT_EQUAL_(m.rows(), m.cols());
	auto N = m.cols();
	for (decltype(N) i = 0; i < N; i++)
		in >> m(i, i);
	for (decltype(N) r = 0; r < N - 1; r++)
	{
		for (decltype(N) c = r + 1; c < N; c++)
		{
			typename MAT::Scalar x;
			in >> x;
			m(r, c) = m(c, r) = x;
		}
	}
}

/** Binary serialization of symmetric matrices, saving the space of duplicated
 * values. \sa deserializeSymmetricMatrixFrom() */
template <typename MAT>
void serializeSymmetricMatrixTo(MAT& m, mrpt::serialization::CArchive& out)
{
	ASSERT_EQUAL_(m.rows(), m.cols());
	auto N = m.cols();
	for (decltype(N) i = 0; i < N; i++)
		out << m(i, i);
	for (decltype(N) r = 0; r < N - 1; r++)
		for (decltype(N) c = r + 1; c < N; c++)
			out << m(r, c);
}

/** @} */  // end MRPT matrices stream operators

/**  @} */	// end of grouping
}  // namespace mrpt::math
