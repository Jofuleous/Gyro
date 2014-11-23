#include "MonsterHunter.h"
#include "Core/World.h"
#include "Actor/GActor.h"
#include "Components/PlayerComponent.h"
#include "GameInit.h"
#include "Renderer/GRenderer.h"
#include "Collision/CollisionManager.h"
#include "Misc/DebugConsole.h"
#include "LuaPlus/LuaPlus.h"
#include "CaptureFlagManager.h"
#include "Audio/GAudioManager.h"
#include "Components/QuatTestComp.h"

namespace MonsterHunter
{

	//Since we don't have a level file or anything yet.
	bool Initialize( )
	{

		GameInit::RegisterComponents();
		GameInit::RegisterTriggerFuncs();

		/// CTF
		g_World::Get().LoadEditorObjects( "data/Levels/BrapbrapQuat.brox" );
		g_Renderer::Get().LoadBinaryScene("data/Levels/BrapbrapQuat.scx" );
		g_Collision::Get().LoadBinarySceneCollision( "data/Levels/BrapbrapQuat.scx" );
		g_CTFManager::Get().Initialize();
		g_AudioManager.PlayMusic( "data/Audio/Music/allidoiswin.mp3" ); 
		
		return true;
	}

	void HandleInput( unsigned int i_CharID, bool i_keyDown )
	{
		/// CTF
		
		Gyro::World& world = g_World::Get();
		GActorHandle player = world.GetActorByName( "PlayerOne" );
		
		if( player )
		{
			PlayerComponent* pC = GetComponent<PlayerComponent>( player );
			pC->HandleInput( player, i_CharID, i_keyDown );
		}

		GActorHandle quatHandle = world.GetActorByName("QuaternTest");
		if (quatHandle)
		{
			QuatTestComp* comp = GetComponent<QuatTestComp>(quatHandle);
			comp->HandleInput(player, i_CharID, i_keyDown);
		}
	}

	void BeginUpdate( )
	{
	}

	void Update( )
	{
		/*
		Gyro::World& world = g_World::Get();

		std::vector<GActorHandle> monsters;
		world.GetActorsByName( "BasicMonster", monsters);

		GActorHandle player = world.GetActorByName( "Player" );
			
		PlayerComponent* pComp = NULL;
		*/

		GActorHandle playerHandle = g_World::Get().GetActorByName( "PlayerOne" );
		GActor* player = GActor::FromHandle( playerHandle );
		if( player )
			g_AudioManager.UpdateListenerPosition( player->m_position, player->m_rotation.Dir(), player->m_rotation.Up() );
		/// CTF
		g_CTFManager::Get().Update();

	}

	void EndUpdate( )
	{
	}

}