#ifndef __GECHARACTERFRAME_H_
#define __GECHARACTERFRAME_H_

#include "../GETypeFrame.h"
#include "../GEditorFrame.h"

class GECharacterFrame : public GETypeFrame
{
private:
	DECLARE_EVENT_TABLE();

public:

	GECharacterFrame( wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Character Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = GEditorFrame_STYLE )
		: GETypeFrame( parent, id, title, pos, size, style )
	{
		CreateGUIControls();
	}

	enum
	{
		ID_ATTR_PANEL_MAIN = 0,
		ID_ATTR_TEXTCTRL_FBX,
		ID_ATTR_TEXTCTRL_CHAR,
		ID_BUTTON_APPLY,
		ID_ATTR_MODEL_BROWSE,
	};

	void BrowseModel( wxCommandEvent& event );
	void UpdateMainPanelUI( wxUpdateUIEvent& event );
	void CreateGUIControls( );
	virtual void DisplayEditorObject( GEditorObject* i_object );
	void ApplyChanges( wxCommandEvent& event );

    virtual ~GECharacterFrame();

	wxPanel*		Panel_Main;
	wxStaticText*	StaticText_FbxName;
	wxTextCtrl*		TextCtrl_FbxName;
	wxButton*		Button_ModelBrowse;
	wxButton*		Button_Apply;

	wxStaticText*	StaticText_CharName;
	wxTextCtrl*		TextCtrl_CharName;
};

#endif