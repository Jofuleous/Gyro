#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Core/GMessageManager.h"


class FollowEnemyComp : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	FollowEnemyComp( )
	{
	}
	virtual								~FollowEnemyComp( void );

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	GActorHandle	m_player;  // the true test of smart pointers.

protected:

	bool					WithinBounds( float width, float height, GActorHandle i_actor ) const ;
};