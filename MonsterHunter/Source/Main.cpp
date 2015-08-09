#include <Windows.h>
#include <assert.h>

#include "Actor/ComponentManager.h"
#include "Utility/Debugging.h"
#include "Core/Loop.h"
#include "MonsterHunter.h"
#include "Utility/GSharedPtr.h"
#include "Utility/GSharedPtrBase.h"
#include "Misc/UserSettings.h"
#include "Profiling/GProfiling.h"
#include "Misc/DebugConsole.h"
//#include "MainWindow.h"
#include "../../Renderer/Source/MainWindow.h"

#define _CRTDBG_MAP_ALLOC_NEW

#include <stdlib.h>
#include <crtdbg.h>

bool bKeyPressed = false;

void OnKeyDown( unsigned int i_CharID )
{
#ifdef _DEBUG
	if( !DebugConsole::Active( ) )
	{
		bKeyPressed = false;
#endif
		MonsterHunter::HandleInput( i_CharID, true );
#ifdef _DEBUG
	}
#endif
}

#ifdef _DEBUG
struct CrtBreakAllocSetter
{
    CrtBreakAllocSetter()
	{
        _crtBreakAlloc=192;
    }
};
#endif

//CrtBreakAllocSetter g_crtBreakAllocSetter;

struct CrtDumperDoo
{
    ~CrtDumperDoo()
	{
        _CrtDumpMemoryLeaks();
    }
};

#ifdef _DEBUG
CrtDumperDoo g_dumper;
#endif

void OnKeyPress( unsigned int i_CharID ) //up
{
	bKeyPressed = false;


	// hack, but okay for now
#ifdef _DEBUG
	if( DebugConsole::Active( ) )
	{
		if( i_CharID == VK_TAB ) // tab
			DebugConsole::SetActive( false );
		else
			DebugConsole::HandleInput( i_CharID );
	}
	else
	{
		if( i_CharID == VK_TAB ) // tab
			DebugConsole::SetActive( true );
		else
#endif
			MonsterHunter::HandleInput( i_CharID, false );
#ifdef _DEBUG
	}
#endif

}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
	Gyro::UserSettings::Initialize();

	g_MainWindow.Create( "Some Game", Gyro::UserSettings::GetScreenWidth(), Gyro::UserSettings::GetScreenHeight(), hInstance, nCmdShow );
	g_MainWindow.RegisterKeyDownCallback( OnKeyDown );
	g_MainWindow.RegisterKeyPressCallback( OnKeyPress );

	bool bQuit = false;
	
	Gyro::Initialize( g_MainWindow.GetHandle() );
	MonsterHunter::Initialize( );
	g_Clock::Get( ).Initialize( );

	g_Profiler::Get().PrintAccumulators();

	do
	{
		g_MainWindow.Service(); // handle win messages.
		PROFILE_SCOPE_BEGIN( "Main" )

		MonsterHunter::BeginUpdate();
		Gyro::BeginUpdate();

		MonsterHunter::Update();
		Gyro::Update();

		Gyro::EndUpdate();
		MonsterHunter::EndUpdate();

		PROFILE_SCOPE_END( )
	}
	while( (bQuit == false) && (bKeyPressed == false) && g_MainWindow.Running );
	
	// has to come before because gyro shuts down the clock.
	g_Profiler::Get().PrintAccumulators();
	g_Profiler::Release();
	Gyro::Shutdown();

	#ifdef _DEBUG
	int created = GSharedPtrBase::Created;
	int deleted = GSharedPtrBase::Deleted;
	#endif
	
#ifdef _DEBUG
	//_CrtDumpMemoryLeaks();
#endif

}