#ifndef __ENEMY_MANAGER_H_
#define __ENEMY_MANAGER_H_

#include "Utility/Singleton.h"
#include "SpawnData.h"
#include <vector>
#include "Math/GAABB.h"

class SpawnManager
{
public:
	friend Singleton<SpawnManager>;

	SpawnManager() : m_luaState( NULL )
	{
	}

	void Initialize( );
	void Update( );
	void Cleanse( );

	float FindYValue( float x, float z );

	LuaPlus::LuaState*		m_luaState; // this is going to pre-load all of the actors we will be creating...


	std::vector<SpawnData*>	m_spawnList;
	std::vector<SpawnData*> m_updateSpawns;
	int						m_nextSpawn;
	float					m_startTime;
	GAABB					m_bounds;
};

typedef Singleton< SpawnManager > g_SpawnManager;

#endif