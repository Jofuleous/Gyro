#include "SpinnerEnemyComp.h"
#include "Physics/GPhysics.h"
#include "Utility/Clock.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Misc/UserSettings.h"
#include "Collision/CollisionManager.h"
#include "Components/GGroundFollowerComp.h"
#include "../GeometryWars/SpawnManager.h"

u32 SpinnerEnemyComp::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString SpinnerEnemyComp::m_typeName = "SpinnerEnemy";

void SpinnerEnemyComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{

	if( !m_player )
	{
		m_player = g_World::Get().GetActorByName( "QuakePlayer" );
		if( !m_player )
		{
			m_targetDir = GVector3( 0.0f, 0.0f, 0.0f );
			return;
		}
	}

	GActor* actor = GActor::FromHandle( i_actor );
	GActor* player = GActor::FromHandle( m_player );
	m_targetDir = player->m_position - actor->m_position;
	if( m_targetDir.LengthSquared() > 0.0f )
		m_targetDir.Normalize();

	m_nextRedirect = g_Clock::Get().SecondsSinceStart() + 7.0f;
}

SpinnerEnemyComp::~SpinnerEnemyComp()
{

}

IActorComponent* SpinnerEnemyComp::Create()
{
	// this is why I need to replace this with a creation function pointer...
	return new SpinnerEnemyComp();
}

void SpinnerEnemyComp::Update( GActorHandle i_actor )
{

	//Pissed
	float curSeconds = (float) g_Clock::Get().SecondsSinceStart();
	GActor* actor = GActor::FromHandle( i_actor );
	GActor* player = GActor::FromHandle( m_player );
		
	float heading = (float)curSeconds * (-GMath::Pi ); 
	GMatrix4 rots;
	rots.SetRotY( heading );
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

	GMatrix4 ident = GMatrix4::Identity;
	if( !g_SpawnManager::Get().m_bounds.PointInside( actor->m_position, ident ) ||
		g_Clock::Get().SecondsSinceStart() > m_nextRedirect )
	{
		m_targetDir = player->m_position - actor->m_position;
		m_targetDir.Normalize();
		GGroundFollowerComp* gFollower = GetComponent<GGroundFollowerComp>( i_actor );

		if( gFollower )
			gFollower->SetVelocity( gFollower->m_maxSpeedXZ * m_targetDir );

		m_nextRedirect = g_Clock::Get().SecondsSinceStart() + 7.0f;
	}


}

void SpinnerEnemyComp::EndUpdate( GActorHandle i_actor)
{

}