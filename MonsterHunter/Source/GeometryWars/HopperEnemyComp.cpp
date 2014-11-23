#include "HopperEnemyComp.h"
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

u32 HopperEnemyComp::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString HopperEnemyComp::m_typeName = "HopperEnemy";

void HopperEnemyComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_nextHop = 0.0f;
}

HopperEnemyComp::~HopperEnemyComp()
{

}

IActorComponent* HopperEnemyComp::Create()
{
	// this is why I need to replace this with a creation function pointer...
	return new HopperEnemyComp();
}

void HopperEnemyComp::Update( GActorHandle i_actor )
{

	//Pissed
	float curSeconds = (float) g_Clock::Get().SecondsSinceStart();
		
	//float heading = (float)curSeconds * (1.0f*GMath::Pi ); 
	//GMatrix4 rots;
	//rots.SetRotY( heading );
	//i_actor->m_rotation = rots;
	GActor* player = GActor::FromHandle( m_player );
	GActor* actor = GActor::FromHandle( i_actor );

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

	float sinceStart = g_Clock::Get().SecondsSinceStart();
	if( m_nextHop < sinceStart )
	{
		GGroundFollowerComp* gHopper = GetComponent<GGroundFollowerComp>( i_actor );
		if( gHopper && gHopper->m_onGround )
		{
			GVector3 dist = player->m_position - actor->m_position;
			GVector3 dir;
			if( dist.LengthSquared() > 0.0f )
				dir = dist.Normal();
			else
				dir = dist;

			float angle = atan2( dist.x(), dist.y() );

			actor->m_rotation.SetRotY( angle );

			gHopper->ApplyAccelerationVectorXZ( dir.x() * gHopper->m_accelerationXZ, dir.z() * gHopper->m_accelerationXZ );

			gHopper->ApplyJumpAccelerationY( 7000.0f ); // data drive 
		}

		m_nextHop = sinceStart + 3.0f;
	}

}

void HopperEnemyComp::EndUpdate( GActorHandle i_actor)
{

}

bool HopperEnemyComp::WithinBounds( float width, float height, GActorHandle i_actor ) const
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