#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <vector>
#include "Math/GVector3.h"
#include "Math/GMatrix4.h"
#include "Utility/Singleton.h"
#include "Actor/GActor.h"

class GActor;

class GRigidBody
{
public:
	GRigidBody( ) { }
	~GRigidBody( ) { }

	GRigidBody( GActorHandle i_actor ):
		m_actor( i_actor ){ }

	GRigidBody( GActorHandle i_actor, float i_mass ) :
		m_actor( i_actor ),
		m_mass( i_mass ),
		m_acceleration( GVector3::Zero ),
		m_velocity( GVector3::Zero ),
		m_gravity( 0.0f, 0.0f, 0.0f ){ }

	GActorHandle	m_actor;

	GVector3			m_velocity;
	GVector3			m_acceleration;
	GVector3			m_lastPosition;
	float				m_mass; // this isn't being used right now...
	GVector3			m_gravity;
	bool				m_resolves; // hack.  remove this later.

	bool				m_markedForDelete;
};

class GPhysics
{
friend Singleton<GPhysics>;

public:
	void							Shutdown( );
	void							Cleanse();

	bool							Initialize( );
	void							Update( );
	void							EndUpdate( );
	void							AddRigidBody( GRigidBody* i_object );
	void							RemoveRigidBody( GRigidBody* i_object );
	void							RemoveDeadObjects();


	static void						ElasticCollision( GRigidBody& i_bodyA, GRigidBody& i_body );
	static void						InEllasticCollision( GRigidBody& i_bodyA, GRigidBody& i_body, float i_coefficient, GVector3& o_a, GVector3& o_b );

private:
									GPhysics( ) { }
									GPhysics( GPhysics& physics ) {}
		
	// lots of optimizations to be made here...  ideally, this data should all be one big chunk of contiguous memory.
	// more than likely, we'd want to use a memory pool since the data is a fixed size.
	std::vector<GRigidBody*>		m_bodyDatabase;
};


typedef Singleton< GPhysics > g_Physics;

#endif