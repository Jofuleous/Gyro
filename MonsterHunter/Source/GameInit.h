#ifndef __GAME_INIT_H_
#define __GAME_INIT_H_

#include "Math/GVector3.h"
#include "Utility/GSharedPtr.h"
#include "Actor/GActor.h"

/*
GameInit registers all game specific functions, components, and objects with their respective engine system.
*/
namespace GameInit
{
	void RegisterComponents( void );

	void RegisterTriggerFuncs(void);

	void DeRegisterTriggerFuncs( void );

	// declare trigger functions in here, I guess?
	void PlayerDoubleSpeed( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal );
	void PlayerSpeedOriginal( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal );
	void Kill( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal );
	void ScoreTeamOne( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal );
	void ScoreTeamTwo( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal );
	void JumpTrigger( GActorHandle i_trigger, GActorHandle i_inside, GVector3 i_collisionNormal );

}

#endif