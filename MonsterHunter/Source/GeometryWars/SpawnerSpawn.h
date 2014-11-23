#ifndef __SPAWNER_SPAWN_H_
#define __SPAWNER_SPAWN_H_

#include "SpawnData.h"

class SpawnerSpawn : public SpawnData
{
	virtual void Load( LuaPlus::LuaObject& i_luaObject )
	{

		SpawnData::Load( i_luaObject );

		if( i_luaObject["Location"].IsTable() )
		{
			m_location = GVector3(i_luaObject["Location"][1].GetNumber(),
								i_luaObject["Location"][2].GetNumber(),
								i_luaObject["Location"][3].GetNumber() );
			m_random = false;
		}
		else
			m_random = true;
	}

	virtual void Initialize( )
	{
		
	}
	virtual void Update( float i_gameTime )
	{

	}

	bool		m_random;
	GVector3	m_location; // will this be used?  maybe not.
};

#endif