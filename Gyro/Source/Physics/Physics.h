#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <vector>
#include "Math/Vector3.h"
#include "Math/GMatrix4.h"
#include "Helper/Singleton.h"
#include "Actor/Actor.h"

//this will be a proper singleton once I get around to creating the macro and class for it...
//also, this basically does almost nothing for now...
namespace Gyro
{
	class Actor;

	// these get physics calculations every frame.
	class GRigidBody
	{
	public:
		GRigidBody( ) { }
		~GRigidBody( ) { }

		GRigidBody( SharedPtr<Actor> i_actor ):
			m_actor( i_actor ){ }

		GRigidBody( SharedPtr<Actor> i_actor, float i_mass ) :
			m_actor( i_actor ),
			m_mass( i_mass ),
			m_acceleration( Vector3::Zero ),
			m_velocity( Vector3::Zero )  { }

		SharedPtr<Actor>	m_actor;

		Vector3				m_velocity;
		float				m_maxSpeed;
		Vector3				m_acceleration;
		Vector3				m_lastPosition; //we'll probably want to save last velocity as well.
		float				m_mass;


		bool				m_markedForDelete;
	};

	class Physics
	{
	friend Singleton<Physics>;

	public:
		void							Shutdown( );

		bool							Initialize( );
		void							Update( );
		void							EndUpdate( );
		void							RemoveDeadObjects( );
		void							AddRigidBody( GRigidBody* i_object );

	private:
										Physics( ) { }
										Physics( Physics& physics ) {}
		
		//lots of optimizations to be made here...
		std::vector<GRigidBody*>		m_bodyDatabase;
	};
}

typedef Singleton< Gyro::Physics > g_Physics;

#endif