#ifndef __SPAWNDATA_H_
#define __SPAWNDATA_H_

#include "Math/GVector3.h"
#include "Utility/GHashedString.h"
#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaState.h"
#include "LuaPlus/LuaTableIterator.h"

// this is a really shitty way to do this...but it will work.  i don't really 
// care about performance for this part of the game.  It's miniscule compared to how
// bad the raycasts are going to fuck us....

enum SpawnType
{
	LAUNCHER, // needs to be killed to stop spawning.
	SPAWNER, // needs to be killed to stop spawning.
	LOCATION,
	INDIVIDUAL,
	ENEMYSPAWNER,
};

class SpawnData
{
public:

	SpawnData( ) : m_LastSpawn( 0.0f ), m_SpawnTotal( 0.0f ), m_Complete( false )
	{ }

	float			m_Time;
	int				m_Count;
	int				m_SpawnTotal;
	SpawnType		m_Type;
	float			m_Frequency;
	float			m_LastSpawn;
	int				m_EnemyTypeCount;	// used to index into m_enemyTypes.
	char			m_EnemyTypes[128];  // each type is spaced 32 bytes apart...
	bool			m_Complete;

	virtual void	Load( LuaPlus::LuaObject& i_luaObject ); // for loading spawner-specific data.

	virtual void	Initialize( ) = 0;
	virtual void	Update( float i_gameTime ) = 0;
};

#endif