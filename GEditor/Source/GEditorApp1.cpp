#include "GEditorApp.h"
#include "GEditorRenderWindow.h"

enum
{
	ID_Quit = 1,
	ID_About,
};

BEGIN_EVENT_TABLE( GEditorFrame, wxFrame )
	EVT_MENU( ID_Quit, GEditorFrame::OnQuit )
	EVT_MENU( ID_About, GEditorFrame::OnAbout )
END_EVENT_TABLE( )

IMPLEMENT_APP( GEditorApp )

bool GEditorApp::OnInit()
{
	GEditorFrame *frame = new GEditorFrame( _T("Hello World"), wxPoint(50,50), wxSize(1200,800) );
	frame->Show( true );
	SetTopWindow(frame);

	return true;
}


GEditorFrame::GEditorFrame( const wxString& title, const wxPoint& pos, const wxSize& size )
: wxFrame( (wxFrame *)NULL, -1, title, pos, size )
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append( ID_About, _T("&About...") );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, _T("E&xit") );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _T("&File") );

	SetMenuBar( menuBar );

	m_renderWindow = new GEditorRenderWindow(this, wxID_ANY, pos,
		wxSize( size.GetWidth(), size.GetHeight() ), wxSUNKEN_BORDER);

	CreateStatusBar();
	SetStatusText( _T("Welcome to wxWidgets!") );
}

void GEditorFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void GEditorFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(_T("This is a wxWidgets D3D9 Hello world sample"),
	_T("About D3D9 Hello World"), wxOK | wxICON_INFORMATION, this);
}