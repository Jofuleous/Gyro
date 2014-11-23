#ifndef __PHYSICS_COMPONENT_H_
#define __PHYSICS_COMPONENT_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Utility/GSharedPtr.h"
#include "Physics/GPhysics.h"
#include "Collision/CollisionManager.h"


//Interface to the physics system object...

class GCollisionComp : public IActorComponent
{
public:

	//Required
	static const GHashedString		m_typeName;
	static u32						m_typeId;	

									GCollisionComp( void ) : m_RigidBody( NULL ), m_Collider( NULL ){}
	virtual							~GCollisionComp( void );

	virtual void					Update( GActorHandle actor );
	virtual void					EndUpdate( GActorHandle actor );
	virtual void					Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual IActorComponent*	Create();
	virtual GHashedString			Name( void ) const { return m_typeName; };
	virtual bool					HasUpdate( ) const { return false; };
	virtual bool					HasEndUpdate( ) const { return true; };
	GRigidBody*						GetRigidBody( ) { return m_RigidBody; }

protected:
	GRigidBody*						m_RigidBody;
	GColliderData*					m_Collider;
	
};

#endif