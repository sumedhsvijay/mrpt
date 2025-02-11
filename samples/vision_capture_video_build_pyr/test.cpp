/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include <mrpt/gui.h>
#include <mrpt/hwdrivers/CCameraSensor.h>
#include <mrpt/img/TColor.h>
#include <mrpt/opengl/Scene.h>
#include <mrpt/vision/CFeatureExtraction.h>
#include <mrpt/vision/CImagePyramid.h>
#include <mrpt/vision/TKeyPoint.h>
#include <mrpt/vision/types.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace mrpt;
using namespace mrpt::gui;
using namespace mrpt::opengl;
using namespace mrpt::obs;
using namespace mrpt::vision;
using namespace mrpt::img;

// ------------------------------------------------------
//				TestVideoBuildPyr
// ------------------------------------------------------
void TestVideoBuildPyr()
{
	size_t N_OCTAVES = 4;
	bool do_smooth = false;
	bool do_grayscale = false;

	// Ask for a different number of octaves:
	cout << "Number of octaves to use [4]: ";
	{
		std::string s;
		std::getline(cin, s);
		int i = atoi(s.c_str());
		if (i > 0) N_OCTAVES = i;
	}

	// Show to the user a list of possible camera drivers and creates and open
	// the selected camera.
	cout << "Please, select the input video file or camera...\n";

	mrpt::hwdrivers::CCameraSensor::Ptr cam =
		mrpt::hwdrivers::prepareVideoSourceFromUserSelection();
	if (!cam) return;

	cout << "Video stream open OK\n";

	// Create 3D window:
	CDisplayWindow3D win("Demo of pyramid building from live video", 800, 600);

	//  Get the smart pointer to the main viewport object in this window,
	//   and create other viewports for the smaller images:
	std::vector<Viewport::Ptr> gl_views(N_OCTAVES);
	{
		Scene::Ptr& theScene = win.get3DSceneAndLock();
		gl_views[0] = theScene->getViewport("main");
		ASSERT_(gl_views[0]);

		// Create the other viewports:
		for (size_t i = 1; i < N_OCTAVES; i++)
			gl_views[i] = theScene->createViewport(format("view_%i", (int)i));

		// Assign sizes:
		//  It can be shown mathematically than if we want all viewports to be
		//  one next to each other
		//  horizontally so they fit to the viewport width (="1") and each is
		//  the half the previous one,
		//  the first one must have a width of 2^(n-1)/(2^n - 1)
		const double W0 =
			(double(1 << (N_OCTAVES - 1))) / ((1 << N_OCTAVES) - 1);

		double X = 0;
		double W = W0;
		for (size_t i = 0; i < N_OCTAVES; i++)
		{
			Viewport* vw = gl_views[i].get();
			vw->setViewportPosition(X, .0, W, 1.);
			// cout << "Created viewport " << i << " at X=" << X << " with
			// Width=" << W << endl;
			X += W;
			W *= 0.5;
		}

		// IMPORTANT!!! IF NOT UNLOCKED, THE WINDOW WILL NOT BE UPDATED!
		win.unlockAccess3DScene();
	}

	win.setPos(10, 10);

	win.addTextMessage(
		0.51, 5,  // X,Y<=1 means coordinates are factors over the entire
		// viewport area.
		"Keys: 's'=Smoothing, 'g': Grayscale 'f': Features",
		10	// An arbitrary ID
	);

	// The image pyramid: Initially empty
	CImagePyramid imgpyr;

	cout << "Close the window to end.\n";
	while (win.isOpen())
	{
		win.addTextMessage(5, 5, format("%.02fFPS", win.getRenderingFPS()));
		std::this_thread::sleep_for(1ms);

		// Grab new video frame:
		CObservation::Ptr obs = cam->getNextFrame();
		if (obs)
		{
			if (IS_CLASS(*obs, CObservationImage))
			{
				// Get the observation object:
				CObservationImage::Ptr o =
					std::dynamic_pointer_cast<CObservationImage>(obs);

				// Update pyramid:
				imgpyr.buildPyramidFast(
					o->image,  // This image is destroyed since we are calling
					// the *Fast() version
					N_OCTAVES, do_smooth, do_grayscale);

				win.get3DSceneAndLock();

				for (size_t i = 0; i < N_OCTAVES; i++)
				{
					Viewport* vw = gl_views[i].get();
					vw->setImageView(imgpyr.images[i]);
				}

				win.addTextMessage(
					0.51, 25,  // X,Y<=1 means coordinates are factors over the
					// entire viewport area.
					format(
						"Smooth=%i Grayscale=%i", int(do_smooth ? 1 : 0),
						int(do_grayscale ? 1 : 0)),
					11	// An arbitrary ID
				);

				win.unlockAccess3DScene();
				win.repaint();
			}

			if (win.keyHit())
			{
				mrptKeyModifier kmods;
				int key = win.getPushedKey(&kmods);

				if (key == MRPTK_ESCAPE) break;

				if (key == 's' || key == 'S') do_smooth = !do_smooth;
				if (key == 'g' || key == 'G') do_grayscale = !do_grayscale;
			}
		}
	}
}

// ------------------------------------------------------
//						MAIN
// ------------------------------------------------------
int main()
{
	try
	{
		TestVideoBuildPyr();

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "MRPT error: " << mrpt::exception_to_str(e) << std::endl;
		return -1;
	}
}
