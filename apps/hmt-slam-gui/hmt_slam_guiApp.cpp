/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "hmt_slam_guiApp.h"

#include <mrpt/math/math_frwds.h>

//(*AppHeaders
#include <wx/image.h>

#include "hmt_slam_guiMain.h"
//*)

IMPLEMENT_APP(hmt_slam_guiApp)

bool hmt_slam_guiApp::OnInit()
{
	// Starting in wxWidgets 2.9.0, we must reset numerics locale to "C",
	//  if we want numbers to use "." in all countries. The App::OnInit() is a
	//  perfect place to undo
	//  the default wxWidgets settings. (JL @ Sep-2009)
	wxSetlocale(LC_NUMERIC, wxString(wxT("C")));

	//(*AppInitialize
	bool wxsOK = true;
	wxInitAllImageHandlers();
	if (wxsOK)
	{
		auto* Frame = new hmt_slam_guiFrame(nullptr);
		Frame->Show();
		SetTopWindow(Frame);
	}
	//*)
	return wxsOK;
}
