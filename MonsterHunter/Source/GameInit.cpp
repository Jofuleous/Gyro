#include "GameInit.h"
#include "Actor/ComponentManager.h"
#include "Components/GTriggerComponent.h"
#include "Components/GTriggerFunctions.h"
#include "Components/PlayerComponent.h"
#include "GeometryWars/QuakePlayerComp.h"
#include "Components/MonsterComponent.h"
#include "Components/GTriggerFunctions.h"
#include "Components/FlagComponent.h"
#include "Components/TeamComponent.h"
#include "GeometryWars/FollowEnemyComp.h"
#include "Components/GGroundFollowerComp.h"
#include "GeometryWars/HopperEnemyComp.h"
#include "GeometryWars/SpinnerEnemyComp.h"
#include "GeometryWars/EnemySpawnerComp.h"
#include "Components/CtfAiPlayerComp.h"
#include "CaptureFlagManager.h"
#include "Components/QuatTestComp.h"
#include "Components/GAnimComponent.h"

namespace GameInit
{
	 void RegisterComponents( void )
	 {
		RegisterComponent<MonsterComponent>();
		RegisterComponent<FollowEnemyComp>();
		RegisterComponent<PlayerComponent>();
		RegisterComponent<FlagComponent>();
		RegisterComponent<TeamComponent>();
		RegisterComponent<HopperEnemyComp>();
		RegisterComponent<SpinnerEnemyComp>();
		RegisterComponent<EnemySpawnerComp>();
		RegisterComponent<QuakePlayerComp>();
		RegisterComponent<CtfAiPlayerComp>();
		RegisterComponent<QuatTestComp>();
		RegisterComponent<GAnimComponent>();
	 }

	 void RegisterTriggerFuncs(void)
	 {
		g_TriggerFuncs::Get().RegisterTriggerFunction( "PlayerDoubleSpeed", PlayerDoubleSpeed);
		g_TriggerFuncs::Get().RegisterTriggerFunction( "PlayerSpeedOriginal", PlayerSpeedOriginal );
		g_TriggerFuncs::Get().RegisterTriggerFunction( "ScoreTeam1", ScoreTeamOne );
		g_TriggerFuncs::Get().RegisterTriggerFunction( "ScoreTeam2", ScoreTeamTwo );
		g_TriggerFuncs::Get().RegisterTriggerFunction( "Kill", Kill );
		g_TriggerFuncs::Get().RegisterTriggerFunction( "JumpTrigger", JumpTrigger );
	 }

	 void DeRegisterTriggerFuncs( void )
	 {
		 g_TriggerFuncs::Release();
	 }

	 void PlayerDoubleSpeed( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal )
	 {
		 // get the rigid body off of what's inside (or exiting, or persisting ).
		 GActor* inside = GActor::FromHandle( i_inside );
		 if( !inside )
			 return;

		 if( inside->IsA( "Player" ) )
		 {
			 // get the player component and increase the max speed.
			 PlayerComponent* pComp = GetComponent<PlayerComponent>( i_inside );
			 if( pComp )
				 pComp->SetSpeedMultiplier( 2.0f );
		 }
	 }

	 void PlayerSpeedOriginal( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal )
	 {
		 GActor* inside = GActor::FromHandle( i_inside );
		 if( !inside )
			 return;

		 if( inside->IsA( "Player" ) )
		 {
			 PlayerComponent* pComp = GetComponent<PlayerComponent>( i_inside );
			 if( pComp )
				 pComp->ReturnNormalSpeed( );
		 }
	 }

	 // set collision bits for this to control who gets killed.
	 void Kill( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal )
	 {
		 GActor* inside = GActor::FromHandle( i_inside );
		 if( !inside )
			 return;

		 inside->Kill();
	 }

	void ScoreTeamOne( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal )
	{
		//TeamComponent* tComp = (TeamComponent*) GetComponent<TeamComponent>( i_inside );
		FlagComponent* flagComponent = (FlagComponent*) GetComponent<FlagComponent>( i_inside );
		if( flagComponent )
		{
			if( flagComponent->m_teamNumber == 2 )
				g_CTFManager::Get().ScoreTeamOne();
		}
	}

	void ScoreTeamTwo( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal )
	{
		FlagComponent* flagComponent = (FlagComponent*) GetComponent<FlagComponent>( i_inside );
		if( flagComponent )
		{
			if( flagComponent->m_teamNumber == 1 )
				g_CTFManager::Get().ScoreTeamTwo();
		}
	}

	void JumpTrigger( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal )
	{
		GGroundFollowerComp* gFollower = (GGroundFollowerComp*) GetComponent<GGroundFollowerComp>( i_inside );
		if( gFollower )
			gFollower->ApplyJumpAccelerationY( 8000.0f );
	}

}