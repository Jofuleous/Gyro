#include "GPhysics.h"
#include <assert.h>
#include "Actor/GActor.h"
#include "Utility/Clock.h"
#include "Core/Loop.h"
#include "Actor/ComponentManager.h"
#include "Collision/CollisionManager.h"
#include "Components/GCollisionComp.h"
#include "Components/GGroundFollowerComp.h"
#include "Components/HealthComponent.h"
#include "Math/GAABB.h"
#include <math.h>

static int MAX_RESOLUTION_ATTEMPTS = 5;

bool GPhysics::Initialize( )
{
	g_Collision::Get().Initialize();

	RegisterComponent<GCollisionComp>();
	// for now, register here.
	RegisterComponent<GGroundFollowerComp>();
	// hack, need game common init
	RegisterComponent<HealthComponent>();

	return true;
}

void GPhysics::Update( )
{

	using namespace Gyro;

	//update position
	float dt = (float) g_Clock::Get().SecondsSinceLastFrame();

	// calculate the new velocities for this frame...
	std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();
	int count = m_bodyDatabase.size();

	GContact contact = g_Collision::Get().GetMostRecentContact();

	while( count-- )
	{
		GRigidBody* body = (*iter);

		//Verlet Integrator
		GVector3 oldVelocity = body->m_velocity;

		// terminal velocity.
		if( body->m_acceleration.y() > -100.0f )
			body->m_acceleration += body->m_gravity;

			
		GVector3 finalVelocity = body->m_velocity + ( body->m_acceleration * dt );
			
		if( abs(finalVelocity.x()) < 0.01f )
			finalVelocity.x( 0.0f );
		if( abs(finalVelocity.y()) < 0.01f )
			finalVelocity.y( 0.0f );
		if( abs(finalVelocity.z()) < 0.01f )
			finalVelocity.z( 0.0f );
			

		body->m_velocity = finalVelocity;
		iter++;

		body->m_velocity = finalVelocity;
	}

	g_Collision::Get().BeginFrame();
	g_Collision::Get().FindContacts( (float)dt, 0.0f );

	// we need to run a collision check for the duration of the entire frame first to determine if there are any contacts.
	int attempts = 0;
	float timeToCollision = 0.0f;
	do
	{
		if( g_Collision::Get().m_needsResolution )
		{
			contact = g_Collision::Get().GetMostRecentContact();

			// now re-run the physics calculation...advance everything to the time of collision.
			timeToCollision = contact.m_time * dt;

			iter = m_bodyDatabase.begin();
			count = m_bodyDatabase.size();

			while( count-- )
			{
				GRigidBody* body = (*iter);
				float y = body->m_lastPosition.y();
				body->m_lastPosition = body->m_lastPosition + body->m_velocity * (float)timeToCollision;
				iter++;
			}
				
			// for now, just reflect and continue. first = purple
			GVector3 a;
			GVector3 b;

			// if we were using conservation of momentum, this is what we would want to do.
			//InEllasticCollision( *contact.m_first->m_rigidBody, *contact.m_second->m_rigidBody, 0.5f, a, b );

			GVector3 f_normal = contact.m_firstNormal; // f = first i.e f_normal = first normal
			
			if( contact.m_first->m_rigidBody != NULL )
			{
				GVector3 f_vel = -contact.m_first->m_rigidBody->m_velocity;
				contact.m_first->m_rigidBody->m_velocity = (2 * f_normal * f_vel.Dot( f_normal )) - f_vel;
			}

			if( contact.m_second->m_rigidBody != NULL )
			{
				f_normal = -f_normal;
				GVector3 s_vel = -contact.m_second->m_rigidBody->m_velocity;
				contact.m_second->m_rigidBody->m_velocity = (2 * f_normal * s_vel.Dot( f_normal )) - s_vel;
			};

			// call the callbacks for this most recent collision pair.
			g_Collision::Get().HandleCallbacks();

			dt = dt - timeToCollision;
		}

	g_Collision::Get().Clear();


	g_Collision::Get().FindContacts( (float)dt, timeToCollision );
	attempts++;

	}
	while( g_Collision::Get().m_needsResolution && (attempts < MAX_RESOLUTION_ATTEMPTS ) );

	iter = m_bodyDatabase.begin();
	count = m_bodyDatabase.size();

	// we either gave up, or there are no more collisions until the end of the frame.
	// advance to the end of the frame.
	while( count-- )
	{
		GRigidBody* body = (*iter);
		body->m_lastPosition = body->m_lastPosition + body->m_velocity * (float)dt;
		iter++;
	}
		
	g_Collision::Get().EndFrame();
}

void GPhysics::EndUpdate( )
{
	std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();

	unsigned int count = m_bodyDatabase.size();

	while( count-- )
	{
		GRigidBody* body = (*iter);
		iter++;
	}

}

void GPhysics::Cleanse()
{
	std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();

	while( iter != m_bodyDatabase.end() )
	{	
		delete (*iter);
		iter++;
	}

	m_bodyDatabase.clear();
}

void GPhysics::Shutdown()
{
	std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();

	while( iter != m_bodyDatabase.end() )
	{	
		delete (*iter);
		iter++;
	}

	g_Collision::Get().Shutdown();

	m_bodyDatabase.clear();
}

void GPhysics::AddRigidBody( GRigidBody* i_object )
{
	m_bodyDatabase.push_back( i_object );
}


// there really should be a faster way of doing this.  do a hashed map later...
void GPhysics::RemoveRigidBody( GRigidBody* i_object )
{
	for( u32 i = 0; i < m_bodyDatabase.size(); i++ )
	{
		if( i_object== m_bodyDatabase[i] )
		{
			m_bodyDatabase[i] = m_bodyDatabase.back();
			m_bodyDatabase.pop_back();
		}
	}
}

void GPhysics::RemoveDeadObjects( )
{
	std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();

	unsigned int count = m_bodyDatabase.size();

	for( u32 i = 0; i < m_bodyDatabase.size(); )
	{
		GActor* actor = GActor::FromHandle( m_bodyDatabase[i]->m_actor );
		if( !actor )
		{
			GRigidBody* last = m_bodyDatabase.back();
			delete m_bodyDatabase[i];
			m_bodyDatabase[i] = last;
			m_bodyDatabase.pop_back();
			continue;
		}
		i++;
	}

	g_Collision::Get().RemoveDeadActors();
}

void GPhysics::InEllasticCollision( GRigidBody& i_bodyA, GRigidBody& i_bodyB, float i_coefficient, GVector3& o_a, GVector3& o_b )
{
	o_a = (i_coefficient * i_bodyB.m_mass * (i_bodyB.m_velocity - i_bodyA.m_velocity) + (i_bodyA.m_mass * i_bodyA.m_velocity) + (i_bodyB.m_mass * i_bodyB.m_velocity))
		/ (i_bodyA.m_mass + i_bodyB.m_mass );

	o_b = (i_coefficient * i_bodyA.m_mass * (i_bodyA.m_velocity - i_bodyB.m_velocity) + (i_bodyA.m_mass * i_bodyA.m_velocity) + (i_bodyB.m_mass * i_bodyB.m_velocity))
		/ (i_bodyA.m_mass + i_bodyB.m_mass );
}
