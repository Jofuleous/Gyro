#ifndef __PHYSICS_COMPONENT_H_
#define __PHYSICS_COMPONENT_H_

#include "Actor/Actor.h"
#include "Actor/IActorComponent.h"
#include "Helper/SharedPtr.h"
#include "GPhysics.h"
#include "Collision/CollisionManager.h"


//Interface to the physics system object...

class PhysicsComponent : public Gyro::IActorComponent
{
public:

	//Required
	static const GHashedString		m_typeName;
	static u32						m_typeId;	

									PhysicsComponent( void ) : m_RigidBody( NULL ), m_Collider( NULL ){}
	virtual							~PhysicsComponent( void );

	virtual void					Update( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					EndUpdate( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					Initialize( Gyro::SharedPtr<Gyro::Actor> actor, libconfig::Setting& i_setting );
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