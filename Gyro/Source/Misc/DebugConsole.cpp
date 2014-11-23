#include "DebugConsole.h"
#include "../../../Renderer/Source/RenderManager.h"

DebugConsole DebugConsole::TheConsole = DebugConsole();

// main menu dimensions
static const float LEFT_PERCENT_OFFSET = 0.01f;
static const float TOP_PERCENT_OFFSET = 0.1f;
static const float MENU_WIDTH_END = 0.2f;
static const float SLIDER_WIDTH = 0.1f;
static const D3DXCOLOR ENTRY_TEXT_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );
static const D3DXCOLOR HEADER_TEXT_COLOR( 1.0f, 1.0f, 0.0f, 1.0f );
static const D3DXCOLOR MENU_BOX_COLOR( 0.2f, 0.2f, 0.2f, 0.9f );
static const D3DXCOLOR MENU_CURSOR_COLOR( 1.0f, 1.0f, 0.0f, 0.5f );
static const D3DXCOLOR SLIDER_NOTCH_COLOR( 1.0f, 1.0f, 1.0f, 0.9f );

// checkbox
static const float CHECK_BOX_WIDTH = 0.01f;
static const D3DXCOLOR CHECKED_COLOR( 1.0f, 1.0f, 1.0f, 0.9f );
static const D3DXCOLOR NOT_CHECKED_COLOR( 0.0f, 0.0f, 0.0f, 0.9f );

// button
static const float BUTTON_WIDTH = 0.03f;
static const D3DXCOLOR BUTTON_COLOR( 0.1f, 0.1f, 0.1f, 0.9f );

// text within the menu dimensions
static const float TEXT_LEFT_PERCENT_OFFSET = 0.02f;

#ifdef _DEBUG
bool DebugConsole::Initialize( void )
{
	TheConsole.m_menuRoot = new DebugConsole::DebugMenu( );
	TheConsole.m_menuRoot->m_menuName = (char*) malloc( strlen( "Root Debug" ) + 1 );
	TheConsole.m_menuRoot->m_parent = NULL;
	strcpy( TheConsole.m_menuRoot->m_menuName, "Root Debug" );
	assert( TheConsole.m_menuRoot ); // could not allocate memory
	TheConsole.m_currentSelection = 0;
	TheConsole.m_currentMenu = TheConsole.m_menuRoot;
	TheConsole.m_active = false;

	return true;
}

void DebugConsole::Shutdown( void )
{
	delete TheConsole.m_menuRoot;
}

void DebugConsole::Draw( void )
{
	int size = TheConsole.m_currentMenu->m_childMenus.size() + 1;

	// just hard code font pixel size.
	float fontHeightPercent = 15.0f / (float) g_MainWindow.GetHeight();
	float entryPaddingPercent = 15.0f / (float) g_MainWindow.GetHeight();
	float height = (fontHeightPercent + entryPaddingPercent ) * size ;

	for( unsigned i = 0; i < TheConsole.m_currentMenu->m_childEntries.size(); i++)
	{
		height += TheConsole.m_currentMenu->m_childEntries[i]->m_screenHeightPercent;
		height += entryPaddingPercent;
	}

	g_RenderManager.m_hud.AddHUDBox( LEFT_PERCENT_OFFSET, TOP_PERCENT_OFFSET, MENU_WIDTH_END, TOP_PERCENT_OFFSET + height, MENU_BOX_COLOR );

	// now add the entries.  percent is the percentage of the screen the entries are taking up.
	float percent = entryPaddingPercent/2.0f;
	percent += TOP_PERCENT_OFFSET;

	long menuTop =  (long)(percent * g_MainWindow.GetHeight());
		g_RenderManager.m_hud.AddHUDText( TEXT_LEFT_PERCENT_OFFSET * g_MainWindow.GetWidth(), menuTop, MENU_WIDTH_END * g_MainWindow.GetHeight(),
			menuTop + 15, HEADER_TEXT_COLOR, TheConsole.m_currentMenu->m_menuName );

	percent += (entryPaddingPercent);

	for( int i = 0; i < TheConsole.m_currentMenu->m_childMenus.size(); i++ )
	{
		long top = percent * g_MainWindow.GetHeight();

		g_RenderManager.m_hud.AddHUDText( TEXT_LEFT_PERCENT_OFFSET * g_MainWindow.GetWidth(), top, MENU_WIDTH_END * g_MainWindow.GetHeight(),
			top + 15, ENTRY_TEXT_COLOR, TheConsole.m_currentMenu->m_childMenus[i]->m_menuName );

		if( TheConsole.m_currentSelection == i )
		{
			//draw the cursor.
			g_RenderManager.m_hud.AddHUDBox( LEFT_PERCENT_OFFSET, percent, MENU_WIDTH_END,
				percent + entryPaddingPercent, MENU_CURSOR_COLOR );
		}

		percent += entryPaddingPercent;
	}

	int selectedAdjusted = TheConsole.m_currentSelection - TheConsole.m_currentMenu->m_childMenus.size();

	for( int i = 0; i < TheConsole.m_currentMenu->m_childEntries.size(); i++ )
	{
		float entryHeight = TheConsole.m_currentMenu->m_childEntries[i]->m_screenHeightPercent;
		if( selectedAdjusted == i )
		{
			//draw the cursor.
			g_RenderManager.m_hud.AddHUDBox( LEFT_PERCENT_OFFSET, percent, MENU_WIDTH_END,
				 percent + entryHeight, MENU_CURSOR_COLOR );
		}

		long top = percent * g_MainWindow.GetHeight();

		g_RenderManager.m_hud.AddHUDText( TEXT_LEFT_PERCENT_OFFSET * g_MainWindow.GetWidth(), top, MENU_WIDTH_END * g_MainWindow.GetHeight(),
			top + 15, ENTRY_TEXT_COLOR, TheConsole.m_currentMenu->m_childEntries[i]->m_entryName );

		//draw the entry
		TheConsole.m_currentMenu->m_childEntries[i]->Draw( percent );
		percent += TheConsole.m_currentMenu->m_childEntries[i]->m_screenHeightPercent;
	}
}

void DebugConsole::HandleInput( int i_charCode )
{
	
	if( i_charCode == 0x26 ) // up arrow.  go up an entry.
	{
		if( TheConsole.m_currentSelection > 0 )
			TheConsole.m_currentSelection--;
		else
			TheConsole.m_currentSelection = TheConsole.m_currentMenu->m_childMenus.size() + TheConsole.m_currentMenu->m_childEntries.size() -1;

		return;
	}
	else if( i_charCode == 0x28 ) // down arrow.
	{
		if( TheConsole.m_currentSelection < ( TheConsole.m_currentMenu->m_childMenus.size() + TheConsole.m_currentMenu->m_childEntries.size() -1  ) )
		{
			TheConsole.m_currentSelection++;
		}
		else
			TheConsole.m_currentSelection = 0;
	}
	else if ( i_charCode == 0xA3 ) // right control.  go up an entry if possible.
	{
		if( TheConsole.m_currentMenu->m_parent != NULL )
		{
			TheConsole.m_currentSelection = 0;
			TheConsole.m_currentMenu = TheConsole.m_currentMenu->m_parent;
		}
		return;
	}

	//get the current selection and see if it's a menu entry.
	if( TheConsole.m_currentSelection < TheConsole.m_currentMenu->m_childMenus.size() )
	{
		if( i_charCode == 0x27 ) // right arrow. go down an entry.
		{
			TheConsole.m_currentMenu = TheConsole.m_currentMenu->m_childMenus[ TheConsole.m_currentSelection ];
			TheConsole.m_currentSelection = 0;
		}
	}
	else
	{
		int index = TheConsole.m_currentSelection - TheConsole.m_currentMenu->m_childMenus.size();
		if( index < TheConsole.m_currentMenu->m_childEntries.size() )
		{
			DebugEntry* entry = TheConsole.m_currentMenu->m_childEntries[index];
			entry->HandleInput( i_charCode );
		}
	}
}

void DebugConsole::DebugSlider::Draw( float i_screenPercent )
{
	float sliderEnd = i_screenPercent + m_screenHeightPercent;
	float sliderTop = (m_screenHeightPercent / 2.0f) * 0.9f;
	float sliderBottom = (m_screenHeightPercent / 2.0f) * 1.1f;

	float notchLeft = MENU_WIDTH_END + ((abs(*m_value) /(abs(m_maxValue) + abs(m_minValue))) * (MENU_WIDTH_END - SLIDER_WIDTH)) - 0.005f;
	float notchRight = MENU_WIDTH_END + ((abs(*m_value) /(abs(m_maxValue) + abs(m_minValue))) * (MENU_WIDTH_END - SLIDER_WIDTH)) + 0.005f;

	//draw the slider.  create the 
	g_RenderManager.m_hud.AddHUDBox( MENU_WIDTH_END, i_screenPercent, MENU_WIDTH_END + 0.1f, i_screenPercent + m_screenHeightPercent, MENU_BOX_COLOR );
	g_RenderManager.m_hud.AddHUDBox( notchLeft, i_screenPercent, notchRight, i_screenPercent + m_screenHeightPercent, SLIDER_NOTCH_COLOR );
}

void DebugConsole::DebugSlider::HandleInput( int i_charCode )
{
	if( i_charCode == 0x25 ) // left arrow. lower value.
	{
		if( *m_value - m_increment >= m_minValue )
			*m_value -= m_increment;
	}
	else if( i_charCode == 0x27 ) // right arrow. increase value.
	{
		if( *m_value + m_increment <= m_maxValue)
			*m_value += m_increment;
	}

	SliderFunc( "what", m_value );
}

void DebugConsole::DebugCheckBox::HandleInput( int i_charCode )
{
	if( i_charCode == 0x27 ) // right arrow.
	{
		*m_value = !*m_value;
		CheckBoxFunc( m_entryName, m_value );
	}
}

void DebugConsole::DebugCheckBox::Draw( float i_screenPercent )
{
	if( *m_value )
		g_RenderManager.m_hud.AddHUDBox( MENU_WIDTH_END, i_screenPercent, MENU_WIDTH_END + CHECK_BOX_WIDTH, i_screenPercent + m_screenHeightPercent, CHECKED_COLOR );
	else
		g_RenderManager.m_hud.AddHUDBox( MENU_WIDTH_END, i_screenPercent, MENU_WIDTH_END + CHECK_BOX_WIDTH, i_screenPercent + m_screenHeightPercent, NOT_CHECKED_COLOR );
}

void DebugConsole::DebugButton::HandleInput( int i_charCode )
{
	if( i_charCode == 0x27 ) // right arrow. call the function
	{
		FuncFunc( m_entryName );
	}
}

void DebugConsole::DebugButton::Draw( float i_screenPercent )
{
	g_RenderManager.m_hud.AddHUDBox( MENU_WIDTH_END, i_screenPercent, MENU_WIDTH_END + BUTTON_WIDTH, i_screenPercent + m_screenHeightPercent, BUTTON_COLOR );
}

DebugConsole::DebugMenu* DebugConsole::FindOrCreateMenu( const char* i_path )
{
	const char* i_start = i_path;
	const char* i_currentChar = i_start;
	DebugMenu* current = m_menuRoot;

	const char* charEnd = strchr( i_path, 32 ); //space
	int end = (int)charEnd - (int)i_path;

	//has to be a safer way of doing this...
	bool found = false;
	char buffer[56];

	for( int index = 0; index < end; )
	{
		i_start = i_currentChar;
		i_currentChar = strstr( i_start, "/" );

		//we've reached the end of the string, so current is the menu we're returning.
		if( i_currentChar == 0 )
		{
			i_currentChar = i_start + end - index ;
		}

		int difference = (int)i_currentChar - (int)i_start;
		index += difference;

		memcpy( buffer, i_start, difference );
		buffer[difference] = '\0';

		bool found = false;
		for( int i = 0; i < (int) current->m_childMenus.size(); i++ )
		{
			if( strcmp( buffer, current->m_childMenus[i]->m_menuName ) == 0 )
			{
				current = current->m_childMenus[i];
				found = true;
				break;
			}
		}

		if( !found )
		{
			DebugConsole::DebugMenu* menu = new DebugConsole::DebugMenu();
			current->m_childMenus.push_back( menu );

			char* name = (char*) malloc( strlen( buffer ) + 1 );
			menu->m_menuName = name;
			menu->m_parent = current;
			strcpy( menu->m_menuName, buffer );
			current = menu;
		}
		i_currentChar += 1;
		index += 1;
	}

	//assert( 0 ); // no menu root.
	return current;
}

DebugConsole::DebugMenu* DebugConsole::FindMenu( const char* i_path )
{
	const char* i_start = i_path;
	const char* i_currentChar = i_start;
	DebugMenu* current = m_menuRoot;

	const char* charEnd = strchr( i_path, 32 ); //space
	int end = (int)charEnd - (int)i_path;

	//has to be a safer way of doing this...
	bool found = false;
	char buffer[56];

	for( int index = 0; index < end; )
	{
		i_start = i_currentChar;
		i_currentChar = strstr( i_start, "/" );

		//we've reached the end of the string, so current is the menu we're returning.
		if( i_currentChar == 0 )
		{
			i_currentChar = i_start + end - index ;
		}

		int difference = (int)i_currentChar - (int)i_start;
		index += difference;

		memcpy( buffer, i_start, difference );
		buffer[difference] = '\0';

		bool found = false;
		for( int i = 0; i < (int) current->m_childMenus.size(); i++ )
		{
			if( strcmp( buffer, current->m_childMenus[i]->m_menuName ) == 0 )
			{
				current = current->m_childMenus[i];
				found = true;
				break;
			}
		}
	}

	if( current == m_menuRoot)
		return NULL;

	return current;
}

void DebugConsole::AddSlider( const char *name, float *value, float minValue, float maxValue, float i_increment, void(*function)(const char*, float*) )
{
	//Find the correct menu, or create it if it doesn't exist.
	DebugMenu* menu = TheConsole.FindOrCreateMenu( name );
	DebugSlider* entry = new DebugSlider();
	const char* newName = strchr( name, 32 ) + 1; //white space
	char* newNewName = (char*) malloc( strlen( newName ) + 1 );
	strcpy( newNewName, newName);
	entry->m_entryName = newNewName;
	entry->m_maxValue = maxValue;
	entry->m_minValue = minValue;
	entry->m_value = value;
	entry->m_increment = i_increment;
	entry->SliderFunc = function;

	menu->m_childEntries.push_back( entry );
}

void DebugConsole::SetActive( bool i_active )
{
	if( TheConsole.m_active != i_active )
	{
		TheConsole.m_currentSelection = 0;
		TheConsole.m_currentMenu = TheConsole.m_menuRoot;
	}
	TheConsole.m_active = i_active;
}

void DebugConsole::AddCheckBox( const char *name, bool *boolValue, void(*function)(const char*, bool*) )
{
	DebugMenu* menu = TheConsole.FindOrCreateMenu( name );
	assert( menu ); // menu could not be found or created.  ran out of memory?


	DebugCheckBox* checkBox = new DebugCheckBox();

	const char* newName = strchr( name, 32 ) + 1; //white space
	char* newNewName = (char*) malloc( strlen( newName ) +1 );
	strcpy( newNewName, newName);
	checkBox->m_entryName = newNewName;
	checkBox->m_value = boolValue;
	checkBox->CheckBoxFunc = function;
	
	menu->m_childEntries.push_back( checkBox );
}


void DebugConsole::AddTextField( const char *name, char *stringValue, void(*function)(const char*, char *) )
{

}

void DebugConsole::Remove( const char* i_name )
{
	DebugMenu* menu =  TheConsole.FindMenu( i_name );
	if( menu )
	{
		const char* newName = strchr( i_name, 32 ) + 1; //white space
		if( newName )
		{
			for( int i = 0; i < menu->m_childEntries.size(); i++ )
			{
				if( strcmp( menu->m_childEntries[i]->m_entryName, newName ) == 0 )
				{
					menu->m_childEntries[i] = menu->m_childEntries.back();
					menu->m_childEntries.pop_back();
					//if( menu->m_childMenus.size() <= 0 && menu->m_ch)
				}
			}
		}
	}
}

	
void DebugConsole::AddButton( const char *name, void(*function)(const char*) )
{
	DebugButton* entry = new DebugButton();
	const char* newName = strchr( name, 32 ) + 1; //white space
	char* newNewName = (char*) malloc( strlen( newName ) +1 );
	strcpy( newNewName, newName);
	entry->m_entryName = newNewName;
	entry->FuncFunc = function;
	DebugMenu* menu = TheConsole.FindOrCreateMenu( name );
	menu->m_childEntries.push_back( entry );
}


#else
bool DebugConsole::Initialize( void ){}
void DebugConsole::Draw( void ){}
void DebugConsole::HandleInput( int i_charCode ){}
void DebugConsole::DebugSlider::Draw( float i_screenPercent ){}
void DebugConsole::DebugSlider::HandleInput( int i_charCode ){}
void DebugConsole::DebugCheckBox::HandleInput( int i_charCode ){}
void DebugConsole::DebugCheckBox::Draw( float i_screenPercent ){}
void DebugConsole::DebugButton::HandleInput( int i_charCode ){}
void DebugConsole::DebugButton::Draw( float i_screenPercent ){}
DebugConsole::DebugMenu* DebugConsole::FindOrCreateMenu( const char* i_path ){}
void DebugConsole::AddSlider( const char *name, float *value, float minValue, float maxValue, float i_increment, void(*function)(const char*, float*) ){}
void DebugConsole::SetActive( bool i_active ){}
void DebugConsole::AddCheckBox( const char *name, bool *boolValue, void(*function)(const char*, bool*) ){}
void DebugConsole::AddTextField( const char *name, char *stringValue, void(*function)(const char*, char *) ){}
void DebugConsole::AddButton( const char *name, void(*function)(const char*) ){}
void DebugConsole::Remove( const char* i_name ){}
void DebugConsole::Shutdown( void ) {} 

#endif