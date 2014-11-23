///-----------------------------------------------------------------
///
/// @file      GEditorFrm.cpp
/// @author    Jofu
/// Created:   2/3/2014 9:16:02 PM
/// @section   DESCRIPTION
///            GEditorFrame class implementation
///
///------------------------------------------------------------------

#include "GEditorFrm.h"
#include "GEditorD3DPanel.h"
#include "GEditorScene.h"
#include "GEditorObject.h"
#include "TypeFrames/GECharacterFrame.h"
#include "TypeFrames/GETriggerFrame.h"
#include "TypeFrames/GEStaticFrame.h"

//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// GEditorFrame
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(GEditorFrame,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(GEditorFrame::OnClose)
	EVT_KEY_UP(GEditorFrame::GEditorFrameKeyUp)
	EVT_KEY_DOWN(GEditorFrame::GEditorFrameKeyDown)

	EVT_TEXT_ENTER(ID_PROPERTY_TRANSLATE_Z, GEditorFrame::Property_Translate_ZUpdated)
	EVT_TEXT_ENTER(ID_PROPERTY_TRANSLATE_Y, GEditorFrame::Property_Translate_YUpdated)
	EVT_TEXT_ENTER(ID_PROPERTY_TRANSLATE_X, GEditorFrame::Property_Translate_XUpdated)

	EVT_TEXT_ENTER(ID_PROPERTY_ROTATE_Z, GEditorFrame::Property_Rotate_ZUpdated)
	EVT_TEXT_ENTER(ID_PROPERTY_ROTATE_Y, GEditorFrame::Property_Rotate_YUpdated)
	EVT_TEXT_ENTER(ID_PROPERTY_ROTATE_X, GEditorFrame::Property_Rotate_XUpdated)

	EVT_TEXT_ENTER(ID_PROPERTY_SCALE_Z, GEditorFrame::Property_Scale_ZUpdated)
	EVT_TEXT_ENTER(ID_PROPERTY_SCALE_Y, GEditorFrame::Property_Scale_YUpdated)
	EVT_TEXT_ENTER(ID_PROPERTY_SCALE_X, GEditorFrame::Property_Scale_XUpdated)

	//OBJECT NAME
	EVT_TEXT_ENTER( ID_PROPERTY_NAME, GEditorFrame::SetObjectName )

	EVT_MENU( ID_CREATE_TRIGGER, GEditorFrame::Menu_Create_Trigger )
	EVT_MENU( ID_FILE_OPEN, GEditorFrame::FileOpen )
	EVT_MENU( ID_FILE_EXPORT, GEditorFrame::FileOpen )
	EVT_MENU( ID_FILE_EXPORTAS, GEditorFrame::ExportAs )
	EVT_MENU( ID_FILE_SAVE, GEditorFrame::QuickSave )
	EVT_MENU( ID_FILE_SAVEAS, GEditorFrame::SaveAs )
	EVT_MENU( ID_CREATE_STATIC, GEditorFrame::CreateStatic )
	EVT_MENU( ID_FILE_NEW, GEditorFrame::NewMap )

	// OBJECT TYPE CHANGE
	EVT_CHOICE( ID_PROPERTY_TYPE, GEditorFrame::Property_Type_Changed )

	// ATTRIBUTES BUTTON
	EVT_BUTTON( ID_PROPERTY_ATTRIBUTES, GEditorFrame::Property_Attributes_Clicked )

	EVT_UPDATE_UI(ID_D3DPANEL,GEditorFrame::D3DPanelUpdateUI)
	
	EVT_UPDATE_UI(ID_PROPERTYPANEL,GEditorFrame::PropertyPanelUpdateUI)

END_EVENT_TABLE()
////Event Table End

GEditorFrame::GEditorFrame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
	m_typeFrame = NULL;
}

GEditorFrame::~GEditorFrame()
{
}

void GEditorFrame::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	PropertyPanel = new wxPanel(this, ID_PROPERTYPANEL, wxPoint(0, 0), wxSize(211, 662));

	D3DPanel = new GEditorD3DPanel(this, ID_D3DPANEL, wxPoint(211, 0), wxSize(1073, 662));

	WxFilePickerCtrl1 = new wxFilePickerCtrl(this, ID_WXFILEPICKERCTRL1, _(""), _("Select a file"), _("*.*"), wxPoint(11, 8), wxSize(7, 2), wxFLP_DEFAULT_STYLE, wxDefaultValidator, _("WxFilePickerCtrl1"));

	WxMenuBar1 = new wxMenuBar();
	wxMenu *ID_FILE_Mnu_Obj = new wxMenu();
	ID_FILE_Mnu_Obj->Append(ID_FILE_OPEN, _("Open.."), _(""), wxITEM_NORMAL);
	ID_FILE_Mnu_Obj->AppendSeparator();
	ID_FILE_Mnu_Obj->Append(ID_FILE_SAVE, _("Save Map"), _(""), wxITEM_NORMAL);
	ID_FILE_Mnu_Obj->Append(ID_FILE_SAVEAS, _("Save Map As.."), _(""), wxITEM_NORMAL);
	ID_FILE_Mnu_Obj->Append(ID_FILE_EXPORT, _("Export"), _(""), wxITEM_NORMAL);
	ID_FILE_Mnu_Obj->Append(ID_FILE_EXPORTAS, _("Export as.."), _(""), wxITEM_NORMAL);
	ID_FILE_Mnu_Obj->Append(ID_FILE_NEW, _("New Map"), _(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_FILE_Mnu_Obj, _("File"));
	
	wxMenu *ID_CREATE_Mnu_Obj = new wxMenu();
	ID_CREATE_Mnu_Obj->Append(ID_CREATE_STATIC, _("Static"), _(""), wxITEM_NORMAL);
	ID_CREATE_Mnu_Obj->Append(ID_CREATE_TRIGGER, _("Trigger"), _(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_CREATE_Mnu_Obj, _("Create"));
	SetMenuBar(WxMenuBar1);

	// PROPERTIES
	WxStaticTextProperties = new wxStaticText(PropertyPanel, ID_WXSTATICTEXT1, _("Properties"), wxPoint(65, 5), wxDefaultSize, 0, _("WxStaticTextProperties"));

	// NAME
	WxStaticTextName = new wxStaticText(PropertyPanel, ID_WXSTATICTEXT1, _("Name"), wxPoint(12, 30), wxDefaultSize, 0, _("WxStaticTextTranslation"));
	Property_Object_Name = new wxTextCtrl(PropertyPanel, ID_PROPERTY_NAME, _("Empty"), wxPoint(62, 30), wxSize(126, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Object_Name"));

	// TRANSLATE
	WxStaticTextTranslation = new wxStaticText(PropertyPanel, ID_WXSTATICTEXT1, _("Translation"), wxPoint(12, 58), wxDefaultSize, 0, _("WxStaticTextTranslation"));
	Property_Translate_X = new wxTextCtrl(PropertyPanel, ID_PROPERTY_TRANSLATE_X, _("0.0"), wxPoint(12, 80), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Translate_X"));
	Property_Translate_Y = new wxTextCtrl(PropertyPanel, ID_PROPERTY_TRANSLATE_Y, _("0.0"), wxPoint(73, 80), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Translate_Y"));
	Property_Translate_Z = new wxTextCtrl(PropertyPanel, ID_PROPERTY_TRANSLATE_Z, _("0.0"), wxPoint(138, 80), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Translate_Z"));

	// SCALE
	WxStaticTextScale = new wxStaticText(PropertyPanel, ID_WXSTATICTEXT2, _("Scale"), wxPoint(12, 105), wxDefaultSize, 0, _("WxStaticTextScale"));
	Property_Scale_X = new wxTextCtrl(PropertyPanel, ID_PROPERTY_SCALE_X, _("0.0"), wxPoint(12, 125), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Scale_X"));
	Property_Scale_Y = new wxTextCtrl(PropertyPanel, ID_PROPERTY_SCALE_Y, _("0.0"), wxPoint(73, 125), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Scale_Y"));
	Property_Scale_Z = new wxTextCtrl(PropertyPanel, ID_PROPERTY_SCALE_Z, _("0.0"), wxPoint(138, 125), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Scale_Z"));

	// ROTATE
	WxStaticTextRotate = new wxStaticText(PropertyPanel, ID_WXSTATICTEXT1, _("Rotate"), wxPoint(12, 150), wxDefaultSize, 0, _("WxStaticTextRotate"));
	Property_Rotate_X = new wxTextCtrl(PropertyPanel, ID_PROPERTY_TRANSLATE_X, _("0.0"), wxPoint(12, 170), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Rotate_X"));
	Property_Rotate_Y = new wxTextCtrl(PropertyPanel, ID_PROPERTY_TRANSLATE_Y, _("0.0"), wxPoint(73, 170), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Rotate_Y"));
	Property_Rotate_Z = new wxTextCtrl(PropertyPanel, ID_PROPERTY_TRANSLATE_Z, _("0.0"), wxPoint(138, 170), wxSize(50, 20), wxTE_PROCESS_ENTER, wxDefaultValidator, _("Property_Rotate_Z"));

	// TYPE
	WxStaticTypeName = new wxStaticText(PropertyPanel, ID_WXSTATICTEXT1, _("Type"), wxPoint(12, 195), wxDefaultSize, 0, _("WxStaticTypeName"));
	Property_Type = new wxChoice( PropertyPanel, ID_PROPERTY_TYPE, wxPoint( 12, 215 ), wxSize( 80, 20 ), 0, NULL, 0, wxDefaultValidator, _("Property_Type"));
	Property_Type->SetCanFocus( false );

	// ATTRIBUTES
	Property_Button_Attributes = new wxButton( PropertyPanel, ID_PROPERTY_ATTRIBUTES, "Attributes", wxPoint(100, 215), wxSize( 80, 20 ), 0, wxDefaultValidator, _("Property_Button_Attributes") );

	wxArrayString items;
    items.Add(wxT("Static"));
    items.Add(wxT("Trigger"));
    items.Add(wxT("Character"));
    items.Add(wxT("WayPoint"));
	Property_Type->Insert( items, 0 );

	// ADVANCED PROPERTIES

	SetTitle(_("X - The Author"));
	SetIcon(wxNullIcon);
	SetSize(0,0,1300,700);
	Center();

	D3DPanel->SetFocus();
	
	////GUI Items Creation End
}

void GEditorFrame::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * D3DPanelUpdateUI
 */
void GEditorFrame::D3DPanelUpdateUI(wxUpdateUIEvent& event)
{
	// insert your code here
}

/*
 * GEditorFrameKeyUp
 */
void GEditorFrame::GEditorFrameKeyUp(wxKeyEvent& event)
{
	// insert your code here
	D3DPanel->HandleKeyUpEvent( event );
}

/*
 * GEditorFrameKeyDown
 */
void GEditorFrame::GEditorFrameKeyDown(wxKeyEvent& event)
{
	// insert your code here
	D3DPanel->HandleKeyDownEvent( event );
}

void GEditorFrame::GEditorFrameWheel( wxMouseEvent& i_event )
{
	D3DPanel->HandleMouseWheel( i_event );
	i_event.StopPropagation();
}

/*
 * Mnutrigger1016Click
 */
void GEditorFrame::Menu_Create_Trigger(wxCommandEvent& event)
{
	GEditorObject* selectedObj = g_EditorScene::Get().CreateTrigger();
	// insert your code here
	DisplayEditorObject( selectedObj );

}

void GEditorFrame::CreateStatic( wxCommandEvent& event )
{
	GEditorObject* selectedObj = g_EditorScene::Get().CreateStatic();
	DisplayEditorObject( selectedObj );
}

/*
 * PropertyPanelUpdateUI
 */
void GEditorFrame::PropertyPanelUpdateUI(wxUpdateUIEvent& event)
{
	// insert your code here
}

void GEditorFrame::SetObjectName( wxCommandEvent& event )
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj && g_EditorScene::Get().m_selectedObjects.size() == 1 )
	{
		char buf[100];
		strcpy( buf, (const char*)Property_Object_Name->GetValue().mb_str(wxConvUTF8) ); // buf will now contain "Hello world"
		obj->SetName( buf );
	}
}

/*
 * Property_Translate_XUpdated
 */
void GEditorFrame::Property_Translate_XUpdated(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			const char* data = Property_Translate_X->GetValue().mb_str();
			double what;
			Property_Translate_X->GetValue().ToDouble( &what );
			GVector3 x( (float)what, obj->m_position._y, obj->m_position._z );
			obj->UpdatePosition( x );
		}
	}
	event.Skip();
}

/*
 * Property_Translate_YUpdated
 */
void GEditorFrame::Property_Translate_YUpdated(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			const char* data = Property_Translate_X->GetValue().mb_str();
			double what;
			Property_Translate_Y->GetValue().ToDouble( &what );
			GVector3 y( obj->m_position._x, (float)what, obj->m_position._z );
			obj->UpdatePosition( y );
		}
	}
	event.Skip();
}

/*
 * Property_Translate_ZUpdated
 */
void GEditorFrame::Property_Translate_ZUpdated(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			const char* data = Property_Translate_X->GetValue().mb_str();
			double what;
			Property_Translate_Z->GetValue().ToDouble( &what );
			GVector3 z( obj->m_position._x, obj->m_position._y, (float)what );
			obj->UpdatePosition( z );
		}
	}
	event.Skip();
}


void GEditorFrame::Property_Scale_XUpdated(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			const char* data = Property_Scale_X->GetValue().mb_str();
			double what;
			Property_Scale_X->GetValue().ToDouble( &what );
			GVector3 x( (float)what, obj->m_scale._y, obj->m_scale._z );
			obj->UpdateScale( x );
		}
	}
	event.Skip();
}

void GEditorFrame::Property_Scale_YUpdated(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			const char* data = Property_Scale_Y->GetValue().mb_str();
			double what;
			Property_Scale_Y->GetValue().ToDouble( &what );
			GVector3 y( obj->m_scale._x, (float)what, obj->m_scale._z );
			obj->UpdateScale( y );
		}
	}
	event.Skip();
}

void GEditorFrame::Property_Scale_ZUpdated(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			const char* data = Property_Scale_Z->GetValue().mb_str();
			double what;
			Property_Scale_Z->GetValue().ToDouble( &what );
			GVector3 z( obj->m_scale._x, obj->m_scale._y, (float)what );
			obj->UpdateScale( z );
		}
	}
	event.Skip();
}

// ROTATION

void GEditorFrame::Property_Rotate_XUpdated(wxCommandEvent& event)
{

}
void GEditorFrame::Property_Rotate_YUpdated(wxCommandEvent& event)
{

}
void GEditorFrame::Property_Rotate_ZUpdated(wxCommandEvent& event)
{

}


// need to make this update the editor object...
void GEditorFrame::Property_Type_Changed(wxCommandEvent& event)
{
	GEditorObject* obj = g_EditorScene::Get().m_selectedObject;
	if( obj )
	{
		obj->SetTypeData( (GEditorObject::GObjectType) Property_Type->GetSelection() );
		DisplayTypeFrame( obj );
	}

	D3DPanel->SetFocus();
}

void GEditorFrame::Property_Attributes_Clicked( wxCommandEvent& event )
{
	if( !m_typeFrame || (g_EditorScene::Get().m_selectedObject && g_EditorScene::Get().m_selectedObjects.size() == 1) )
	{
		DisplayTypeFrame( g_EditorScene::Get().m_selectedObject );
	}
	else if( (g_EditorScene::Get().m_selectedObject && g_EditorScene::Get().m_selectedObjects.size() == 1) ) 
	{
		wxString msg("No object or too many objects select.  Please select one object.");
		wxMessageDialog dialog(this, msg, wxT("Selected files"));
		dialog.ShowModal();
		return;
	}
}

void GEditorFrame::DisplayEditorObject( GEditorObject* i_object )
{
	char buffer[64];
	// Translation
	sprintf( buffer, "%f", i_object->m_position._x );
	Property_Translate_X->ChangeValue( buffer );
	sprintf( buffer, "%f", i_object->m_position._y );
	Property_Translate_Y->ChangeValue( buffer );
	sprintf( buffer, "%f", i_object->m_position._z );
	Property_Translate_Z->ChangeValue( buffer );

	// Scale
	sprintf( buffer, "%f", i_object->m_scale._x );
	Property_Scale_X->ChangeValue( buffer );
	sprintf( buffer, "%f", i_object->m_scale._y );
	Property_Scale_Y->ChangeValue( buffer );
	sprintf( buffer, "%f", i_object->m_scale._z );
	Property_Scale_Z->ChangeValue( buffer );

	// Rotate
	sprintf( buffer, "%f", 0.0f );
	Property_Rotate_X->ChangeValue( buffer );
	sprintf( buffer, "%f", i_object->m_yOrientation );
	Property_Rotate_Y->ChangeValue( buffer );
	sprintf( buffer, "%f", 0.0f );
	Property_Rotate_Z->ChangeValue( buffer );

	Property_Object_Name->ChangeValue( i_object->m_name );
	Property_Type->SetSelection( i_object->m_type );

	if( m_typeFrame )
		DisplayTypeFrame( i_object );

}

void GEditorFrame::DisplayTypeFrame( GEditorObject* i_obj )
{
	GETypeFrame* newTypeFrame = NULL;
	switch( g_EditorScene::Get().m_selectedObject->m_type )
	{
		case GEditorObject::CHARACTER:
			newTypeFrame = new GECharacterFrame( this );
		break;
		case GEditorObject::TRIGGER:
			newTypeFrame = new GETriggerFrame( this );
		break;
		case GEditorObject::STATIC:
			newTypeFrame = new GEStaticFrame( this );
		break;
		default:
			newTypeFrame = 0;
		break;
	}
	if( newTypeFrame )
	{
		if( m_typeFrame )
			delete m_typeFrame;

		m_typeFrame = newTypeFrame;
		m_typeFrame->DisplayEditorObject( g_EditorScene::Get().m_selectedObject );
		//windowList->Append( m_attrFrame );  
		m_typeFrame->SetWindowStyleFlag( wxSTAY_ON_TOP | GEditorFrame_STYLE );
		m_typeFrame->Show( true );
		m_typeFrame->SetWindowStyleFlag( wxSTAY_ON_TOP | GEditorFrame_STYLE );
	}
}

void GEditorFrame::Property_Translate_XKillFocus( wxFocusEvent& event )
{

}

void GEditorFrame::FileOpen( wxCommandEvent& event )
{

	g_EditorScene::Get().UnloadScene();
    static wxString s_extDef;
    wxString path = wxFileSelector( wxT("Select a map to load."),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef, wxT("Bro Map (*.bro)|*.bro"),
                                    wxFD_OPEN,
                                    this );

    if ( !path )
        return;

	if( !g_EditorScene::Get().LoadLuaScene( path.mb_str() ) )
	{
		wxString msg("Could not load map file.  Possible corruption.");
		wxMessageDialog dialog(this, msg, wxT("Selected files"));
		dialog.ShowModal();
	}



    // it is just a sample, would use wxSplitPath in real program
    //s_extDef = path.AfterLast(wxT('.'));

	/*
    wxLogMessage(wxT("You selected the file '%s', remembered extension '%s'"),
                 path, s_extDef);
	*/
}

void GEditorFrame::QuickExport( wxCommandEvent& event )
{
	g_EditorScene::Get().QuickExport();
}

void GEditorFrame::ExportAs( wxCommandEvent& event )
{

    static wxString s_extDef;
    wxString path = wxFileSelector( wxT("Select a map to export."),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef, wxT("Bro Map (*.brox)|*.brox"),
                                    wxFD_SAVE,
                                    this );
    if ( !path )
        return;

	if( !g_EditorScene::Get().ExportMapFiles( path.mb_str() ) )
	{
		wxString msg("Could not export map file.  Jon messed up.");
		wxMessageDialog dialog(this, msg, wxT("Selected files"));
		dialog.ShowModal();
	}
}


void GEditorFrame::SaveAs( wxCommandEvent& event )
{
    static wxString s_extDef;
    wxString path = wxFileSelector( wxT("Select a map to export."),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef, wxT("Bro Map (*.bro)|*.bro"),
                                    wxFD_SAVE,
                                    this );
    if ( !path )
        return;

	if( !g_EditorScene::Get().SaveMapFile( path.mb_str() ) )
	{
		wxString msg("Could not save map file.  Jon messed up.");
		wxMessageDialog dialog(this, msg, wxT("Selected files"));
		dialog.ShowModal();
	}
}

void GEditorFrame::QuickSave( wxCommandEvent& event )
{
	g_EditorScene::Get().QuickSave();
}

void GEditorFrame::NewMap( wxCommandEvent& event )
{
	g_EditorScene::Get().LoadBlankScene();
}

