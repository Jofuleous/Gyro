#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

// Initialize Windows
#include <Windows.h>
#include <vector>
#include <assert.h>

typedef void (*GkeyCallback)( unsigned int i_CharID );
typedef void (*GMouseButtonCallback)( unsigned int i_message, void* i_data );
typedef void (*GMousePosCallback)( float x, float y, void* i_data );
typedef void (*GWindowActiveCallback)( bool active, void* i_data ); // may need to expand this later?

enum GKeyState
{
	DOWN,
	UP,
	UNKNOWN
} ;

namespace Renderer
{
	class MainWindow
	{
	public:

		static const unsigned int	NUM_KEYS = 255;
		GKeyState					keyState[ NUM_KEYS ];

		static inline MainWindow& GetSingleton()
		{
			static MainWindow singleton;
			return singleton;
		}

		bool	Create( char* i_windowName, int i_width, int i_height, const HINSTANCE i_thisInstanceOfTheApplication, const int i_initialWindowDisplayState );
		void	RegisterKeyDownCallback( GkeyCallback i_callback ) { fnKeyDownCallback = i_callback; }
		void	RegisterKeyPressCallback( GkeyCallback i_callback ) { fnKeyPressCallback = i_callback; }
		void	RegisterMouseCallbacks( GMouseButtonCallback i_callback, GMousePosCallback i_MouseBack, void* i_data )
		{
				fnMouseButtonCallback = i_callback;
				fnMousePosCallback = i_MouseBack;
				fnMouseOwner = i_data;
		}
		void	UnRegisterMouseCallbacks( )
		{
				fnMouseButtonCallback = NULL;
				fnMousePosCallback = NULL;
				fnMouseOwner = NULL;
		}

		void RegisterWindowActiveCallback( GWindowActiveCallback i_callback, void* i_data )
		{
#ifdef _DEBUG
			for( unsigned i = 0; i < m_fnWindowActiveCallbacks.size(); i++ )
				assert( m_fnWindowActiveCallbacks[i] != i_callback );
#endif
			m_fnWindowActiveCallbacks.push_back( i_callback );
			m_fnWindowActiveOwners.push_back( i_data );
		}

		void UnRegisterWindowActiveCallback( GWindowActiveCallback i_callback )
		{
			for( unsigned i = 0; i < m_fnWindowActiveCallbacks.size(); i++ )
			{
				if( m_fnWindowActiveCallbacks[i] == i_callback )
				{
					m_fnWindowActiveCallbacks[i] = m_fnWindowActiveCallbacks.back();
					m_fnWindowActiveCallbacks.pop_back();

					m_fnWindowActiveOwners[i] = m_fnWindowActiveOwners.back();
					m_fnWindowActiveOwners.pop_back();
				}
			}
		}

		void			Service( void );

		HWND			GetHandle() const;
		inline int		GetWidth() const { return m_width; }
		inline int		GetHeight() const { return m_height; }
 		inline int		GetPosX( ) const { return m_posX; }
		inline int		GetPosY( ) const { return m_posY; }
		inline bool		Active( ) const { return m_windowActive; }

		~MainWindow();

		bool Running;


		GkeyCallback			fnKeyDownCallback;
		GkeyCallback			fnKeyPressCallback;
		GMousePosCallback		fnMousePosCallback;
		GMouseButtonCallback	fnMouseButtonCallback;
		void*					fnMouseOwner; // keeping it hacky for now...

		// here's how things should be done!
		std::vector<void*>					m_fnWindowActiveOwners;
		std::vector<GWindowActiveCallback>	m_fnWindowActiveCallbacks;

	private:

		// Callbacks
		//----------

		static LRESULT CALLBACK OnMessageReceived( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam );


		MainWindow() : m_applicationInstance( NULL ), m_class( NULL ), m_window( NULL ),
						fnKeyDownCallback( NULL ), fnKeyPressCallback( NULL ), fnMouseButtonCallback( NULL ),
						fnMousePosCallback( NULL )
		{

		}

		bool Destroy();
		bool RegisterClass();
		bool UnregisterClass();

		HINSTANCE m_applicationInstance;
		ATOM m_class;
		HWND m_window;

		char*	m_sClassName;
		int		m_posX;
		int		m_posY;
		int		m_width;
		int		m_height;
		bool	m_windowActive;
	};
}

#define g_MainWindow Renderer::MainWindow::GetSingleton()

#endif

