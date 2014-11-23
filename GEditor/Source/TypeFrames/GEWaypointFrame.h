#ifndef __GEWAYPOINTFRAME_H_
#define __GEWAYPOINTFRAME_H_

#include "../GETypeFrame.h"
#include "../GEditorFrame.h"
#include <vector>

class GEWaypointFrame : public GETypeFrame
{
private:
	DECLARE_EVENT_TABLE();

public:

	GEWaypointFrame( wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Waypoint Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = GEditorFrame_STYLE )
		: GETypeFrame( parent, id, title, pos, size, style )
	{
		CreateGUIControls();
	}

	enum
	{
		ID_ATTR_PANEL_MAIN = 5,
		ID_ATTR_TEXTCTRL_ONENTER,
		ID_ATTR_TEXTCTRL_ONEXIT,
		ID_ATTR_MODEL_BROWSE,
	};

	void BrowseModel( wxCommandEvent& event );
	void UpdateMainPanelUI( wxUpdateUIEvent& event );
	void CreateGUIControls( );
	virtual void DisplayEditorObject( GEditorObject* i_object );
	void UpdateOnEnter(wxCommandEvent& event);
	void UpdateOnExit(wxCommandEvent& event);
	//void OnCloseTypeFrame( wxCloseEvent& event );

    virtual ~GEWaypointFrame();

	wxPanel*		Panel_Main;
	wxStaticText*	StaticText_OnEnter;
	wxTextCtrl*		TextCtrl_OnEnter;

	wxStaticText*	StaticText_OnExit;
	wxTextCtrl*		TextCtrl_OnExit;

};

#endif