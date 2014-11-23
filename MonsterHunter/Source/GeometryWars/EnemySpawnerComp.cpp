#include "EnemySpawnerComp.h"
#include "Physics/GPhysics.h"
#include "Utility/Clock.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Misc/UserSettings.h"
#include "Collision/CollisionManager.h"
#include "Components/GGroundFollowerComp.h"
#include "../GeometryWars/SpawnManager.h"

u32 EnemySpawnerComp::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString EnemySpawnerComp::m_typeName = "EnemySpawner";

void EnemySpawnerComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
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

	m_nextSpawn = 0.0f;
}

EnemySpawnerComp::~EnemySpawnerComp()
{

}

IActorComponent* EnemySpawnerComp::Create()
{
	// this is why I need to replace this with a creation function pointer...
	return new EnemySpawnerComp();
}

void EnemySpawnerComp::Update( GActorHandle i_actor )
{
	float curSeconds = (float) g_Clock::Get().SecondsSinceStart();
	
	GActor* actor = GActor::FromHandle( i_actor );
	float heading = (float)curSeconds * (-GMath::Pi ); 
	GMatrix4 rots;
	rots.SetRotY( heading );
	actor->m_rotation = rots;

	if( curSeconds > m_nextSpawn )
	{
		GActorHandle enemy = GActor::CreateActor( m_EnemyTypes, actor->m_position, g_SpawnManager::Get().m_luaState );
		g_World::Get().AddActor( enemy );

		m_nextSpawn = curSeconds + m_frequency;
	}
}

void EnemySpawnerComp::EndUpdate( GActorHandle i_actor)
{

}