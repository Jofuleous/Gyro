#include "MonsterComponent.h"
#include "Physics/GPhysics.h"
#include "Utility/Clock.h"
//#include "Renderer/GRenderer.h"
//#include "Components/GRenderComponent.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Misc/UserSettings.h"
#include "Collision/CollisionManager.h"

#include <stdlib.h> 

// no longer being used.

u32 MonsterComponent::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString MonsterComponent::m_typeName = "Monster";

void MonsterComponent::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_isPissed = false;
	m_lastPissed = 0.0f;

	//GVector3 pos = GetRandomPosition( 640, 480 );
	//i_actor->m_position = pos;

	//GPhysicsComponent* pComp = GetComponent<GPhysicsComponent>(i_actor);
	//GRigidBody* pObj = pComp->GetRigidBody();


	GActorHandle player = g_World::Get().GetActorByName( "Player" );

	/*
	m_MaxSpeed = attributes["MaxSpeed"];


	//Don't move if no player..
	GVector3 newDir = GVector3::Zero;
	if( player )
	{
		newDir = player->m_position - i_actor->m_position;
		newDir.Normalize();
		newDir *= m_MaxSpeed;
		pObj->m_velocity = newDir;
	}
	else
	{
		newDir = GetRandomPosition( (float)Gyro::UserSettings::GetScreenWidth(), (float)Gyro::UserSettings::GetScreenHeight() ) - i_actor->m_position;
		newDir.Normalize();
		//newDir *= 200.0f;
		//pObj->m_velocity = newDir;
	}
	*/

	m_collisionHandler = new MonsterCollisionHandler();
	m_collisionHandler->m_messageOwner = this;

	// register to hear for a player 
	g_MessageManager::Get().RegisterHandler( "MonsterCollidedWithPlayer", m_collisionHandler );
}

MonsterComponent::~MonsterComponent()
{

	if( m_collisionHandler )
	{
		g_MessageManager::Get().RemoveHandler( "MonsterCollidedWithPlayer" , m_collisionHandler );
		delete m_collisionHandler;
	}
}

IActorComponent* MonsterComponent::Create()
{

	// this is why I need to replace this with a creation function pointer...
	m_collisionHandler = NULL;
	return new MonsterComponent();
}

void MonsterComponent::Update( GActorHandle i_actor )
{

	//Pissed
	double curSeconds = g_Clock::Get().SecondsSinceStart();
	if( curSeconds - m_lastPissed > 2.0f )
	{
		m_lastPissed = (float)curSeconds;
		m_isPissed = !m_isPissed;

		/*
		GRenderComponent* rComp = GetComponent<GRenderComponent>( i_actor );

		if( rComp && rComp->GetRendererObject() )
		{
			if( m_isPissed )
			{
				
				//delete rComp->GetRendererObject()->m_sprite;
				//rComp->GetRendererObject()->m_sprite = Cheesy::Sprite::Create( Cheesy::Point2D( 0.0f, 0.0f ), 0.0f, Cheesy::Point2D( 40, 40 ), color );
			}
			else
			{

				//delete rComp->GetRendererObject()->m_sprite;
				//rComp->GetRendererObject()->m_sprite = Cheesy::Sprite::Create( Cheesy::Point2D( 0.0f, 0.0f ), 0.0f, Cheesy::Point2D( 40, 40 ), color );
			}
		}
		*/
	}

	//check boundaries.  if outside, place a new point and charge the player.
	if( !WithinBounds( (float)Gyro::UserSettings::GetScreenWidth(), (float)Gyro::UserSettings::GetScreenHeight(), i_actor))
	{
		GVector3 pos = GetRandomPosition( (float)Gyro::UserSettings::GetScreenWidth(), (float)Gyro::UserSettings::GetScreenHeight() );
		//i_actor->m_position = pos;

		GActorHandle player = g_World::Get().GetActorByName( "Player" );

		//Don't move if no player..
		GVector3 newDir = GVector3::Zero;
		if( player )
		{

			/*
			//GPhysicsComponent* pComp = GetComponent<GPhysicsComponent>( i_actor );
			newDir = player->m_position - i_actor->m_position;
			newDir.Normalize();
			//newDir *= 200.0f;

			if( pComp && pComp->GetRigidBody() )
			{
				//pComp->GetRigidBody()->m_velocity = newDir;
				//pComp->GetRigidBody()->m_lastPosition = pos;
			}
			*/
		}

	}

	//because I was asked to do something involving this.  Spins once in one second.
	if( m_isPissed )
	{
		
		float heading = (float)curSeconds * (1.0f*GMath::Pi ); 
		GMatrix4 rots;
		rots.SetRotZ( heading );

		//i_actor->m_rotation = rots;
		
	}

}

void MonsterComponent::EndUpdate( GActorHandle i_actor)
{

}

GVector3 MonsterComponent::GetRandomPosition( float width, float height ) const
{
	int fixedHeight = (int)height/2;
	int fixedWidth = (int)width/2;
	int sign = rand() % 2;
	int side = rand() % 4;
	GVector3 loc;
	float x, y;
	switch (side)
	{
	case 0:
		x = fixedWidth - 20.0f;
		y = (float) (rand() % (int)fixedHeight);
		if( sign )
			y = -y;
		break;
	case 1:
		x = -fixedWidth + 20.0f;
		y = (float)(rand() % (int)fixedHeight);
		if( sign )
			y = -y;
		break;
	case 2:
		x = (float)(rand() % (int)fixedWidth);
		y = fixedHeight - 20.0f;
		if( sign )
			x = -x;
		break;
	case 3:
		x = (float)(rand() % (int)fixedWidth);
		y = -fixedHeight + 20.0f;
		if( sign )
			x = -x;
		break;
	}
	loc.x( x );
	loc.y( y );
	loc.z( 0.0f );

	return loc;

}

void MonsterComponent::MonsterCollisionHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	// i_data is a collision contact...
	GContact* contact = (GContact*) i_data;

	assert( contact->m_first->m_owner ); // pointer is null
	assert( contact->m_second->m_owner );

	MonsterComponent* comp = (MonsterComponent*) m_messageOwner;
	if( comp->IsPissed() )
	{
		GActor* first = GActor::FromHandle( contact->m_first->m_owner );
		GActor* second = GActor::FromHandle( contact->m_second->m_owner );
		if( !first || !second )
			return;

		if( first->IsA( "Player" ) )
			first->Kill( );
		else if( second->IsA( "Player" ) )
			second->Kill( );
	}
}

bool MonsterComponent::WithinBounds( float width, float height, GActorHandle i_actor ) const
{
	int fixedWidth = (int)width/2;
	int fixedHeight = (int)height/2;

	GActor* actor = GActor::FromHandle( i_actor );
	if( !actor )
		return false;

	if( actor->m_position.x() < fixedWidth && actor->m_position.x() > -fixedWidth )
		if( actor->m_position.y() < fixedHeight && actor->m_position.y() > -fixedHeight )
			return true;

	return false;
}