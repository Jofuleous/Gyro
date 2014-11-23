#ifndef __ENEMYSPAWNERDATA_H_
#define __ENEMYSPAWNERDATA_H_

#include "SpawnData.h"
#include "Core/World.h"
#include "SpawnManager.h"
#include "Actor/GActor.h"
#include "EnemySpawnerComp.h"

class EnemySpawnerData : public SpawnData
{
	virtual void Load( LuaPlus::LuaObject& i_luaObject )
	{
		SpawnData::Load( i_luaObject );

		m_location = GVector3(i_luaObject["Location"][1].GetNumber(),
							i_luaObject["Location"][2].GetNumber(),
							i_luaObject["Location"][3].GetNumber() );

		sprintf( m_spawnerCharacter, "%s", i_luaObject["Spawner"].GetString() );
	}

	virtual void Initialize()
	{

	}

	virtual void Update( float i_gameTime )
	{
		if( !m_Complete )
		{
			GActorHandle actor = GActor::CreateActor( m_spawnerCharacter, m_location, g_SpawnManager::Get().m_luaState );
			g_World::Get().AddActor( actor );


			EnemySpawnerComp* spawner = GetComponent<EnemySpawnerComp>( actor );
			memcpy( spawner->m_EnemyTypes, m_EnemyTypes, 128 );
			spawner->m_frequency = m_Frequency;

			m_Complete = true;
		}
	}

	GVector3	m_location;
	char		m_spawnerCharacter[32];
};

#endif