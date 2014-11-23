#ifndef __GEDITORWINDOW_H_
#define __GEDITORWINDOW_H_

#include <wx/wx.h>
#include "wx/window.h"


class GEditorD3DPanel : public wxPanel
{
public:
    GEditorD3DPanel( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT( "Fuck" ) );

    ~GEditorD3DPanel();
	void HandleKeyDownEvent( wxKeyEvent& event );
	void HandleKeyUpEvent( wxKeyEvent& event );
	void HandleMouseWheel( wxMouseEvent& event );

protected:
    void OnPaint( wxPaintEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnEraseBackground( wxEraseEvent& event );
    void OnIdle( wxIdleEvent& event );
	void HandleLeftClick( wxMouseEvent& i_event );
	void HandleRightClick( wxMouseEvent& i_event );
	void HandleMouseMotion( wxMouseEvent& i_event );
	void HandleWindowLeft( wxMouseEvent& i_event );
	void HandleGainFocus( wxFocusEvent& event );

private:
    void InitD3D();
    void ResetProjectionMode();
    void Render();
    bool m_init;

    DECLARE_EVENT_TABLE()
};

#endif