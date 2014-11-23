#include "GMouse.h"
#include <assert.h>
#include "UserSettings.h"
#include "Utility/Debugging.h"

void MousePosChanged( float i_x, float i_y, void* i_data )
{
	assert( i_data );
	GMouse* mouse = ( GMouse* ) i_data;

	mouse->HandlePosition( i_x, i_y );
}

void MouseButtonCallback( unsigned int i_message, void* i_data )
{
	assert( i_data );
	GMouse* mouse = ( GMouse* ) i_data;
	mouse->HandleButton( i_message );
}

GMouse::~GMouse()
{
	// be safe for now...
	g_MainWindow.UnRegisterMouseCallbacks( );
}

void GMouse::Shutdown()
{
	g_MainWindow.UnRegisterMouseCallbacks( );
}

void GMouse::HandleButton( u32 i_message )
{
	// set button vars.
	// WINDOWS
	switch( i_message )
	{
	case WM_LBUTTONDOWN: // 201
		m_leftUp = false;
	break;
	case WM_LBUTTONUP: // 202
		m_leftUp = true;
	break;
	case WM_RBUTTONDOWN: // 204
		m_rightUp = false;
	break;
	case WM_RBUTTONUP: // 205
		m_rightUp = true;
	break;
	case WM_MBUTTONDOWN: // 207
		m_middleUp = false;
	break;
	case WM_MBUTTONUP: // 208
		m_middleUp = true;
	break;
	}


	// for now, just pass through the message.
	for( int i = 0; i < m_fnButtonCallbacks.size(); i++ )
	{
		m_fnButtonCallbacks[i]( i_message, m_fnCallbackOwners[i] );
	}
}

void GMouse::HandlePosition( float x, float y )
{
	//POINT p;
	//GetCursorPos( &p );
	//m_x = p.x;
	//m_y = p.y;
	//m_windowX = x;
	//m_windowY = y;
	//DEBUG_PRINT( "X: %d\n", (int)x );
	//DEBUG_PRINT( "Y: %d\n", (int)y );
}

void GMouse::Initialize( )
{
	m_centerCursor = false;
	m_dx = 0;
	m_dy = 0;
	m_dxTotal = 0;
	m_dyTotal = 0;
	m_middleUp = true;
	m_leftUp = true;
	m_rightUp = true;
	m_x = 0;
	m_y = 0;
	m_windowX = 0;
	m_windowY = 0;
	g_MainWindow.RegisterMouseCallbacks( MouseButtonCallback, 0, this );
}

void GMouse::Update( )
{
	if( m_centerCursor && g_MainWindow.Active() )
	{
		POINT p;
		GetCursorPos( &p );
		m_x = p.x;
		m_y = p.y;
		//m_windowX = x;
		//m_windowY = y;

		m_dx = m_x - (g_MainWindow.GetPosX() + Gyro::UserSettings::GetScreenWidth() / 2);
		m_dy = m_y - (g_MainWindow.GetPosY() + Gyro::UserSettings::GetScreenHeight() / 2);

		m_dxTotal += m_dx;
		m_dyTotal += m_dy;
		if( m_dyTotal > (float)Gyro::UserSettings::GetScreenHeight() / 2.0f )  // this is to prevent flipping.  may need some kind of camera support...
			m_dyTotal = (float)Gyro::UserSettings::GetScreenHeight() / 2.0f;
		if( m_dyTotal < -(float)Gyro::UserSettings::GetScreenHeight() / 2.0f )
			m_dyTotal = -(float)Gyro::UserSettings::GetScreenHeight() / 2.0f;

		// center the cursor again :)
		SetCursorPos( g_MainWindow.GetPosX() + (float)Gyro::UserSettings::GetScreenWidth() / 2.0f,
		g_MainWindow.GetPosY() + (float)Gyro::UserSettings::GetScreenHeight() / 2.0f );
	}
}

void GMouse::CenterCursor( bool i_value )
{
	m_centerCursor = i_value;

	if( i_value )
	{
		SetCursorPos( g_MainWindow.GetPosX() + (float)Gyro::UserSettings::GetScreenWidth() / 2.0f,
		g_MainWindow.GetPosY() + (float)Gyro::UserSettings::GetScreenHeight() / 2.0f );
	}

	m_dx = 0;
	m_dy = 0;
}

void GMouse::UnRegisterButtonCallback( GMouseButtonCallback i_callback, void* i_owner )
{
	for( int i = 0; i < m_fnButtonCallbacks.size(); i++ )
	{
		if( m_fnButtonCallbacks[i] == i_callback )
		{
			if( m_fnCallbackOwners[i] == i_owner )
			{
				m_fnButtonCallbacks[i] = m_fnButtonCallbacks.back();
				m_fnButtonCallbacks.pop_back();

				m_fnCallbackOwners[i] = m_fnCallbackOwners.back();
				m_fnCallbackOwners.pop_back();
			}
		}
	}
}