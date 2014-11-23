#ifndef __LOCATION_SPAWN_H_
#define __LOCATION_SPAWN_H_

#include "SpawnData.h"
#include "Core/World.h"

class LocationSpawn : public SpawnData
{
	virtual void Load( LuaPlus::LuaObject& i_luaObject )
	{
		SpawnData::Load( i_luaObject );

		m_radius = i_luaObject["Radius"].GetNumber();

		m_location = GVector3(i_luaObject["Location"][1].GetNumber(),
							i_luaObject["Location"][2].GetNumber(),
							i_luaObject["Location"][3].GetNumber() );
	}

	virtual void Initialize()
	{

	}

	virtual void Update( float i_gameTime )
	{
		if( i_gameTime > m_LastSpawn + m_Frequency )
		{
			// do the creation...
			m_LastSpawn = i_gameTime;
			
			int id = m_SpawnTotal % m_EnemyTypeCount;
			const char* type = &m_EnemyTypes[32 * id];

			GActorHandle actor = GActor::CreateActor( type, m_location, g_SpawnManager::Get().m_luaState );
			g_World::Get().AddActor( actor );
			m_SpawnTotal++;
		}

		if( m_SpawnTotal >= m_Count )
			m_Complete = true;
	}

	float		m_radius;
	GVector3	m_location;
};

#endif