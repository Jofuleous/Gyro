//---------------------------------------------------------------------------
//
// Name:        GEditorApp.cpp
// Author:      Jofu
// Created:     2/3/2014 9:16:01 PM
// Description: 
//
//---------------------------------------------------------------------------

#include "GEditorApp.h"
#include "GEditorFrm.h"

IMPLEMENT_APP(GEditorFrameApp)

bool GEditorFrameApp::OnInit()
{
    GEditorFrame* frame = new GEditorFrame(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int GEditorFrameApp::OnExit()
{
	return 0;
}
