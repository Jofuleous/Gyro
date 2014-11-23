#include "Loop.h"
#include "World.h"
#include "Utility/Clock.h"
#include "Physics/GPhysics.h"
#include "Collision/CollisionManager.h"
#include "Renderer/GRenderer.h"
#include "Utility/GSharedPtrBase.h"
#include "Actor/ComponentManager.h"
#include "Math/GVector3.h"
#include "Core/GMessageManager.h"
#include "Profiling/GProfiling.h"
#include "Misc/DebugConsole.h"
#include "Navigation/GWayPointManager.h"
#include "Audio/GAudioManager.h"


bool Gyro::Initialize( HWND i_windowHandle )
{
	g_ComponentManager::Get().Initialize();

#ifdef _DEBUG
	DebugConsole::Initialize();
#endif 

	if( !g_World::Get().Initialize() )
		return false;

	if( !g_Physics::Get().Initialize() )
		return false;

	if( !g_Renderer::Get().Initialize( i_windowHandle ) )
		return false;

	if( !g_WayPointManager.Initialize() )
		return false;

	if( !g_AudioManager.Initialize() )
		return false;
	
	return true;
}

void Gyro::BeginUpdate( )
{
	g_Clock::Get().Update();

}

void Gyro::Update( )
{
	g_AudioManager.Update();

	g_WayPointManager.UpdateSolvers();

	//PROFILE_SCOPE_BEGIN( "World Update" )
	g_World::Get().Update( );
	//PROFILE_SCOPE_END( )

	//PROFILE_SCOPE_BEGIN( "Physics" )
	g_Physics::Get().Update();
	//PROFILE_SCOPE_END( )
}

void Gyro::EndUpdate( )
{

	g_Physics::Get().EndUpdate();
	g_World::Get().EndUpdate();

// submit the debug draw data..
#ifdef _DEBUG
	if( DebugConsole::Active() )
		DebugConsole::Draw( );
#endif 

	g_Renderer::Get().BeginUpdate();
	g_Renderer::Get().Update();
	g_Renderer::Get().EndUpdate();

	g_Physics::Get().RemoveDeadObjects();
	g_Renderer::Get().RemoveDeadObjects();
	g_World::Get().RemoveDeadActors();

}

void Gyro::Shutdown( )
{
	//PROFILE_PRINT_RESULTS()

	// for now, renderer has to be destroyed first because it keeps a smart pointer to actor...fix this later.
	g_RenderManager.Shutdown();
	g_Renderer::Get().Shutdown();
	g_Physics::Get().Shutdown();
	g_MessageManager::Get().Shutdown();
	g_ComponentManager::Get().Shutdown();
	g_World::Get().Shutdown();

#ifdef _DEBUG 
	// clean up debug console...haha. not now :)
#endif

	g_Physics::Release();
	g_Renderer::Release();
	g_World::Release();
	g_Clock::Release();
	g_Collision::Release();
	g_MessageManager::Release();
	g_ComponentManager::Release();
	DebugConsole::Shutdown();
	//g_Profiler::Release();

	delete GSharedPtrBase::s_refPool;
	delete GVector3::m_allocator;
}