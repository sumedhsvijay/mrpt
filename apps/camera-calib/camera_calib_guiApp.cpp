/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "camera_calib_guiApp.h"

//(*AppHeaders
#include <wx/image.h>

#include "camera_calib_guiMain.h"
//*)

#include <wx/log.h>

IMPLEMENT_APP(camera_calib_guiApp)

bool camera_calib_guiApp::OnInit()
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
		camera_calib_guiDialog Dlg(nullptr);
		SetTopWindow(&Dlg);
		Dlg.ShowModal();
		wxsOK = false;
	}
	//*)
	return wxsOK;
}
