#include "GEStaticFrame.h"
#include "../GEditorScene.h"

BEGIN_EVENT_TABLE( GEStaticFrame, GETypeFrame )
	EVT_UPDATE_UI(ID_ATTR_PANEL_MAIN, GEStaticFrame::UpdateMainPanelUI)
	EVT_BUTTON( ID_ATTR_MODEL_BROWSE, GEStaticFrame::BrowseModel )
END_EVENT_TABLE()

GEStaticFrame::~GEStaticFrame()
{
	GEditorFrame* theParent = (GEditorFrame*) m_parent;
	theParent->m_typeFrame = NULL;
}

void GEStaticFrame::CreateGUIControls()
{
	Panel_Main = new wxPanel(this, ID_ATTR_PANEL_MAIN, wxPoint(0, 0), wxSize(800, 600));
	StaticText_FbxName = new wxStaticText( Panel_Main, 1, _("Model Name"), wxPoint(15, 5), wxDefaultSize, 0, _("StaticText_FbxName"));
	TextCtrl_FbxName = new wxTextCtrl( Panel_Main, ID_ATTR_TEXTCTRL_FBX, _("None"), wxPoint(15, 30), wxSize(200, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("TextCtrl_FbxName"));
	Button_ModelBrowse = new wxButton( Panel_Main, ID_ATTR_MODEL_BROWSE, "Browse..", wxPoint(220, 30), wxSize( 50, 20 ), 0, wxDefaultValidator, _("Button_ModelBrowse") );

	SetIcon(wxNullIcon);
	SetSize(0,0,800,600);
	Center();
}

void GEStaticFrame::DisplayEditorObject( GEditorObject* i_object )
{
	TextCtrl_FbxName->SetValue( i_object->m_fbxFileName );
}

void GEStaticFrame::BrowseModel( wxCommandEvent& event )
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

void GEStaticFrame::UpdateMainPanelUI( wxUpdateUIEvent& event )
{

}