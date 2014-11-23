#ifndef __HEALTH_COMPONENT_H_
#define __HEALTH_COMPONENT_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"

class HealthComponent : public IActorComponent
{
public:
	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	HealthComponent( )
	{

	}
	virtual								~HealthComponent( void ) { }

	virtual void						Update( GActorHandle actor ) {}
	virtual void						EndUpdate( GActorHandle actor ) {}
	virtual bool						HasUpdate( ) const { return false; }
	virtual bool						HasEndUpdate( ) const { return false; }
	virtual IActorComponent*			Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	 // Do damage takes in the owner because components currently don't know which actor they belong to because I had to do that to get the grade...
	void								DoDamage( int i_ammount, GActorHandle i_owner, GActorHandle i_attacker );

	int		m_MaxHealth;
	int		m_CurrentHealth;
	bool	m_KillOnHealthZero;
};

#endif