#include "GEWaypointFrame.h"
#include "../GEditorScene.h"
#include "../ObjectTypes/GETriggerData.h"
#include "../GEditorObject.h"
#include "../GEditorFrame.h"

BEGIN_EVENT_TABLE( GEWaypointFrame, GETypeFrame )
	EVT_UPDATE_UI(ID_ATTR_PANEL_MAIN, GEWaypointFrame::UpdateMainPanelUI )
	EVT_TEXT_ENTER(ID_ATTR_TEXTCTRL_ONENTER, GEWaypointFrame::UpdateOnEnter )
	EVT_TEXT_ENTER(ID_ATTR_TEXTCTRL_ONEXIT, GEWaypointFrame::UpdateOnExit )
	//EVT_CLOSE( GEditorFrame::OnCloseTypeFrame )
END_EVENT_TABLE()

GEWaypointFrame::~GEWaypointFrame()
{
	GEditorFrame* theParent = (GEditorFrame*) m_parent;
	theParent->m_typeFrame = NULL;
}

void GEWaypointFrame::CreateGUIControls()
{
	Panel_Main = new wxPanel(this, ID_ATTR_PANEL_MAIN, wxPoint(0, 0), wxSize(800, 600));

	// on enter.
	StaticText_OnEnter = new wxStaticText( Panel_Main, 1, _("On Enter"), wxPoint(15, 5), wxDefaultSize, 0, _("StaticText_OnEnter"));
	TextCtrl_OnEnter = new wxTextCtrl( Panel_Main, ID_ATTR_TEXTCTRL_ONENTER, _(""), wxPoint(15, 30), wxSize(200, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("TextCtrl_OnEnter"));

	// on exit.
	StaticText_OnExit = new wxStaticText( Panel_Main, 1, _("On Exit"), wxPoint(250, 5), wxDefaultSize, 0, _("StaticText_OnExit"));
	TextCtrl_OnExit = new wxTextCtrl( Panel_Main, ID_ATTR_TEXTCTRL_ONEXIT, _(""), wxPoint(250, 30), wxSize(200, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("TextCtrl_OnExit"));

	SetIcon(wxNullIcon);
	SetSize(0,0,800,600);
	Center();
}

void GEWaypointFrame::DisplayEditorObject( GEditorObject* i_object )
{
	//TextCtrl_FbxName->SetValue( i_object->m_fbxFileName );
	if( i_object->m_type == GEditorObject::TRIGGER )
	{
		GETriggerData* data = (GETriggerData*) i_object->m_typeData;
		if( data->m_onEnters.size() > 0 )
			TextCtrl_OnEnter->SetValue( data->m_onEnters[0] );

		if( data->m_onExits.size() > 0 )
			TextCtrl_OnEnter->SetValue( data->m_onExits[0] );
	}
}

void GEWaypointFrame::UpdateMainPanelUI( wxUpdateUIEvent& event )
{

}

void GEWaypointFrame::UpdateOnEnter(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			// get the type data.
			if( obj->m_type == GEditorObject::TRIGGER )
			{
				char buf[100];
				strcpy( buf, (const char*)TextCtrl_OnEnter->GetValue().mb_str(wxConvUTF8) ); // buf will now contain "Hello world"

				GETriggerData* data = (GETriggerData*) obj->m_typeData;
				if( data )
					data->SetOnEnter( buf );
			}
		}
	}
}

void GEWaypointFrame::UpdateOnExit(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			// get the type data.
			if( obj->m_type == GEditorObject::TRIGGER )
			{
				char buf[100];
				strcpy( buf, (const char*)TextCtrl_OnEnter->GetValue().mb_str(wxConvUTF8) ); // buf will now contain "Hello world"

				GETriggerData* data = (GETriggerData*) obj->m_typeData;
				if( data )
					data->SetOnExit( buf );
			}
		}
	}
}