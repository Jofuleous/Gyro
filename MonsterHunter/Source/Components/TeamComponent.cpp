#include "TeamComponent.h"
#include "Physics/GPhysics.h"
#include "Utility/Clock.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Misc/UserSettings.h"
#include "Collision/CollisionManager.h"

#include <stdlib.h> 

// hack, but fulfills assignment requirements.  teams should be a bigger system.

u32 TeamComponent::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString TeamComponent::m_typeName = "Team";

void TeamComponent::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_teamNumber = i_obj["Number"].GetInteger();
	m_carryingFlag = false;
}

TeamComponent::~TeamComponent()
{
}

void TeamComponent::SetFlag( bool i_bool )
{
	m_carryingFlag = i_bool;
}

void TeamComponent::RemoveFlag( )
{
	m_carryingFlag = false;
}

IActorComponent* TeamComponent::Create()
{
	return new TeamComponent();
}


// when two players collide, if they're holding flags, they are returned.
void TeamComponent::TeamCollisionHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	/*
	// i_data is a collision contact...
	GContact* contact = (GContact*) i_data;

	assert( contact->m_first->m_owner ); // pointer is null
	assert( contact->m_second->m_owner );

	TeamComponent* comp = (TeamComponent*) m_messageOwner;
	if( contact->m_first->m_owner->IsA( "Player" ) && contact->m_second->m_owner->IsA( "Player" ) )
	{
		TeamComponent* firstTeam = (TeamComponent*) GetComponent<TeamComponent>( contact->m_first->m_owner );
		TeamComponent* secondTeam = (TeamComponent*) GetComponent<TeamComponent>( contact->m_first->m_owner );
		if( firstTeam && secondTeam && (firstTeam->m_teamNumber != secondTeam->m_teamNumber) )
		{
			if( firstTeam->m_carryingFlag )
			{
				firstTeam->RemoveFlag();
				firstTeam->
				g_MessageManager::Get().ProcessMessage( "FlagReturned", (void*) secondTeam->m_teamNumber );
			}
			if( secondTeam->m_carryingFlag )
			{
				secondTeam->RemoveFlag();
				g_MessageManager::Get().ProcessMessage( "FlagReturned", (void*) firstTeam->m_teamNumber );
			}
		}
	}
	*/
}