///-----------------------------------------------------------------
///
/// @file      GEditorFrm.h
/// @author    Jofu
/// Created:   2/3/2014 9:16:02 PM
/// @section   DESCRIPTION
///            GEditorFrame class declaration
///
///------------------------------------------------------------------

#ifndef __GEDITORFRAME_H__
#define __GEDITORFRAME_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/filepicker.h>
#include <wx/panel.h>
////Header Include End

class GEditorD3DPanel;
class GEditorObject;
class GETypeFrame;

////Dialog Style Start
#undef GEditorFrame_STYLE
#define GEditorFrame_STYLE wxSUNKEN_BORDER | wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class GEditorFrame : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		GEditorFrame(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("X - The Author"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = GEditorFrame_STYLE);
		virtual ~GEditorFrame();
		void D3DPanelUpdateUI(wxUpdateUIEvent& event);
		void GEditorFrameKeyUp(wxKeyEvent& event);
		void GEditorFrameKeyDown(wxKeyEvent& event);
		void GEditorFrameWheel( wxMouseEvent& event );
		void Menu_Create_Trigger(wxCommandEvent& event);
		void PropertyPanelUpdateUI(wxUpdateUIEvent& event);
		void Property_Translate_XUpdated(wxCommandEvent& event);
		void Property_Translate_YUpdated(wxCommandEvent& event);
		void Property_Translate_ZUpdated(wxCommandEvent& event);

		void Property_Rotate_XUpdated(wxCommandEvent& event);
		void Property_Rotate_YUpdated(wxCommandEvent& event);
		void Property_Rotate_ZUpdated(wxCommandEvent& event);

		void Property_Scale_XUpdated(wxCommandEvent& event);
		void Property_Scale_YUpdated(wxCommandEvent& event);
		void Property_Scale_ZUpdated(wxCommandEvent& event);
		void Property_Attributes_Clicked( wxCommandEvent& event );

		// SET OBJECT NAME
		void SetObjectName(wxCommandEvent& event);

		// CREATE STATIC
		void CreateStatic( wxCommandEvent& event );

		void Property_Type_Changed(wxCommandEvent& event);

		void FileOpen( wxCommandEvent& event );
		void QuickExport( wxCommandEvent& event );
		void ExportAs( wxCommandEvent& event );
		void SaveAs( wxCommandEvent& event );
		void QuickSave( wxCommandEvent& event );
		void NewMap( wxCommandEvent& event );

		void Property_Translate_XKillFocus( wxFocusEvent& event );
		void DisplayEditorObject( GEditorObject* i_object );
		void DisplayTypeFrame( GEditorObject* i_object );


		GETypeFrame* m_typeFrame;
		
	private:
		//Do not add custom control declarations between
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start

		wxStaticText *WxStaticTextScale;
		wxStaticText *WxStaticTextTranslation;
		wxStaticText *WxStaticTextRotate;
		wxStaticText *WxStaticTextProperties;
		wxStaticText *WxStaticTextName;
		wxStaticText *WxStaticTypeName;
		wxButton*	Property_Button_Attributes;
		wxTextCtrl *Property_Translate_Z;
		wxTextCtrl *Property_Translate_Y;
		wxTextCtrl *Property_Translate_X;
		wxTextCtrl *Property_Scale_Z;
		wxTextCtrl *Property_Scale_Y;
		wxTextCtrl *Property_Scale_X;
		wxTextCtrl *Property_Rotate_Z;
		wxTextCtrl *Property_Rotate_Y;
		wxTextCtrl *Property_Rotate_X;
		wxTextCtrl *Property_Object_Name;
		wxChoice*	Property_Type;
		wxMenuBar *WxMenuBar1;
		wxFilePickerCtrl *WxFilePickerCtrl1;
		GEditorD3DPanel *D3DPanel;
		wxPanel *PropertyPanel;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXSTATICTEXT3 = 1022,
			ID_WXSTATICTEXT2 = 1021,
			ID_WXSTATICTEXT1 = 1020,
			ID_PROPERTY_TRANSLATE_Z = 1019,
			ID_PROPERTY_TRANSLATE_Y = 1018,
			ID_PROPERTY_TRANSLATE_X = 1017,
			ID_PROPERTY_ROTATE_Z = 505,
			ID_PROPERTY_ROTATE_Y = 504,
			ID_PROPERTY_ROTATE_X = 503,
			ID_PROPERTY_SCALE_Z = 502,
			ID_PROPERTY_SCALE_Y = 501,
			ID_PROPERTY_SCALE_X = 500,
			ID_PROPERTY_NAME = 499,
			ID_PROPERTY_ATTRIBUTES = 497,
			ID_FILE_SAVEAS = 496,
			ID_FILE_SAVE = 495,
			ID_FILE_NEW = 494,
			ID_FRAME_TYPE = 493,
			ID_PROPERTY_TYPE = 298,
			ID_FILE = 1006,
			ID_FILE_OPEN = 1008,
			ID_FILE_EXPORTRUN = 1010,
			ID_FILE_EXPORT = 1013,
			ID_FILE_EXPORTAS = 1014,
			ID_CREATE = 1009,
			ID_CREATE_STATIC = 1015,
			ID_CREATE_TRIGGER = 1016,
			
			ID_WXFILEPICKERCTRL1 = 1005,
			ID_D3DPANEL = 1002,
			ID_PROPERTYPANEL = 1001,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
