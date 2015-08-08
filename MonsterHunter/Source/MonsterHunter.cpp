#include "MonsterHunter.h"
#include "Core/World.h"
#include "Actor/GActor.h"
#include "Components/PlayerComponent.h"
#include "GameInit.h"
#include "Renderer/GRenderer.h"
#include "Collision/CollisionManager.h"
// #include "CaptureFlagManager.h"  CTF INCLUDE
#include "Audio/GAudioManager.h"
#include "Components/QuatTestComp.h"

namespace MonsterHunter
{

	//Since we don't have a level file or anything yet.
	bool Initialize()
	{
		GameInit::RegisterComponents();
		GameInit::RegisterTriggerFuncs();

		/// CTF
		g_World::Get().LoadEditorObjects( "data/Levels/AnimFinalProj.brox" );
		g_Renderer::Get().LoadBinaryScene("data/Levels/AnimFinalProj.scx" );
		g_Collision::Get().LoadBinarySceneCollision( "data/Levels/AnimFinalProj.scx" );
		//g_CTFManager::Get().Initialize();
		g_AudioManager.PlayMusic( "data/Audio/Music/allidoiswin.mp3" ); 
		
		return true;
	}

	void HandleInput( unsigned int i_CharID, bool i_keyDown )
	{
		/// CTF
		
		Gyro::World& world = g_World::Get();
		GActorHandle player = world.GetActorByName( "PlayerOne" );
		
		if ( player )
		{
			PlayerComponent* pC = GetComponent<PlayerComponent>( player );
			pC->HandleInput(player, i_CharID, i_keyDown);

			QuatTestComp* comp = GetComponent<QuatTestComp>( player );
			if ( comp )
				comp->HandleInput( player, i_CharID, i_keyDown );
		}
	}

	void BeginUpdate()
	{
	}

	void Update()
	{
		GActorHandle playerHandle = g_World::Get().GetActorByName( "PlayerOne" );
		GActor* player = GActor::FromHandle( playerHandle );
		if( player )
			g_AudioManager.UpdateListenerPosition( player->m_position, player->m_rotation.Dir(), player->m_rotation.Up() );
		/// CTF
		//g_CTFManager::Get().Update();

	}

	void EndUpdate()
	{
	}

}