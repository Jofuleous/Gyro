#include "GameStateManager.h"
#include "SpawnManager.h"
#include "Utility/Clock.h"
#include "LocationSpawn.h"
#include "LauncherSpawn.h"
#include "IndividualSpawn.h"
#include "SpawnerSpawn.h"
#include "Collision/CollisionManager.h"
#include "EnemySpawnerData.h"
#include <stdlib.h>     /* qsort */


static int cmpare2 (const void * a, const void * b)
{
	SpawnData* ad = *((SpawnData**)a);
	SpawnData* bd = *((SpawnData**)b);

	//SpawnData* aad = (*ad);
	//SpawnData* bbd = (*bd);

	return (int)( (ad)->m_Time - (bd)->m_Time );
}


void SpawnManager::Initialize( )
{
	// load the spawn list lua.
	m_luaState = LuaPlus::LuaState::Create( true );
		if( m_luaState->DoFile( "data/GameProgression.spawn" ) )
			if( m_luaState->GetTop() == 1 )
				assert( 0 ); // an error occured.

	{

		LuaPlus::LuaObject categories = m_luaState->GetGlobal( "Progression" );


		for ( LuaPlus::LuaTableIterator it( categories["CharacterFiles"] ); it; it.Next() )
		{
			char name[128];
			sprintf( name, "data/Characters/%s.carx", it.GetValue().GetString() );
			if( m_luaState->DoFile( name ) )
			{
				if( m_luaState->GetTop() == 1 )
					assert( 0 ); // an error occured.
			}
		}

		LuaPlus::LuaObject bounds = categories["Bounds"];
		if( !bounds.IsNil() )
		{
			// create the bounds aabb.
			if( !bounds["AABB"].IsNil() )
			{
				bounds = bounds["AABB"];
				float x, y, z, halfX, halfY, halfZ;
				x = bounds["Center"][1].GetFloat();
				y = bounds["Center"][2].GetFloat();
				z = bounds["Center"][3].GetFloat();
				halfX = bounds["HalfX"].GetFloat();
				halfY = bounds["HalfY"].GetFloat();
				halfZ = bounds["HalfZ"].GetFloat();

				m_bounds = GAABB( GVector3( x, y, z ), halfX, halfY, halfZ );
			}
		}

		for ( LuaPlus::LuaTableIterator it( categories["Spawns"] ); it; it.Next() )
		{
			LuaPlus::LuaObject obj = it.GetValue();

			// this could be data driven.  since i'm saving time, i'm not going to bother with some kind of registration/factory system...
			const char* spwnType = obj["Type"].GetString();
			SpawnData* data;

			if( strcmp( spwnType, "Launcher" ) == 0 )
			{
				data = new LauncherSpawn();
				data->m_Type = LAUNCHER;
			}
			else if( strcmp( spwnType, "Spawner" ) == 0 )
			{
				data = new LauncherSpawn();
				data->m_Type = SPAWNER;
			}
			else if( strcmp( spwnType, "Location" ) == 0 )
			{
				data = new LocationSpawn();
				data->m_Type = LOCATION;
			}
			else if( strcmp( spwnType, "Individual" ) == 0 )
			{
				data = new IndividualSpawn();
				data->m_Type = INDIVIDUAL;
			}
			else if( strcmp( spwnType, "EnemySpawner" ) == 0 )
			{
				data = new EnemySpawnerData();
				data->m_Type = ENEMYSPAWNER;
			}

			data->Load( it.GetValue() );
			m_spawnList.push_back( data );
		}

		qsort( &m_spawnList[0], m_spawnList.size(), sizeof( SpawnData*), cmpare2 );
	}

	m_startTime = g_Clock::Get().SecondsSinceStart();

	m_nextSpawn = 0;
}

void SpawnManager::Cleanse( )
{
	for( int i = 0; i < m_spawnList.size(); i++ )
		delete m_spawnList[i];
	m_spawnList.clear();
	m_updateSpawns.clear();

	LuaPlus::LuaState::Destroy( m_luaState );
}


void SpawnManager::Update( )
{
	float gameTime = g_Clock::Get().SecondsSinceStart() - m_startTime;

	while( m_nextSpawn < m_spawnList.size() && m_spawnList[m_nextSpawn]->m_Time < gameTime )
	{
		m_spawnList[m_nextSpawn]->Initialize( );
		m_updateSpawns.push_back( m_spawnList[m_nextSpawn] );
		m_nextSpawn++;
	}

	for( int i = 0; i< m_updateSpawns.size(); i++ )
	{
		m_updateSpawns[i]->Update( gameTime );
		if( m_updateSpawns[i]->m_Complete )
		{
			m_updateSpawns[i] = m_updateSpawns.back();
			m_updateSpawns.pop_back();
		}
	}

}

float SpawnManager::FindYValue( float x, float z )
{
	// cast a ray downward to get the y-value of this location...
	GRayQuery queryDown( GVector3( x, 1000000.0f, z ), GVector3( 0.0f, 1.0f, 0.0f ), g_Collision::Get().GetCollisionMask( "TERRAIN" ), 1000000.0f );
	if( g_Collision::Get().RayQuery( queryDown ) )
		return 1000000.0f - queryDown.m_distance;

	return 0.0f;
}