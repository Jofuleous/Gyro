#ifndef __DEBUGCONSOLE_H_
#define __DEBUGCONSOLE_H_

#include "Utility/Singleton.h"
#include "../../Renderer/Source/MainWindow.h"
#include <vector>

class DebugConsole
{
public:

	class DebugEntry
	{
	public:
		DebugEntry( float m_screenHeightPercent ) : m_screenHeightPercent( m_screenHeightPercent ) { }
		~DebugEntry()
		{
			free( m_entryName );
		}

		float			m_screenHeightPercent;
		char*			m_entryName;
		virtual void	HandleInput( int i_charCode ) = 0;
		virtual void	Draw( float i_screenPercent ) = 0;
	};

	class DebugSlider : public DebugEntry
	{
	public:
		DebugSlider( ) : DebugEntry( 15.0f / (float) g_MainWindow.GetHeight() )
		{
		}

		float*									m_value;
		float									m_maxValue;
		float									m_minValue;
		float									m_increment;

		void(*SliderFunc)(const char*, float*);
		virtual void							HandleInput( int i_charCode );
		virtual void							Draw( float i_screenPercent  );
	};

	class DebugCheckBox : public DebugEntry
	{
	public:
		DebugCheckBox( ) : DebugEntry( 15.0f / (float) g_MainWindow.GetHeight() )
		{
		}

		bool*									m_value;
		void(*CheckBoxFunc)(const char*, bool*);
		virtual void							HandleInput( int i_charCode );
		virtual void							Draw( float i_screenPercent  );
	};

	class DebugButton : public DebugEntry
	{
	public:
		DebugButton( ) : DebugEntry( 15.0f / (float) g_MainWindow.GetHeight() )
		{
		}

		void(*FuncFunc)(const char*);
		virtual void							HandleInput( int i_charCode );
		virtual void							Draw( float i_screenPercent  );
	};

	/* Not sure what to do with this yet...
	template< typename T >
	class DebugViewValue
	{
		const char*								m_entryName;
		T*										m_value;
		void(*CheckBoxFunc)(const char*, bool);
		virtual void							HandleInput( int i_charCode );
		virtual void							Draw( );
	};
	*/

	class DebugMenu
	{
	public:
										~DebugMenu( )
										{
											for( unsigned i = 0; i < m_childMenus.size(); i++ )
												delete m_childMenus[i];
											for( unsigned i = 0; i < m_childEntries.size(); i++ )
												delete m_childEntries[i];

											//if( m_parent )
												free( m_menuName );
										}

		void							Draw(  float i_screenPercent );
		char*							m_menuName;
		DebugMenu*						m_parent;
		std::vector< DebugMenu* >		m_childMenus;
		std::vector< DebugEntry* >		m_childEntries;
	};

	static bool							Initialize( void );
	static void							HandleInput( int i_KeyChar );
	static void							AddSlider( const char *i_name, float *i_value, float i_minValue, float i_maxValue, float i_increment, void(*function)(const char*, float*)=0 );
	static void							AddCheckBox( const char *i_name, bool *i_boolValue, void(*function)(const char*, bool*)=0 );
	static void							AddTextField( const char *i_name, char *i_stringValue, void(*function)(const char*, char *)=0 );
	static void							AddButton( const char *i_name, void(*function)(const char*) );
	static void							Remove( const char *i_name );
	static void							Draw ( void );
	static bool							Active( void ) { return TheConsole.m_active; } // not compiling this out.
	static void							SetActive( bool i_active );
	static void							Shutdown( void );

	static DebugConsole					TheConsole;
private:
	DebugMenu*						FindOrCreateMenu( const char* i_path );
	DebugMenu*						FindMenu( const char* i_path );

	DebugMenu*						m_menuRoot;
	DebugMenu*						m_currentMenu;
	int								m_currentSelection;
	bool							m_active;

};

#endif