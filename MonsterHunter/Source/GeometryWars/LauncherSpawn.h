#ifndef __LAUNCHER_SPAWN_H_
#define __LAUNCHER_SPAWN_H_

#include "SpawnData.h"

class LauncherSpawn : public SpawnData
{
	virtual void Load( LuaPlus::LuaObject& i_luaObject )
	{

	}

	virtual void Initialize( )
	{

	}

	virtual void Update( float i_dt )
	{

	}

	float	m_launchHeight;
	float	m_launchDistance;
};

#endif