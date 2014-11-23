#include "FollowEnemyComp.h"
#include "Physics/GPhysics.h"
#include "Utility/Clock.h"
//#include "Renderer/GRenderer.h"
//#include "Components/GRenderComponent.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Misc/UserSettings.h"
#include "Collision/CollisionManager.h"
#include "Components/GGroundFollowerComp.h"

#include <stdlib.h> 

// no longer being used.

u32 FollowEnemyComp::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString FollowEnemyComp::m_typeName = "FollowerEnemy";

void FollowEnemyComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{

}

FollowEnemyComp::~FollowEnemyComp()
{

}

IActorComponent* FollowEnemyComp::Create()
{
	// this is why I need to replace this with a creation function pointer...
	return new FollowEnemyComp();
}

void FollowEnemyComp::Update( GActorHandle i_actor )
{

	//Pissed
	float curSeconds = (float) g_Clock::Get().SecondsSinceStart();
		
	float heading = (float)curSeconds * (1.0f*GMath::Pi ); 
	GMatrix4 rots;
	rots.SetRotY( heading );
	GActor* actor = GActor::FromHandle( i_actor );
	GActor* player = GActor::FromHandle( m_player );

	actor->m_rotation = rots;

	if( !player )
	{
		m_player = g_World::Get().GetActorByName( "QuakePlayer" );
		player = GActor::FromHandle( m_player );
		if( !player )
			return;
	}

	if( player->m_markedForDelete )
	{
		m_player = GActorHandle( 0, 0, 0 );
		return;
	}

	// move toward the player with some velocity.
	GGroundFollowerComp* gFollower = GetComponent<GGroundFollowerComp>( i_actor );
	if( gFollower )
	{
		GVector3 dist = player->m_position - actor->m_position;
		GVector3 dir;
		if( dist.LengthSquared() > 0.0f )
			dir = dist.Normal();
		else
			dir = dist;

		dir.y( 0.0f );
		gFollower->ApplyAccelerationVectorXZ( dir.x() * gFollower->m_accelerationXZ, dir.z() * gFollower->m_accelerationXZ );
	}

}

void FollowEnemyComp::EndUpdate( GActorHandle i_actor)
{

}

bool FollowEnemyComp::WithinBounds( float width, float height, GActorHandle i_actor ) const
{
	int fixedWidth = (int)width/2;
	int fixedHeight = (int)height/2;

	GActor* actor = GActor::FromHandle( i_actor );
	if( !actor )
		return false;

	if( actor->m_position.x() < fixedWidth && actor->m_position.x() > -fixedWidth )
		if( actor->m_position.y() < fixedHeight && actor->m_position.y() > -fixedHeight )
			return true;

	return false;
}