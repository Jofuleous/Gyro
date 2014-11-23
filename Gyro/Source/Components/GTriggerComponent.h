#ifndef __GTRIGGERCOMPONENT_H_
#define __GTRIGGERCOMPONENT_H_

#include "Actor/IActorComponent.h"
#include "Utility/GSharedPtr.h"
#include "Math/GVector3.h"
#include "Actor/GActor.h"
#include <vector>

class GTriggerComponent : public IActorComponent
{
public:
	// probably wont wind up using the collision normal....
	typedef void (*OnTriggerEvent) ( GActorHandle i_trigger, GActorHandle i_entering, GVector3 i_collisionNormal );

	//Required
	static const GHashedString		m_typeName;
	static u32						m_typeId;

	GTriggerComponent( void ) { }
	~GTriggerComponent( void )
	{
		m_currentData.clear();
	}

	virtual void					Update( GActorHandle actor );
	virtual void					EndUpdate( GActorHandle actor );
	virtual void					Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual IActorComponent*		Create( );
	virtual GHashedString			Name( void ) const { return m_typeName; }

	virtual bool					HasUpdate() const { return false; };
	virtual bool					HasEndUpdate() const { return true; };

	// i may want to use the normal later...
	void							OnCollide( GActorHandle i_us, GActorHandle i_them, GVector3 i_collisionNormal );

	class TriggerData
	{
	public:
		TriggerData( GActorHandle i_actor, bool i_enteredThisFrame ) : m_actor( i_actor ), m_enteredThisFrame( i_enteredThisFrame ) {}
		~TriggerData() { }
		GActorHandle	m_actor;
		bool							m_enteredThisFrame;
	};

	std::vector<OnTriggerEvent>		m_onEnters;
	std::vector<OnTriggerEvent>		m_onPersists;
	std::vector<OnTriggerEvent>		m_onExits;
	std::vector<TriggerData>		m_currentData;

};

#endif