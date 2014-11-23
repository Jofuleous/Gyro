#include "FlagComponent.h"
#include "Physics/GPhysics.h"
#include "Utility/Clock.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Misc/UserSettings.h"
#include "Collision/CollisionManager.h"
#include "Actor/GActor.h"
#include "TeamComponent.h"
#include "Audio/GAudioManager.h"

#include <stdlib.h> 

// no longer being used.

u32 FlagComponent::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString FlagComponent::m_typeName = "Flag";

void FlagComponent::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_teamNumber = i_obj["Number"].GetInteger();

	m_collisionHandler = new FlagCollisionHandler();
	m_collisionHandler->m_messageOwner = this;

	m_returnHandler = new FlagReturnHandler();
	m_returnHandler->m_messageOwner = this;

	//m_captor is null initially.
	m_captor = GActorHandle();
	GActor* actor = GActor::FromHandle( i_actor );
	
	//store off the init location, because this is where we will return the flag.
	m_startLocation = actor->m_position;
	m_stopImmune = 0.0f;

	// register to hear for a player 
	g_MessageManager::Get().RegisterHandler( "FlagCollidedWithPlayer", m_collisionHandler );
	g_MessageManager::Get().RegisterHandler( "FlagReturned", m_returnHandler );
	m_immune = false;
}

FlagComponent::~FlagComponent()
{
	// hopefully these get deleted...
	/*
	if( m_collisionHandler )
	{
		g_MessageManager::Get().RemoveHandler( "FlagCollidedWithPlayer" , m_collisionHandler );
		delete m_collisionHandler;
	}
	if( m_returnHandler )
	{
		g_MessageManager::Get().RemoveHandler( "ReturnFlag" , m_returnHandler );
		delete m_returnHandler;
	}
	*/
}

IActorComponent* FlagComponent::Create()
{

	// this is why I need to replace this with a creation function pointer...
	m_collisionHandler = NULL;
	return new FlagComponent();
}

void FlagComponent::Update( GActorHandle i_actor )
{

}

void FlagComponent::EndUpdate( GActorHandle i_actor)
{
	GActor* captor = GActor::FromHandle( m_captor );
	GActor* actor = GActor::FromHandle( i_actor );

	if( captor )
	{
		GVector3 onHead = captor->m_position;
		onHead._y += 150.0f;
		actor->m_position = onHead;
	}
	else
	{
		actor->m_position = m_startLocation;
		if( m_immune )
		{
			if( g_Clock::Get().SecondsSinceStart() > m_stopImmune )
				m_immune = false;
		}
	}
}

// when a player collides with the flag, if the flag is not their flag, mount it to the player.
void FlagComponent::FlagCollisionHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	// i_data is a collision contact...
	GContact* contact = (GContact*) i_data;

	assert( contact->m_first->m_owner ); // pointer is null
	assert( contact->m_second->m_owner );

	FlagComponent* comp = (FlagComponent*) m_messageOwner;
	if( comp->m_immune )
		return;

	GActorHandle captor( 0, 0, 0 );
	GActor* first = GActor::FromHandle( contact->m_first->m_owner );
	assert( first );
	GActor* second = GActor::FromHandle( contact->m_second->m_owner );
	assert( second );
	if( first->IsA( "PlayerOne" ) || first->IsA( "CtfAiPlayer" )) // hack
	{
		captor = contact->m_first->m_owner;
		if( GetComponent<FlagComponent>(contact->m_second->m_owner ) != comp )
			return;
	}
	else if( second->IsA( "PlayerOne" ) || second->IsA( "CtfAiPlayer" ) )
	{
		captor = contact->m_second->m_owner;
		if( GetComponent<FlagComponent>(contact->m_first->m_owner ) != comp )
			return;
	}

	if( captor )
	{
		TeamComponent* tComp = (TeamComponent*) GetComponent<TeamComponent>( captor );
		if( tComp->m_teamNumber != comp->m_teamNumber && !comp->m_captor )
		{
			g_AudioManager.PlayEffect( "data/Audio/SFX/airhorn.mp3", false, false );
			g_MessageManager::Get().ProcessMessage("FlagTaken", (void*) comp->m_teamNumber );
			tComp->SetFlag( true );
			comp->m_captor = captor;
		}
		else if( tComp->m_teamNumber == comp->m_teamNumber && comp->m_captor )
		{
			g_AudioManager.PlayEffect( "data/Audio/Dialog/flag_returned.mp3", false, false );
			g_MessageManager::Get().ProcessMessage("FlagReturned", (void*) comp->m_teamNumber );
			comp->ResetFlag();
			tComp->SetFlag( false );
		}
	}
}

void FlagComponent::FlagReturnHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	FlagComponent* flag = (FlagComponent*) m_messageOwner;
	if( (int) i_data  == flag->m_teamNumber )
	{
		flag->m_captor = GActorHandle( 0, 0, 0 );
		flag->ResetFlag();
	}
}

void FlagComponent::ResetFlag()
{
	m_immune = true;
	m_stopImmune = g_Clock::Get().SecondsSinceStart() + 1.0f;
	m_captor = GActorHandle( 0, 0, 0 );
}