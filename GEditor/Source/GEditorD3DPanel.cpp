#include "GEditorD3DPanel.h"
#include "RenderManager.h"
#include "GEditorScene.h"
#include "Utility/Debugging.h"
#include "GEditorFrame.h"
#include "Camera.h"
#include "Utility/Clock.h"
#include <WinUser.h>
#include <wx/dcclient.h>
#include "GEditorFrame.h"

BEGIN_EVENT_TABLE( GEditorD3DPanel, wxPanel )// this used to be "wxWindow".  it might have changed some behavior...like focus.
    EVT_SIZE( GEditorD3DPanel::OnSize )
    EVT_PAINT( GEditorD3DPanel::OnPaint )
    EVT_ERASE_BACKGROUND( GEditorD3DPanel::OnEraseBackground )
    EVT_IDLE( GEditorD3DPanel::OnIdle )
	EVT_LEFT_DOWN( GEditorD3DPanel::HandleLeftClick )
	EVT_LEFT_UP( GEditorD3DPanel::HandleLeftClick )
	EVT_RIGHT_DOWN( GEditorD3DPanel::HandleRightClick )
	EVT_RIGHT_UP( GEditorD3DPanel::HandleRightClick )
	EVT_KEY_DOWN( GEditorD3DPanel::HandleKeyDownEvent )
	EVT_KEY_UP( GEditorD3DPanel::HandleKeyUpEvent )
	EVT_MOTION( GEditorD3DPanel::HandleMouseMotion )
	EVT_MOUSEWHEEL( GEditorD3DPanel::HandleMouseWheel )
	EVT_LEAVE_WINDOW( GEditorD3DPanel::HandleWindowLeft )
	EVT_SET_FOCUS( GEditorD3DPanel::HandleGainFocus )
END_EVENT_TABLE()

GEditorD3DPanel::GEditorD3DPanel( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
    : wxPanel(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
{
	m_init = false;
}

GEditorD3DPanel::~GEditorD3DPanel()
{
	g_RenderManager.Shutdown();
	/*
    if( g_pVertexBuffer != NULL )
        g_pVertexBuffer->Release(); 

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
	*/
}

void GEditorD3DPanel::HandleLeftClick( wxMouseEvent& i_event )
{
	int x = i_event.GetPosition().x;
	int y = i_event.GetPosition().y;

	//ScreenToClient( &x, &y );
	if( i_event.LeftDown() )
	{
		g_EditorScene::Get().m_editorCamera.m_LMBDown = true;

		// user is dragging the mouse on the widget.  let's track it.
		if(g_EditorScene::Get().m_editorCamera.m_altDown )
		{
			g_EditorScene::Get().m_editorCamera.m_mouseStartX = x;
			g_EditorScene::Get().m_editorCamera.m_mouseStartY = y;
		}
		else if( !g_EditorScene::Get().m_axisMarker.m_selectedAxis )
		{
			// the user is clicking on something!  ray cast.
			// convert to normalized screen coords
			//DEBUG_PRINT( "windowsizex %d, windowsizey %d \n", GetClientSize().GetWidth(), GetClientSize().GetHeight() );
			//DEBUG_PRINT( "x %d, y %d \n", x, y );
			float floatX;
			float floatY;
			floatX = (( 2.0f * (float)x ) / (float)GetClientSize().GetWidth()) - 1.0f;
			floatY = 1.0f - ( 2.0f * (float)y ) / (float)GetClientSize().GetHeight();
			//DEBUG_PRINT( "screenx %f, screeny %f \n", floatX, floatY );

			if( g_RenderManager.GetCurrentScene() )
			{
				float xr, yr, zr;
				g_RenderManager.GetCurrentScene()->GetCamera()->ScreenToWorld( floatX, floatY, xr, yr, zr );
				GVector3 dir( xr, yr, zr );
				dir.Normalize();
				g_EditorScene::Get().AttemptRaySelect( dir, false );

				/*
				if( g_EditorScene::Get().m_selectedObjects.size() > 0 )
				{
					// test for intersection with the axis marker.
					D3DXVECTOR3 hack = g_RenderManager.GetCurrentScene()->GetCamera()->m_position;
					GVector3 v( hack.x, hack.y, hack.z);
					g_EditorScene::Get().m_axisMarker.Select( v, dir );
				}
				*/
			}
		}

		g_EditorScene::Get().m_editorCamera.m_LMBStart = (float) g_Clock::Get().SecondsSinceStart();
	}
	else if( i_event.LeftUp() )
	{
		g_EditorScene::Get().m_editorCamera.m_LMBDown = false;
		g_EditorScene::Get().m_axisMarker.UnSelect();
	}
}

void GEditorD3DPanel::HandleRightClick( wxMouseEvent& i_event )
{
	if( i_event.RightDown() )
	{
		g_EditorScene::Get().m_editorCamera.m_zoomYLast = i_event.GetY();
		g_EditorScene::Get().m_editorCamera.m_RMBDown = true;
	}
	else if( i_event.RightUp() )
		g_EditorScene::Get().m_editorCamera.m_RMBDown = false;
}

void GEditorD3DPanel::HandleKeyDownEvent( wxKeyEvent& i_event )
{
	switch( i_event.GetKeyCode() )
	{
	case WXK_ALT:
		g_EditorScene::Get().m_editorCamera.m_altDown = true;
	break;
	case WXK_DELETE:
		if( g_EditorScene::Get().m_path.NodeSelected() )
			g_EditorScene::Get().m_path.DeleteNode( g_EditorScene::Get().m_path.m_selectedNode );
	break;
	case 74: //bleh.  this is "J"
		if( g_EditorScene::Get().m_path.NodeSelected() )
			g_EditorScene::Get().m_path.LinkNodes( g_EditorScene::Get().m_path.m_selectedNode, g_EditorScene::Get().m_path.m_lastSelectedNode );
	break;

	/*
	case WXK_NUMPAD2:
		g_EditorScene::Get().m_moveVector.y( -1.0f );
	break;
	case WXK_NUMPAD8:
		g_EditorScene::Get().m_moveVector.y( 1.0f );
	break;
	case WXK_NUMPAD4:
		g_EditorScene::Get().m_moveVector.x( -1.0f );
	break;
	case WXK_NUMPAD6:
		g_EditorScene::Get().m_moveVector.x( 1.0f );
	break;
	case WXK_NUMPAD7:
		g_EditorScene::Get().m_moveVector.z( -1.0f );
	break;
	case WXK_NUMPAD9:
		g_EditorScene::Get().m_moveVector.z( 1.0f );
	break;
	*/
	}

}

void GEditorD3DPanel::HandleKeyUpEvent( wxKeyEvent& i_event )
{
	switch( i_event.GetKeyCode() )
	{
		case WXK_ALT:
			g_EditorScene::Get().m_editorCamera.m_altDown = false;
		break;
		/*
		case WXK_NUMPAD2:
		case WXK_NUMPAD8:
			g_EditorScene::Get().m_moveVector.y( 0.0f );
		break;
		case WXK_NUMPAD4:
		case WXK_NUMPAD6:
			g_EditorScene::Get().m_moveVector.x( 0.0f );
		case WXK_NUMPAD7:
		case WXK_NUMPAD9:
			g_EditorScene::Get().m_moveVector.z( 0.0f );
		break;
		*/
	}
}

void GEditorD3DPanel::HandleMouseMotion( wxMouseEvent& i_event )
{
	///////////////////// MOUSE MOVING //////////////////////////
	/////////////////////////////////////////////////////////////
	int x = i_event.GetX();
	int y = i_event.GetY();

	if( g_EditorScene::Get().m_editorCamera.m_altDown && g_EditorScene::Get().m_editorCamera.m_LMBDown )
	{

		//wxWindow::ScreenToClient( &x, &y );

		float dx = (float)x - g_EditorScene::Get().m_editorCamera.m_mouseStartX;
		float dy = (float)y - g_EditorScene::Get().m_editorCamera.m_mouseStartY;

		g_EditorScene::Get().m_editorCamera.m_deltaX += dx;
		g_EditorScene::Get().m_editorCamera.m_deltaY += dy;
		// DEBUG_PRINT( "dx %f, dy %f \n", dx, dy );

		g_EditorScene::Get().m_editorCamera.m_mouseStartX = x;
		g_EditorScene::Get().m_editorCamera.m_mouseStartY = y;
	}
	else if( g_EditorScene::Get().m_editorCamera.m_RMBDown )
	{
		float deltaY = -(i_event.GetY() - g_EditorScene::Get().m_editorCamera.m_zoomYLast);
		g_EditorScene::Get().m_editorCamera.m_zoomDelta += deltaY;
		g_EditorScene::Get().m_editorCamera.m_zoomYLast = i_event.GetY();
	}
	else if( g_EditorScene::Get().m_editorCamera.m_LMBDown && g_EditorScene::Get().m_axisMarker.m_selectedAxis )
	{
		if( g_Clock::Get().SecondsSinceStart() > g_EditorScene::Get().m_editorCamera.m_LMBStart + 0.001f )
			g_EditorScene::Get().m_axisMarker.HandleMouse( x, y, GetClientSize().GetWidth(), GetClientSize().GetHeight() );
	}
}

void GEditorD3DPanel::HandleWindowLeft( wxMouseEvent& i_event )
{
	g_EditorScene::Get().m_editorCamera.m_LMBDown = false;
	g_EditorScene::Get().m_editorCamera.m_RMBDown = false;
	g_EditorScene::Get().m_editorCamera.m_MMBDown = false;
	g_EditorScene::Get().m_axisMarker.UnSelect();
}

void GEditorD3DPanel::HandleMouseWheel( wxMouseEvent& i_event )
{
	float delta = i_event.GetWheelDelta() / 1.5f;
	delta *= (float)i_event.GetWheelRotation() / fabs( (float)i_event.GetWheelRotation() );
	g_EditorScene::Get().m_editorCamera.m_zoomDelta += delta;
}

void GEditorD3DPanel::OnIdle( wxIdleEvent &event )
{
    event.RequestMore(true);
    Render();
}

void GEditorD3DPanel::OnPaint( wxPaintEvent& WXUNUSED( event ) )
{
    wxPaintDC dc(this);

    // Initialize D3D
    if ( !m_init )
    {
        InitD3D();
        ResetProjectionMode();
        m_init= true;
    }
	
}

void GEditorD3DPanel::OnSize( wxSizeEvent& event )
{
    // Reset the D3D view aspect
    ResetProjectionMode();
}

void GEditorD3DPanel::OnEraseBackground( wxEraseEvent& WXUNUSED( event ) )
{
    // Do nothing, to avoid flashing on MSW
}

void GEditorD3DPanel::InitD3D()
{
	// initialize.
	HWND winHandle = GetHWND();
	g_RenderDevices.Initialize( winHandle, GetClientSize().GetWidth(), GetClientSize().GetHeight() );
	g_RenderManager.Initialize( GetClientSize().GetWidth(), GetClientSize().GetHeight() );
	Scene* currentScene = g_RenderManager.GetCurrentScene();
	g_EditorScene::Get().Initialize( (GEditorFrame*)GetParent() );
	currentScene->GetCamera()->m_aspectRatio = ((float)GetClientSize().GetWidth()) / ((float)GetClientSize().GetHeight());
	g_EditorScene::Get().m_editorCamera.SetAspectRatio( GetClientSize().GetWidth(), GetClientSize().GetHeight() );

	g_Clock::Get().Initialize();

	//g_EditorScene::Get().LoadLuaScene( "C:/Users/Jofu/Desktop/SomeGame/Assets/EditorScenes/LuaLevel.bro" );
}

void GEditorD3DPanel::ResetProjectionMode()
{
	/*
   if(g_pd3dDevice == NULL)
	return;

    int w, h;
    GetClientSize(&w, &h);

	D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( 45.0f ), 
                                (float)w / float(h), 0.1f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	*/
	g_EditorScene::Get().m_editorCamera.SetAspectRatio( GetClientSize().GetWidth(), GetClientSize().GetHeight() );
}

void GEditorD3DPanel::Render()
{
	// LOCK
	g_EditorScene::Get().m_renderLock.Lock();
	g_EditorScene::Get().m_editorCamera.Update();
	g_EditorScene::Get().Update();
	g_EditorScene::Get().Render();
	g_Clock::Get().Update();

	if( g_RenderManager.GetCurrentScene() )
	{
		g_RenderManager.Update( 0.0f );
		g_RenderManager.Sort();
		g_RenderManager.Draw();
		g_RenderManager.Present();
		g_RenderManager.Clear();
	}

	// UNLOCK
	g_EditorScene::Get().m_renderLock.Unlock();
}

void GEditorD3DPanel::HandleGainFocus( wxFocusEvent& event )
{

}