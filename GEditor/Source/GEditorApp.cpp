//---------------------------------------------------------------------------
//
// Name:        GEditorApp.cpp
// Author:      Jofu
// Created:     2/3/2014 9:16:01 PM
// Description: 
//
//---------------------------------------------------------------------------

#include "GEditorApp.h"
#include "GEditorFrame.h"

IMPLEMENT_APP(GEditorFrameApp)

bool GEditorFrameApp::OnInit()
{
    m_frame = new GEditorFrame(NULL);
    SetTopWindow( m_frame );
    m_frame->Show();
    return true;
}

int GEditorFrameApp::FilterEvent( wxEvent& event )
{
    if ((event.GetEventType() == wxEVT_KEY_DOWN) )
    {
        m_frame->GEditorFrameKeyDown( (wxKeyEvent&)event );
    }
	else if( (event.GetEventType() == wxEVT_KEY_UP) )
	{
        m_frame->GEditorFrameKeyUp( (wxKeyEvent&)event );
	}
	else if( (event.GetEventType() == wxEVT_MOUSEWHEEL) )
	{
		m_frame->GEditorFrameWheel( (wxMouseEvent&)event );
	}

	return -1;
}
 
int GEditorFrameApp::OnExit()
{
	return 0;
}
