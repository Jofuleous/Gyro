#ifndef __PHYSICS_COMPONENT_H_
#define __PHYSICS_COMPONENT_H_

#include "Actor/Actor.h"
#include "Actor/IActorComponent.h"
#include "Utility/SharedPtr.h"
#include "Physics/GPhysics.h"
#include "Collision/CollisionManager.h"


//Interface to the physics system object...

class GPhysicsComponent : public Gyro::IActorComponent
{
public:

	//Required
	static const GHashedString		m_typeName;
	static u32						m_typeId;	

									GPhysicsComponent( void ) : m_RigidBody( NULL ), m_Collider( NULL ){}
	virtual							~GPhysicsComponent( void );

	virtual void					Update( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					EndUpdate( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					Initialize( Gyro::SharedPtr<Gyro::Actor> actor, LuaPlus::LuaObject& i_obj );
	virtual Gyro::IActorComponent*	Create();
	virtual GHashedString			Name( void ) const { return m_typeName; };
	virtual bool					HasUpdate( ) const { return false; };
	virtual bool					HasEndUpdate( ) const { return true; };
	GRigidBody*						GetRigidBody( ) { return m_RigidBody; }

protected:
	GRigidBody*						m_RigidBody;
	GColliderData*					m_Collider;
	
};

#endif