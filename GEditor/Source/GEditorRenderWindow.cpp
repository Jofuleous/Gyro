#include "GEditorRenderWindow.h"
#include "RenderManager.h"
#include "GEditorScene.h"
#include "Utility/Debugging.h"
#include "Camera.h"
#include <WinUser.h>
#include <wx/dcclient.h>

BEGIN_EVENT_TABLE( GEditorRenderWindow, wxWindow )
    EVT_SIZE( GEditorRenderWindow::OnSize )
    EVT_PAINT( GEditorRenderWindow::OnPaint )
    EVT_ERASE_BACKGROUND( GEditorRenderWindow::OnEraseBackground )
    EVT_IDLE( GEditorRenderWindow::OnIdle )
	EVT_LEFT_DOWN( GEditorRenderWindow::HandleLeftClick )
	EVT_LEFT_UP( GEditorRenderWindow::HandleLeftClick )
	EVT_KEY_DOWN( GEditorRenderWindow::HandleKeyDownEvent )
	EVT_KEY_UP( GEditorRenderWindow::HandleKeyUpEvent )
	EVT_MOTION( GEditorRenderWindow::HandleMouseMotion )
	EVT_MOUSEWHEEL( GEditorRenderWindow::HandleMouseWheel )
	EVT_LEAVE_WINDOW( GEditorRenderWindow::HandleWindowLeft )
END_EVENT_TABLE()

GEditorRenderWindow::GEditorRenderWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
    : wxPanel(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
{
	m_init = false;
}

GEditorRenderWindow::~GEditorRenderWindow()
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

void GEditorRenderWindow::HandleLeftClick( wxMouseEvent& i_event )
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
		else
		{
			// the user is clicking on something!  ray cast.
			// convert to normalized screen coords
			DEBUG_PRINT( "windowsizex %d, windowsizey %d \n", GetClientSize().GetWidth(), GetClientSize().GetHeight() );
			DEBUG_PRINT( "x %d, y %d \n", x, y );
			float floatX;
			float floatY;
			floatX = (( 2.0f * (float)x ) / (float)GetClientSize().GetWidth()) - 1.0f;
			floatY = 1.0f - ( 2.0f * (float)y ) / (float)GetClientSize().GetHeight();
			DEBUG_PRINT( "screenx %f, screeny %f \n", floatX, floatY );

			if( g_RenderManager.GetCurrentScene() )
			{
				float xr, yr, zr;
				g_RenderManager.GetCurrentScene()->GetCamera()->GetMouseRayDirection( floatX, floatY, xr, yr, zr );
				GVector3 dir( xr, yr, zr );
				dir.Normalize();
				g_EditorScene::Get().AttemptRaySelect( dir, false );
			}
		}
	}
	else if( i_event.LeftUp() )
	{
		g_EditorScene::Get().m_editorCamera.m_LMBDown = false;
	}
}

void GEditorRenderWindow::HandleKeyDownEvent( wxKeyEvent& i_event )
{
	if( i_event.GetKeyCode() ==  WXK_ALT )
	{
		g_EditorScene::Get().m_editorCamera.m_altDown = true;
		//g_EditorScene::Get().m_editorCamera.m_mouseStartX = 0;
		//g_EditorScene::Get().m_editorCamera.m_mouseStartY = 0;
	}
}

void GEditorRenderWindow::HandleKeyUpEvent( wxKeyEvent& i_event )
{
	if( i_event.GetKeyCode() ==  WXK_ALT )
	{
		g_EditorScene::Get().m_editorCamera.m_altDown = false;
		//g_EditorScene::Get().m_editorCamera.m_mouseStartX = 0;
		//g_EditorScene::Get().m_editorCamera.m_mouseStartY = 0;
	}
}

void GEditorRenderWindow::HandleMouseMotion( wxMouseEvent& i_event )
{
	///////////////////// MOUSE MOVING //////////////////////////
	/////////////////////////////////////////////////////////////
	if( g_EditorScene::Get().m_editorCamera.m_altDown && g_EditorScene::Get().m_editorCamera.m_LMBDown )
	{
		int x = i_event.GetX();
		int y = i_event.GetY();

		//wxWindow::ScreenToClient( &x, &y );

		float dx = (float)x - g_EditorScene::Get().m_editorCamera.m_mouseStartX;
		float dy = (float)y - g_EditorScene::Get().m_editorCamera.m_mouseStartY;

		g_EditorScene::Get().m_editorCamera.m_deltaX += dx;
		g_EditorScene::Get().m_editorCamera.m_deltaY += dy;
		// DEBUG_PRINT( "dx %f, dy %f \n", dx, dy );

		g_EditorScene::Get().m_editorCamera.m_mouseStartX = x;
		g_EditorScene::Get().m_editorCamera.m_mouseStartY = y;
	}
}

void GEditorRenderWindow::HandleWindowLeft( wxMouseEvent& i_event )
{
	g_EditorScene::Get().m_editorCamera.m_LMBDown = false;
}

void GEditorRenderWindow::HandleMouseWheel( wxMouseEvent& i_event )
{
	float delta = i_event.GetWheelDelta() / 1.5f;
	delta *= (float)i_event.GetWheelRotation() / fabs( (float)i_event.GetWheelRotation() );
	g_EditorScene::Get().m_editorCamera.m_zoomDelta += delta;
}

void GEditorRenderWindow::OnIdle( wxIdleEvent &event )
{
    event.RequestMore(true);
    Render();
}

void GEditorRenderWindow::OnPaint( wxPaintEvent& WXUNUSED( event ) )
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

void GEditorRenderWindow::OnSize( wxSizeEvent& event )
{
    // Reset the D3D view aspect
    ResetProjectionMode();
}

void GEditorRenderWindow::OnEraseBackground( wxEraseEvent& WXUNUSED( event ) )
{
    // Do nothing, to avoid flashing on MSW
}

void GEditorRenderWindow::InitD3D()
{
	// initialize.
	HWND winHandle = GetHWND();
	g_RenderDevices.Initialize( winHandle, GetClientSize().GetWidth(), GetClientSize().GetHeight() );
	g_RenderManager.Initialize( GetClientSize().GetWidth(), GetClientSize().GetHeight() );
	Scene* currentScene = g_RenderManager.GetCurrentScene();
	g_EditorScene::Get().Initialize();
	currentScene->GetCamera()->m_aspectRatio = ((float)GetClientSize().GetWidth()) / ((float)GetClientSize().GetHeight());
	g_EditorScene::Get().m_editorCamera.SetAspectRatio( GetClientSize().GetWidth(), GetClientSize().GetHeight() );

	g_EditorScene::Get().LoadXMLScene( "C:/Users/Jofu/Desktop/SomeGame/Assets/EditorScenes/LuaLevel.bro" );
}

void GEditorRenderWindow::ResetProjectionMode()
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

void GEditorRenderWindow::Render()
{
	g_EditorScene::Get().m_editorCamera.Update();
	g_EditorScene::Get().Update();
	g_EditorScene::Get().Render();

	if( g_RenderManager.GetCurrentScene() )
	{
		g_RenderManager.Update( 0.0f );
		g_RenderManager.Sort();
		g_RenderManager.Draw();
		g_RenderManager.Present();
		g_RenderManager.Clear();
	}
}