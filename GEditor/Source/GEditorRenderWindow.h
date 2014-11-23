#ifndef __GEDITORWINDOW_H_
#define __GEDITORWINDOW_H_

#include <wx/wx.h>
#include "wx/window.h"


class GEditorRenderWindow : public wxPanel
{
public:
    GEditorRenderWindow( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT( "Fuck" ) );

    ~GEditorRenderWindow();

protected:
    void OnPaint( wxPaintEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnEraseBackground( wxEraseEvent& event );
    void OnIdle( wxIdleEvent& event );
	void HandleLeftClick( wxMouseEvent& i_event );
	void HandleMouseMotion( wxMouseEvent& i_event );
	void HandleKeyDownEvent( wxKeyEvent& event );
	void HandleKeyUpEvent( wxKeyEvent& event );
	void HandleMouseWheel( wxMouseEvent& i_event );
	void HandleWindowLeft( wxMouseEvent& i_event );

private:
    void InitD3D();
    void ResetProjectionMode();
    void Render();
    bool m_init;

    DECLARE_EVENT_TABLE()
};

#endif