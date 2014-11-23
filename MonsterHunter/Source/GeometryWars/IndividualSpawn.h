#ifndef __INDIVIDUAL_SPAWN_H_
#define __INDIVIDUAL_SPAWN_H_

#include "SpawnData.h"

class IndividualSpawn : public SpawnData
{
	virtual void Load( LuaPlus::LuaObject& i_luaObject )
	{
		SpawnData::Load( i_luaObject );
	}

	virtual void Initialize()
	{

	}

	virtual void Update( float i_dt )
	{
	}
};

#endif