#ifndef __SPINNERENEMY_COMP_H_
#define __SPINNERENEMY_COMP_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Core/GMessageManager.h"


class SpinnerEnemyComp : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	SpinnerEnemyComp( )
	{
	}
	virtual								~SpinnerEnemyComp( void );

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	GActorHandle						m_player;  // the true test of smart pointers.
	GVector3							m_targetDir;
	float								m_nextRedirect;
};

#endif