#include "Physics.h"
#include <assert.h>
#include "Actor/Actor.h"
#include "Core/Clock.h"
#include "Core/Loop.h"
#include "Actor/ComponentManager.h"
#include "PhysicsComponent.h"
#include "Collision/CollisionManager.h"
#include <math.h>

namespace Gyro
{

	bool Physics::Initialize( )
	{
		RegisterComponent<PhysicsComponent>();
		return true;
	}

	void Physics::Update( )
	{
		//update position
		double dt = g_Clock::Get().SecondsSinceLastFame();


		// we need to run a collision check for the duration of the entire frame first to determine if there are any contacts.
		int giveUpAttempts = 30;
		int attempts = 0;

		// calculate the new velocities for this frame...
		std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();
		int count = m_bodyDatabase.size();

		while( count-- )
		{
			GRigidBody* body = (*iter);

			//Verlet Integrator
			Vector3 oldVelocity = body->m_velocity;
			Vector3 finalVelocity = body->m_velocity + ( body->m_acceleration * (float)dt );
			body->m_velocity = finalVelocity;
			iter++;
		}

		g_Collision::Get().FindContacts( (float)dt );
		//attempts++;

		do
		{
			if( g_Collision::Get().m_needsResolution )
			{
				GContact contact = g_Collision::Get().GetMostRecentContact();

				// now re-run the physics calculation...advance everything to the time of collision...
				float timeToCollision = contact.m_time * dt;

				iter = m_bodyDatabase.begin();
				count = m_bodyDatabase.size();

				while( count-- )
				{
					GRigidBody* body = (*iter);

					//for contact resolution later.
					body->m_lastPosition = body->m_lastPosition + body->m_velocity * (float)timeToCollision;
			
					iter++;
				}
				//first normal is the normal that the first object collided with.  AKA, the second object's normal...not sure if i like that...but meh
				
				// for now, just reflect and continue.
				if( contact.m_first->m_rigidBody != NULL )
				{
					contact.m_first->m_rigidBody->m_velocity = (2.0f * contact.m_firstNormal * contact.m_firstNormal.Dot( -contact.m_first->m_rigidBody->m_velocity )) - -contact.m_first->m_rigidBody->m_velocity;
					//Gyro::Reflect( contact.m_first->m_rigidBody->m_velocity, contact.m_firstNormal, contact.m_first->m_rigidBody->m_velocity );
				}

				if( contact.m_second->m_rigidBody != NULL )
					contact.m_second->m_rigidBody->m_velocity = 2.0f * -contact.m_firstNormal * -contact.m_second->m_rigidBody->m_velocity.Dot( -contact.m_firstNormal) - -contact.m_second->m_rigidBody->m_velocity;

				dt = dt - timeToCollision;
			}

		g_Collision::Get().Clear();

		g_Collision::Get().FindContacts( (float)dt );
		attempts++;

		} while( g_Collision::Get().m_needsResolution && (attempts < giveUpAttempts) );

		iter = m_bodyDatabase.begin();
		count = m_bodyDatabase.size();

		while( count-- )
		{
			GRigidBody* body = (*iter);

			//for contact resolution later.
			body->m_lastPosition = body->m_lastPosition + body->m_velocity * (float)dt;
			
			iter++;
		}
	}

	void Physics::EndUpdate( )
	{
		std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();

		unsigned int count = m_bodyDatabase.size();

		while( count-- )
		{
			GRigidBody* body = (*iter);
			iter++;
		}

	}

	void Physics::Shutdown()
	{
		std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();

		while( iter != m_bodyDatabase.end() )
		{	
			delete (*iter);
			iter++;
		}

		m_bodyDatabase.clear();
	}

	void Physics::AddRigidBody( GRigidBody* i_object )
	{
		m_bodyDatabase.push_back( i_object );
	}

	void Physics::RemoveDeadObjects( )
	{
		std::vector< GRigidBody* >::iterator iter = m_bodyDatabase.begin();

		unsigned int count = m_bodyDatabase.size();

		for( u32 i = 0; i < m_bodyDatabase.size(); )
		{
			if( m_bodyDatabase[i]->m_actor->m_markedForDelete )
			{
				GRigidBody* last = m_bodyDatabase.back();
				delete m_bodyDatabase[i];
				m_bodyDatabase[i] = last;
				m_bodyDatabase.pop_back();
				continue;
			}
			i++;
		}
	}

}