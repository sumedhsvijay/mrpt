/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */
#pragma once

#include <mrpt/system/CTicTac.h>

namespace mrpt::system
{
/** A class for calling sleep() in a loop, such that the amount of sleep time
 * will be computed to make the loop run at the desired rate (in Hz).
 * \ingroup mrpt_system_grp
 */
class CRateTimer
{
   public:
	/** Ctor: specifies the desired rate (Hz) */
	CRateTimer(const double rate_hz = 1.0);
	/** Dtor */
	virtual ~CRateTimer() = default;

	/** Changes the object loop rate (Hz) */
	void setRate(const double rate_hz);

	/** Gets current rate (Hz) */
	double rate() const { return m_rate_hz; }

	/** Sleeps for some time, such as the return of this method is 1/rate
	 * (seconds)
	 * after the return of the previous call.
	 * \return false if the rate could not be achieved ("we are already late"),
	 * true if all went right. */
	bool sleep();

   private:
	double m_rate_hz{1.0};
	mrpt::system::CTicTac m_tictac;
};	// End of class def.

}  // namespace mrpt::system
