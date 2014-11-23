#include "MainWindow.h"
#include "../Resources/resource.h"
#include <assert.h>
#include <stdio.h>

namespace
{
	// This text will be displayed in the title bar of the main window
	const char* s_windowCaption = "X";
}

namespace Renderer
{

HWND MainWindow::GetHandle() const
{
	return m_window;
}

MainWindow::~MainWindow()
{
	Destroy();
	UnregisterClass();
}


bool MainWindow::RegisterClass()
{

	WNDCLASSEX wndClassEx = { 0 };
	wndClassEx.cbSize = sizeof( WNDCLASSEX );
	wndClassEx.hInstance = m_applicationInstance;

	// The class's style
	// (We don't have to worry about any of these)
	wndClassEx.style = 0;
	// The function that will process all of the messages
	// that Windows will send to windows of this class
	wndClassEx.lpfnWndProc = OnMessageReceived;
	// Extra bytes can be set aside in the class for user data
	// (This is rarely used, but can be useful)
	wndClassEx.cbClsExtra = 0;
	// Extra bytes can be set aside for each window of the class,
	// but this is usually specified for each window individually
	wndClassEx.cbWndExtra = 0;
	// The large and small icons windows of this class should use
	// (These can be found in the Resources folder; feel free to change them)
	wndClassEx.hIcon = LoadIcon( m_applicationInstance, MAKEINTRESOURCE( IDI_RENDERER ) );
	wndClassEx.hIconSm = LoadIcon( m_applicationInstance, MAKEINTRESOURCE( IDI_SMALL ) );
	// The cursor that should display when the mouse pointer is over windows of this class
	wndClassEx.hCursor = LoadCursor( NULL, IDC_ARROW );
	// The brush windows of this class will use as a background
	// (Setting this is a bit confusing, so don't be alarmed if the next line looks scary)
	wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>( IntToPtr( COLOR_BACKGROUND + 1 ) );
	// A menu can be specified that all windows of this class would use by default,
	// but usually this is set for each window individually
	wndClassEx.lpszMenuName = NULL;
	// The class name (see comments where this is initialized)
	const char* lol = m_sClassName;
	wndClassEx.lpszClassName = lol;

	// Now we can provide this information to Windows.
	// If all goes well, the class will be successfully registered
	// and we can specify it by name when creating our windows.
	m_class = RegisterClassEx( &wndClassEx );
	if ( m_class != NULL )
	{
		//LogMessage( "Registered the main window class" );
		return true;
	}
	else
	{
		//LogMessage( std::string( "Windows failed to register the main window's class: " ) + GetLastWindowsError() );
		return false;
	}
}

bool MainWindow::UnregisterClass()
{
	bool wereThereAnyErrors = false;

	if ( m_class )
	{
		if ( ::UnregisterClass( m_sClassName, m_applicationInstance ) == TRUE )
		{
			//LogMessage( "Unregistered the main window class" );
		}
		else
		{
			//LogMessage( std::string( "Windows failed to unregister the main window's class: " ) + GetLastWindowsError() );
			assert( 0 );
		}
		m_class = NULL;
	}

	return !wereThereAnyErrors;
}

bool MainWindow::Create( char* i_windowName, int i_width, int i_height, const HINSTANCE i_thisInstanceOfTheApplication, const int i_initialWindowDisplayState )
{
	m_applicationInstance = i_thisInstanceOfTheApplication;
	m_sClassName = i_windowName;
	m_width = i_width;
	m_height = i_height;

	if ( !RegisterClass() )
	{
		return false;
	}

	for( unsigned int i = 0; i < NUM_KEYS; i++ )
		keyState[i] = UNKNOWN;

		// The window's style
	DWORD windowStyle =
		// "Overlapped" is basically the same as "top-level"
		WS_OVERLAPPED
		// The caption is the title bar when in windowed-mode
		| WS_CAPTION
		// The window should never change dimensions, so only a minimize box is allowed
		| WS_MINIMIZEBOX
		// The system menu appears when you right-click the title bar
		| WS_SYSMENU;
	// The window's extended style
	DWORD windowStyle_extended =
		// The following is a macro to make the extended style the default top-level look
		WS_EX_OVERLAPPEDWINDOW;
	// The width and height of the window
	// (The renderer is concerned about the width and height of the actual "client area",
	// which is the part of the window that doesn't include the borders and title bar.
	// the initial window, then, will created with default values that Windows chooses
	// and then resized after creation)
	int width = CW_USEDEFAULT;
	int height = CW_USEDEFAULT;
	// The initial position of the window
	// (We don't care, and will let Windows decide)
	int position_x = CW_USEDEFAULT;
	int position_y = CW_USEDEFAULT;
	// Handle to the parent of this window
	// (Since this is our main window, it can't have a parent)
	HWND hParent = NULL;
	// Handle to the menu for this window
	HMENU hMenu = NULL;
	// Handle to the instance of the application this window should be associated with
	HINSTANCE hApplication = m_applicationInstance;
	// Any arbitrary pointer can be associated with this window;
	// usually an application's representation of the window will be used, like this:
	void* userData = this;
	// (Since the main window is a global singleton, though, this program won't actually use this)

	// Ask Windows to create the specified window.
	// CreateWindowEx() will return a handle to the window,
	// which is what we'll use to communicate with Windows about this window
	m_window = CreateWindowEx( windowStyle_extended, i_windowName, s_windowCaption, windowStyle,
		position_x, position_y, width, height,
		hParent, hMenu, hApplication, userData );
	if ( m_window )
	{
		// Change the window's size based on the requested user settings
		{
			// Calculate how much of the window is coming from the "non-client area"
			// (the borders and title bar)
			RECT windowCoordinates;
			struct
			{
				long width;
				long height;
			} nonClientAreaSize;
			{
				// Get the coordinates of the entire window
				if ( GetWindowRect( m_window, &windowCoordinates ) == FALSE )
				{
					assert( 0 );
					//LogMessage( std::string( "Windows failed to get the coordinates of the main window: " ) + GetLastWindowsError() );
					return false;
				}
				// Get the dimensions of the client area
				RECT clientDimensions;
				if ( GetClientRect( m_window, &clientDimensions ) == FALSE )
				{
					assert( 0 );
					//LogMessage( std::string( "Windows failed to get the dimensions of the main window's client area: " )
					//	+ GetLastWindowsError() );
					return false;
				}
				// Get the difference between them
				nonClientAreaSize.width = ( windowCoordinates.right - windowCoordinates.left ) - clientDimensions.right;
				nonClientAreaSize.height = ( windowCoordinates.bottom - windowCoordinates.top ) - clientDimensions.bottom;
			}
			// Resize the window
			{
				BOOL shouldTheResizedWindowBeRedrawn = TRUE;
				if ( MoveWindow( m_window, windowCoordinates.left, windowCoordinates.top,
					i_width + nonClientAreaSize.width, i_height + nonClientAreaSize.height,
					shouldTheResizedWindowBeRedrawn ) == FALSE )
				{
					assert( 0 );
					//LogMessage( std::string( "Windows failed to resize the main window: " ) + GetLastWindowsError() );
					return false;
				}
			}
		}

		// Display the window in the initial state that Windows requested
		ShowWindow( m_window, i_initialWindowDisplayState );

		//LogMessage( "Created the main window" );
		Running = true;
		return true;
	}
	else
	{
		assert( 0 );
		//LogMessage( std::string( "Windows failed to create the main window: " ) + GetLastWindowsError() );
		return false;
	}

	Running = true;

	return true;
}

bool MainWindow::Destroy( )
{
	bool wereThereAnyErrors = false;

	if ( m_window )
	{
		if ( DestroyWindow( m_window ) == TRUE )
		{
			//LogMessage( "Destroyed the main window" );
		}
		else
		{
			//LogMessage( std::string( "Windows failed to destroy the main window: " ) + GetLastWindowsError() );
			wereThereAnyErrors = true;
		}
		m_window = NULL;
	}

	return !wereThereAnyErrors;
}

void MainWindow::Service( )
{
	MSG message = { 0 };

	bool hasWindowsSentAMessage;
	{
		HWND getMessagesFromAnyWindowBelongingToTheCurrentThread = NULL;
		unsigned int getAllMessageTypes = 0;
		unsigned int ifAMessageExistsRemoveItFromTheQueue = PM_REMOVE;
		hasWindowsSentAMessage = PeekMessage( &message, getMessagesFromAnyWindowBelongingToTheCurrentThread,
			getAllMessageTypes, getAllMessageTypes, ifAMessageExistsRemoveItFromTheQueue ) == TRUE;
	}
	if ( !hasWindowsSentAMessage )
	{
		// Usually there will be no messages in the queue, and the renderer can run
		// g_time.OnNewFrame();
	}
	else
	{
		// If Windows _has_ sent a message, this iteration of the loop will handle it

		// First, the message must be "translated"
		// (Key presses are translated into character messages)
		TranslateMessage( &message );

		// Then, the message is sent on to the appropriate processing function.
		// This function is specified in the lpfnWndProc field of the WNDCLASSEX struct
		// used to register a class with Windows.
		// In the case of the main window it will be cMainWindow::OnMessageReceived()
		DispatchMessage( &message );
	}
}


LRESULT CALLBACK MainWindow::OnMessageReceived( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam )
{

	if( (i_message == WM_KEYDOWN)  &&  (g_MainWindow.keyState[i_wParam] != DOWN) )
	{
		g_MainWindow.keyState[i_wParam] = DOWN;

		if( g_MainWindow.fnKeyDownCallback != NULL )
			g_MainWindow.fnKeyDownCallback( i_wParam );

		char buffer[256];
		sprintf_s( buffer, sizeof( buffer ), "C: %d KD: %d\n", i_wParam, true );

		OutputDebugStringA( buffer );

	}
	else if( (i_message == WM_KEYUP)  &&  (g_MainWindow.keyState[ i_wParam ] != UP) )
	{
		g_MainWindow.keyState[ i_wParam ] = UP;
	
		if( g_MainWindow.fnKeyPressCallback != NULL )
			g_MainWindow.fnKeyPressCallback( i_wParam );
		
		char buffer[256];
		sprintf_s( buffer, sizeof( buffer ), "C: %d KD: %d\n", i_wParam, false );

		OutputDebugStringA( buffer );
	}
	// mouse button press/release
	else if( i_message >= WM_LBUTTONDOWN && i_message <= WM_MBUTTONDBLCLK )
	{
		if( g_MainWindow.fnMouseButtonCallback )
			g_MainWindow.fnMouseButtonCallback( i_message, g_MainWindow.fnMouseOwner );
	}
	// mouse move
	else if( i_message == WM_MOUSEMOVE  )
	{
		if( g_MainWindow.fnMousePosCallback )
		{
			float x, y;
			x = LOWORD( i_lParam );
			y = HIWORD( i_lParam );
			g_MainWindow.fnMousePosCallback( x, y, g_MainWindow.fnMouseOwner );
		}
	}
	else if( i_message == WM_WINDOWPOSCHANGED )
	{
		LPWINDOWPOS pos;
		pos = (LPWINDOWPOS) i_lParam;
		g_MainWindow.m_posX = pos->x;
		g_MainWindow.m_posY = pos->y;
		// assume that the size will not change after boot...
	}
	else if( i_message == WM_ACTIVATE )
	{
		g_MainWindow.m_windowActive = (bool) LOWORD( i_wParam );

		// call the callbacks eventually?

	}
	//WM_MOUSEWHEEL = 0x20A
	//WM_MOUSEHWHEEL = 0x20E


	// DispatchMessage() will send messages to the main window here.
	// There are many messages that get sent to a window,
	// but the renderer application can ignore most of them
	// and let Windows handle them in the default way.

	// Process any messages that the renderer cares about
	// (any messages that are processed here should return a value
	// rather than letting the default processing function try to handle them a second time)

	
	switch( i_message )
	{
	case WM_CHAR:
		{
			// A key has been pressed down, and this is the translated character
			//------------------------------------------------------------------

			// This isn't usually the best way to handle keyboard input,
			// but it is a convenient way to handle an exit key
			if ( i_wParam == VK_ESCAPE )
			{
				//LogMessage( "The escape key was pressed; the application will now exit" );
				g_MainWindow.Running = false;
				int exitCode = 0;
				PostQuitMessage( exitCode );
			}

			break;
		}
	case WM_NCDESTROY:
		{
			// The window's nonclient area is being destroyed
			//-----------------------------------------------

			//LogMessage( "Main window is being destroyed" );

			// This is the last message a window will receive
			// (Any child windows have already been destroyed).
			// After this message has been processed the window's handle will be invalid
			g_MainWindow.m_window = NULL;

			// When the main window is destroyed
			// a WM_QUIT message should be sent
			// (if this isn't done the application would continue to run with no window).
			// This is where the exitCode in cMainWindow::WaitForShutdown() comes from:
			//g_MainWindow.Running = false;
			g_MainWindow.Running = false;
			int exitCode = 0;
			PostQuitMessage( exitCode );

			break;
		}
	}

	// Pass any messages that weren't handled on to Windows
	return DefWindowProc( i_window, i_message, i_wParam, i_lParam );
}


}