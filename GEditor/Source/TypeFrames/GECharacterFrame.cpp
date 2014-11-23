#include "GECharacterFrame.h"
#include "../GEditorScene.h"
#include  "../ObjectTypes/GECharacterData.h"

BEGIN_EVENT_TABLE( GECharacterFrame, GETypeFrame )
	EVT_UPDATE_UI(ID_ATTR_PANEL_MAIN, GECharacterFrame::UpdateMainPanelUI)
	EVT_BUTTON( ID_ATTR_MODEL_BROWSE, GECharacterFrame::BrowseModel )
	EVT_BUTTON( ID_BUTTON_APPLY, GECharacterFrame::ApplyChanges )
END_EVENT_TABLE()

GECharacterFrame::~GECharacterFrame()
{
	GEditorFrame* theParent = (GEditorFrame*) m_parent;
	theParent->m_typeFrame = NULL;
}

void GECharacterFrame::CreateGUIControls()
{
	Panel_Main = new wxPanel(this, ID_ATTR_PANEL_MAIN, wxPoint(0, 0), wxSize(800, 600));
	StaticText_FbxName = new wxStaticText( Panel_Main, 1, _("Model Name"), wxPoint(15, 5), wxDefaultSize, 0, _("StaticText_FbxName"));
	TextCtrl_FbxName = new wxTextCtrl( Panel_Main, ID_ATTR_TEXTCTRL_FBX, _("None"), wxPoint(15, 30), wxSize(200, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("TextCtrl_FbxName"));
	Button_ModelBrowse = new wxButton( Panel_Main, ID_ATTR_MODEL_BROWSE, "Browse..", wxPoint(220, 30), wxSize( 50, 20 ), 0, wxDefaultValidator, _("Button_ModelBrowse") );

	// character file
	StaticText_CharName = new wxStaticText( Panel_Main, 1, _("Character File"), wxPoint(15, 50), wxDefaultSize, 0, _("StaticText_CharacterFile"));
	TextCtrl_CharName = new wxTextCtrl( Panel_Main, ID_ATTR_TEXTCTRL_FBX, _("None"), wxPoint(15, 75), wxSize(200, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("TextCtrl_CharacterFile"));

	// Apply
	Button_Apply = new wxButton( Panel_Main, ID_BUTTON_APPLY, "Apply", wxPoint(15, 110), wxSize( 80, 20 ), 0, wxDefaultValidator, _("Button_Apply") );

	SetIcon(wxNullIcon);
	SetSize(0,0,800,600);
	Center();
}

void GECharacterFrame::ApplyChanges( wxCommandEvent& event )
{
	char buf[128];
	strcpy( buf, (const char*)TextCtrl_CharName->GetValue().mb_str(wxConvUTF8) ); // buf will now contain "Hello world"
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		GECharacterData* data = (GECharacterData*) obj->m_typeData;
		data->SetCharacterFile( buf );
	}
}

void GECharacterFrame::DisplayEditorObject( GEditorObject* i_object )
{
	TextCtrl_FbxName->SetValue( i_object->m_fbxFileName );

	GECharacterData* data = (GECharacterData*) i_object->m_typeData;
	TextCtrl_CharName->SetValue( data->m_characterFile );
}

void GECharacterFrame::BrowseModel( wxCommandEvent& event )
{

    static wxString s_extDef;
    wxString path = wxFileSelector( wxT("Choose a model."),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef, wxT("FBX (*.fbx)|*.fbx"),
                                    wxFD_SAVE,
                                    this );

    if ( !path )
        return;

	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj && g_EditorScene::Get().m_selectedObjects.size() == 1 )
	{
		if( obj->LoadEntitiesFromFBX( path ) )
		{
			TextCtrl_FbxName->SetValue( path );
			const char* lol = (const char*) path;
			char* lol2 = (char*)lol;
			obj->SetFbxName( lol2 );
			return;
		}
		else
		{
			wxString msg("Could not load FBX file.  Possible corruption.");
			wxMessageDialog dialog(this, msg, wxT("Selected files"));
			dialog.ShowModal();
			return;
		}
	}
	else
	{
		wxString msg("No object or too many objects select.  Please select one object.");
		wxMessageDialog dialog(this, msg, wxT("Selected files"));
		dialog.ShowModal();
		return;
	}
}

void GECharacterFrame::UpdateMainPanelUI( wxUpdateUIEvent& event )
{

}