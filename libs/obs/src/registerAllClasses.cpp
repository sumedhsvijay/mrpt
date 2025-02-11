/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "obs-precomp.h"  // Precompiled headers
//

#define MRPT_NO_WARN_BIG_HDR
#include <mrpt/core/initializer.h>
#include <mrpt/math/registerAllClasses.h>
#include <mrpt/obs.h>
#include <mrpt/obs/registerAllClasses.h>
#include <mrpt/serialization/CSerializable.h>
// deps:
#include <mrpt/opengl/registerAllClasses.h>
#include <mrpt/tfest/registerAllClasses.h>

MRPT_INITIALIZER(registerAllClasses_mrpt_obs)
{
	using namespace mrpt::obs;
	using namespace mrpt::maps;

#if !defined(DISABLE_MRPT_AUTO_CLASS_REGISTRATION)
	registerClass(CLASS_ID(CSensoryFrame));
	registerClassCustomName("CSensorialFrame", CLASS_ID(CSensoryFrame));

	registerClass(CLASS_ID(CObservation));
	registerClass(CLASS_ID(CObservation2DRangeScan));
	registerClass(CLASS_ID(CObservation3DRangeScan));
	registerClass(CLASS_ID(CObservation3DScene));
	registerClass(CLASS_ID(CObservationVelodyneScan));
	registerClass(CLASS_ID(CObservationRGBD360));
	registerClass(CLASS_ID(CObservationBatteryState));
	registerClass(CLASS_ID(CObservationWirelessPower));
	registerClass(CLASS_ID(CObservationRFID));
	registerClass(CLASS_ID(CObservationBeaconRanges));
	registerClass(CLASS_ID(CObservationBearingRange));
	registerClass(CLASS_ID(CObservationComment));
	registerClass(CLASS_ID(CObservationGasSensors));
	registerClass(CLASS_ID(CObservationWindSensor));
	registerClass(CLASS_ID(CObservationGPS));
	registerClass(CLASS_ID(CObservationImage));
	registerClass(CLASS_ID(CObservationIMU));
	registerClass(CLASS_ID(CObservationOdometry));
	registerClass(CLASS_ID(CObservationRange));
	registerClass(CLASS_ID(CObservationReflectivity));
	registerClass(CLASS_ID(CObservationStereoImages));
	registerClass(CLASS_ID(CObservationStereoImagesFeatures));
	registerClass(CLASS_ID(CObservation6DFeatures));
	registerClass(CLASS_ID(CObservationRobotPose));
	registerClass(CLASS_ID(CObservationCANBusJ1939));
	registerClass(CLASS_ID(CObservationRawDAQ));

	registerClass(CLASS_ID(CSimpleMap));
	registerClassCustomName("CSensFrameProbSequence", CLASS_ID(CSimpleMap));

	registerClass(CLASS_ID(CMetricMap));
	registerClass(CLASS_ID(CRawlog));

	registerClass(CLASS_ID(CAction));
	registerClass(CLASS_ID(CActionCollection));
	registerClass(CLASS_ID(CActionRobotMovement2D));
	registerClass(CLASS_ID(CActionRobotMovement3D));

	registerClass(CLASS_ID(CObservationSkeleton));

	registerClass(CLASS_ID(TMapGenericParams));
#endif
}

void mrpt::obs::registerAllClasses_mrpt_obs()
{
	::registerAllClasses_mrpt_obs();
	// deps:
	mrpt::opengl::registerAllClasses_mrpt_opengl();
	mrpt::tfest::registerAllClasses_mrpt_tfest();
}
