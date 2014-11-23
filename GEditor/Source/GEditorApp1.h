#ifndef __GEDITORMAINWINDOW_H_
#define __GEDITORMAINWINDOW_H_

#include <wx/wx.h>
#include "GEditorCamera.h"

// Define a new application type
class GEditorApp: public wxApp
{
public:
    bool OnInit();
};

class GEditorRenderWindow;

class GEditorFrame: public wxFrame
{
public:
							GEditorFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void					OnQuit(wxCommandEvent& event);
	void					OnAbout(wxCommandEvent& event);
	void					SetCanvas( GEditorRenderWindow* window ) { m_renderWindow = window; }
	GEditorRenderWindow*	GetRenderWindow() { return m_renderWindow; }

	DECLARE_EVENT_TABLE()

private:
	GEditorRenderWindow*	m_renderWindow;

};

#endif
