/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "kinematics-precomp.h"	 // Precompiled header
//
#include <mrpt/kinematics/CVehicleSimulVirtualBase.h>
#include <mrpt/math/wrap2pi.h>
#include <mrpt/random.h>

using namespace mrpt::kinematics;

CVehicleSimulVirtualBase::CVehicleSimulVirtualBase() = default;

CVehicleSimulVirtualBase::~CVehicleSimulVirtualBase() = default;
void CVehicleSimulVirtualBase::setCurrentGTPose(const mrpt::math::TPose2D& pose)
{
	m_GT_pose = pose;
}

void CVehicleSimulVirtualBase::simulateOneTimeStep(const double dt)
{
	using mrpt::math::TPose2D;
	const double final_t = m_time + dt;
	while (m_time <= final_t)
	{
		// Simulate movement during At:
		TPose2D nextOdometry = m_odometry;
		nextOdometry.x += m_odometric_vel.vx * m_firmware_control_period;
		nextOdometry.y += m_odometric_vel.vy * m_firmware_control_period;
		nextOdometry.phi += m_odometric_vel.omega * m_firmware_control_period;
		mrpt::math::wrapToPiInPlace(nextOdometry.phi);

		TPose2D gtDelta = m_GT_vel * m_firmware_control_period;
		// Add some errors
		if (m_use_odo_error)
		{
			auto& rng = mrpt::random::getRandomGenerator();
			gtDelta.x *= 1.0 + m_Ax_err_bias +
				m_Ax_err_std * rng.drawGaussian1D_normalized();
			gtDelta.y *= 1.0 + m_Ay_err_bias +
				m_Ay_err_std * rng.drawGaussian1D_normalized();
			gtDelta.phi *= 1.0 + m_Aphi_err_bias +
				m_Aphi_err_std * rng.drawGaussian1D_normalized();
			mrpt::math::wrapToPiInPlace(gtDelta.phi);
		}

		TPose2D nextGT = {
			m_GT_pose.x + gtDelta.x, m_GT_pose.y + gtDelta.y,
			m_GT_pose.phi + gtDelta.phi};
		mrpt::math::wrapToPiInPlace(nextGT.phi);

		this->internal_simulControlStep(m_firmware_control_period);

		// Now rotate our current Odo velocity into GT coordinates
		m_GT_vel = getCurrentOdometricVelLocal();
		m_GT_vel.rotate(m_GT_pose.phi);

		m_odometry = nextOdometry;
		m_GT_pose = nextGT;

		m_time += m_firmware_control_period;  // Move forward
	}
}

void CVehicleSimulVirtualBase::resetStatus()
{
	m_GT_pose = mrpt::math::TPose2D(.0, .0, .0);
	m_GT_vel = mrpt::math::TTwist2D(.0, .0, .0);
	m_odometry = mrpt::math::TPose2D(.0, .0, .0);
	m_odometric_vel = mrpt::math::TTwist2D(.0, .0, .0);
	internal_clear();
}

void CVehicleSimulVirtualBase::resetTime() { m_time = .0; }
mrpt::math::TTwist2D CVehicleSimulVirtualBase::getCurrentGTVelLocal() const
{
	mrpt::math::TTwist2D tl = this->m_GT_vel;
	tl.rotate(-m_GT_pose.phi);
	return tl;
}

mrpt::math::TTwist2D CVehicleSimulVirtualBase::getCurrentOdometricVelLocal()
	const
{
	mrpt::math::TTwist2D tl = this->m_odometric_vel;
	tl.rotate(-m_odometry.phi);
	return tl;
}
