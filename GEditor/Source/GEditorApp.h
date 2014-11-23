//---------------------------------------------------------------------------
//
// Name:        GEditorApp.h
// Author:      Jofu
// Created:     2/3/2014 9:16:01 PM
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __GEDITORFRAMEApp_h__
#define __GEDITORFRAMEApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class GEditorFrame;

class GEditorFrameApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
		int FilterEvent( wxEvent& i_event );

	GEditorFrame* m_frame;
};

#endif
