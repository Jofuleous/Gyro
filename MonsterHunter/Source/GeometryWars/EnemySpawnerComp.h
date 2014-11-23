#ifndef __ENEMYSPAWNERCOMP_H_
#define __ENEMYSPAWNERCOMP_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Core/GMessageManager.h"
#include "../GeometryWars/SpawnData.h"

class EnemySpawnerComp : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	EnemySpawnerComp( )
	{
	}
	virtual								~EnemySpawnerComp( void );

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	GActorHandle		m_player;  // the true test of smart pointers.
	GVector3							m_targetDir;
	float								m_nextSpawn;
	float								m_frequency;
	char								m_EnemyTypes[128];
};

#endif