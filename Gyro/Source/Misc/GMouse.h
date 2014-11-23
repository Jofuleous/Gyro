#ifndef __GMOUSE_H_
#define __GMOUSE_H_

#include "../../../Renderer/Source/MainWindow.h"
#include <vector>

// this should probably go in the renderer since it's so tied to the screen.
// set this up for polling and callbacks

//typedef void (*ButtonEventCallback)( GMouse::ButtonEvent i_event, void* i_owner );

class GMouse
{
public:
	std::vector<GMouseButtonCallback> m_fnButtonCallbacks;
	std::vector<void*> m_fnCallbackOwners;

	enum ButtonEvent
	{
		LEFT_UP,
		LEFT_DOWN,
		MIDDLE_UP,
		MIDDLE_DOWN,
		RIGHT_UP,
		RIGHT_DOWN,
	};

					GMouse() {}
					~GMouse();

	void			Initialize( );
	void			Shutdown( );
	void			Update( ); // Update delta X and Y if centering the cursor.

	inline int		GetX( ) { return g_MainWindow.GetPosX() - m_x; } // Returns the mouse position X in window space.
	inline int		GetY( ) { return g_MainWindow.GetPosY() - m_y; } // Return the mouse position Y in window space.
	inline int		GetDeltaX( ) { return m_dx; } // How much the cursor moved since the last update?
	inline int		GetDeltaY( ) { return m_dy; } // How much the cursor moved since the last update?
	inline int		GetTotalDeltaX() { return m_dxTotal; }
	inline int		GetTotalDeltaY() { return m_dyTotal; }
	void			HideCursor( bool i_value ) { ShowCursor( !i_value ); m_cursorHidden = i_value; }
	void			CenterCursor( bool i_value ); // Centers the cursor in the middle of the screen.
	bool			CenterCursor() { return m_centerCursor; }
	bool			CursorHidden( ) { return m_cursorHidden; }
	bool			LMBDown( ) { return !m_leftUp; }
	bool			RMBDown( ) { return !m_rightUp; }


	void			RegisterButtonCallback( GMouseButtonCallback i_callback, void* i_owner )
	{
		m_fnButtonCallbacks.push_back( i_callback );
		m_fnCallbackOwners.push_back( i_owner );
	}

	void			UnRegisterButtonCallback( GMouseButtonCallback i_callback, void* i_owner );
	void			HandleButton( u32 i_message );
	inline void		HandlePosition( float i_x, float i_y );

private:

	bool		m_centerCursor;
	bool		m_cursorHidden;
	bool		m_rightUp;
	bool		m_middleUp;
	bool		m_leftUp;
	int			m_dx;
	int			m_dy;
	int			m_dxTotal;
	int			m_dyTotal;
	int			m_windowX;
	int			m_windowY;
	int			m_x;
	int			m_y;
};

#endif