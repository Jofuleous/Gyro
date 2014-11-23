#include "CtfAiPlayerComp.h"
#include "Physics/GPhysics.h"
#include "Utility/Clock.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Misc/UserSettings.h"
#include "Collision/CollisionManager.h"
#include "Actor/GActor.h"
#include "TeamComponent.h"
#include "Navigation/GWayPointManager.h"
#include "Components/GGroundFollowerComp.h"
#include "FlagComponent.h"
#include "../CaptureFlagManager.h"

#include <stdlib.h> 

// no longer being used.

static const float MAX_STAMINA;

u32 CtfAiPlayerComp::m_typeId =ComponentManager::GetNextTypeId();

const GHashedString CtfAiPlayerComp::m_typeName = "CtfAiPlayer";

static float GCtfHeuristic( GWayNode* i_current, GWayNode* i_next, void* i_data )
{
	CtfAiPlayerComp* comp = (CtfAiPlayerComp*) i_data;
	GVector3 toEnd = i_next->m_Position - i_current->m_Position;
	float toTargetLen = toEnd.LengthSquared();
	GActor* player = GActor::FromHandle( comp->m_Player );
	if( !player )
		return 1.0f;

	GVector3 toPlayer = player->m_position - i_current->m_Position;
	float toPlayerLen = toPlayer.LengthSquared();

	if( toPlayerLen < (250.0f * 250.0f) * 2.0f )
		return FLT_MAX / 5.0f; // this node is BASICALLY off limits...

	return toTargetLen;

	//return toTargetLen;
}

void CtfAiPlayerComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_teamNumber = 2;
	GActor* actor = GActor::FromHandle( i_actor );
	
	//store off the init location, because this is where we will return the flag.
	m_startLocation = actor->m_position;

	m_State = SEEK;
	m_SubState = SEEK_ENTER;

	m_staminaMeter = 0.0f;
	m_usingStamina = false;

	// register for messages.
	m_collisionHandler.m_messageOwner = this;
	m_flagTakenHandler.m_messageOwner = this;
	m_returnHandler.m_messageOwner = this;

	g_MessageManager::Get().RegisterHandler( "FlagReturned", &m_returnHandler );
	g_MessageManager::Get().RegisterHandler( "FlagTaken", &m_flagTakenHandler );
	g_MessageManager::Get().RegisterHandler( "AiCollidedWithPlayer", &m_collisionHandler );
	m_pantingChannel = g_AudioManager.PlayEffect( "data/Audio/Dialog/heavy-breathing-off-mic-loop.mp3", true, true );
	m_walkOnGroundSlow = g_AudioManager.PlayEffect( "data/Audio/SFX/walkingonground.mp3", true, true );
	m_pantingChannel->setVolume( 1.0f );
	m_pantingChannel->set3DMinMaxDistance( 500.0f, 1000.0f );
	m_walkOnGroundSlow->set3DMinMaxDistance( 500.0f, 1000.0f );

}

CtfAiPlayerComp::CollisionHandler::~CollisionHandler()
{
	g_MessageManager::Get().RemoveHandler( "AiCollidedWithPlayer", this );
}

CtfAiPlayerComp::FlagReturnedHandler::~FlagReturnedHandler()
{
	g_MessageManager::Get().RemoveHandler( "FlagReturned", this );
}

CtfAiPlayerComp::FlagTakenHandler::~FlagTakenHandler()
{
	g_MessageManager::Get().RemoveHandler( "FlagTaken", this );
}

IActorComponent* CtfAiPlayerComp::Create()
{
	return new CtfAiPlayerComp();
}

void CtfAiPlayerComp::StaminaBrain( GActorHandle i_actor )
{
	GActor* actor = GActor::FromHandle( i_actor );
	if( m_usingStamina )
	{
		g_AudioManager.UpdateChannelPosition( m_pantingChannel, actor->m_position );
		m_pantingChannel->setVolume( 1.0f );
	}

	if( m_staminaMeter > 0.0f && m_usingStamina )
		return;
	else if( m_staminaMeter <= 0.0f )
	{
		m_usingStamina = false;
		m_pantingChannel->setVolume( 0.0f );
		m_lastStamina = g_Clock::Get().SecondsSinceStart();
	}
	else if( g_Clock::Get().SecondsSinceStart() - m_lastStamina > 4.0f )
	{
		if(	m_State == PURSUE )
		{
			if( m_PathToTarget.m_PathNodes.size() > 0 )
			{
				u16 target = m_PathToTarget.m_PathNodes[m_PathToTarget.m_PathNodes.size() - 1];
				GVector3 targetPos;
				g_WayPointManager.NodePosition( target, targetPos );

				if( (targetPos - actor->m_position).LengthSquared() < 4000000 )
				{
					m_usingStamina = true;
				}
			}
		}
		else if( m_State == CAPTURE )
		{
			m_usingStamina = true;
			m_lastStamina = g_Clock::Get().SecondsSinceStart();
		}
		else if( m_State == SEEK )
		{
			m_usingStamina = true;
			m_lastStamina = g_Clock::Get().SecondsSinceStart();
		}
	}
}

void CtfAiPlayerComp::HandlePathFound( GActorHandle i_actor )
{
	GActor* actor = GActor::FromHandle( i_actor );
	m_LastNodePos = actor->m_position;

	// the closest node could be behind us.  make sure that's not the case. this could be breaking some rules...
	u16 selected = m_PathToTarget.m_PathNodes[0];
	if( m_PathToTarget.m_PathNodes.size() > 1 )
	{
		u16 second = m_PathToTarget.m_PathNodes[1];
		GVector3 firstNode, secondNode;
		g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[0], firstNode);
		g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[1], secondNode );
		GVector3 nodeDir = secondNode - firstNode;
		GVector3 toFirst = firstNode - m_LastNodePos;
		if( nodeDir.Dot( toFirst ) < 0.0f )
		{
			m_PathIndex = 1;
			selected = m_PathToTarget.m_PathNodes[1];
			m_TargetNodePos = secondNode;
			if( m_PathToTarget.m_PathNodes.size() > 2 )
				g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[2], m_NextTargetNodePos );
			else
				m_NextTargetNodePos = secondNode;
		}
		else
		{
			m_PathIndex = 0;
			m_TargetNodePos = firstNode;
			m_NextTargetNodePos = secondNode;
			//m_CurrentPathNode = selected;
		}

	}
	else
	{
		m_PathIndex = 0;
		//m_CurrentPathNode = selected;
		g_WayPointManager.NodePosition( selected, m_TargetNodePos );
		if( m_PathToTarget.m_PathNodes.size() > 1 )
			g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[1], m_NextTargetNodePos );
		else
			m_NextTargetNodePos = m_TargetNodePos;
	}

	m_CurrentPathNode = selected;
}

// SEEK_ENTER
CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Seek_Enter( GActorHandle i_actor )
{
	m_PathToTarget.m_pathFound = false;
	m_PathToTarget.m_PathNodes.clear();
	GActor* actor = GActor::FromHandle( i_actor );

	// get the flag we want.
	GActorHandle flag = g_World::Get().GetActorByName( "TeamOneFlag" );
	if( !flag )
		return SEEK_ENTER;

	GActor* flagActor = GActor::FromHandle( flag );

	g_WayPointManager.QueuePathFind( &m_PathToTarget, actor->m_position, flagActor->m_position, GDefaultHeuristic, NULL );

	return SEEK_PATH_WAIT;
}


// SEEK_PATH_WAIT
CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Seek_Path_Wait( GActorHandle i_actor )
{
	if( !m_PathToTarget.m_pathFound )
		return SEEK_PATH_WAIT;

	HandlePathFound( i_actor );

	return SEEK_SEEKING;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Seek_Seeking( GActorHandle i_actor  )
{
	GActor* actor = GActor::FromHandle( i_actor );
	
	if( g_CTFManager::Get().m_teamTwoFlagTaken )
	{
		HandleNewState( PURSUE );
		return PURSUE_ENTER;
	}

	// for now, just go to the target.
	if( (actor->m_position - m_TargetNodePos).LengthXZSquared() < 30.0f )
	{
		if( m_PathIndex + 1 < m_PathToTarget.m_PathNodes.size() )
		{
			m_PathIndex++;
			m_LastNodePos = m_TargetNodePos;
			g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[m_PathIndex], m_TargetNodePos );
			if( m_PathIndex + 1 < m_PathToTarget.m_PathNodes.size() )
				g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[m_PathIndex+1], m_NextTargetNodePos );
			else
				m_NextTargetNodePos = m_TargetNodePos;

		}
		else
		{
			// we have reached our destination.  start captured state...since we hypothetically have our flag.
			return SEEK_EXIT;
		}
	}

	// just make sure this works.
	return SEEK_SEEKING;
}

CtfAiPlayerComp::AiPlayerState CtfAiPlayerComp::Seek( GActorHandle i_actor )
{
	AiPlayerSubState substate = m_SubState;
	switch( m_SubState )
	{
		case SEEK_ENTER:
			substate = Seek_Enter( i_actor );
		break;
		case SEEK_PATH_WAIT:
			substate = Seek_Path_Wait( i_actor );
		break;
		case SEEK_SEEKING:
			substate = Seek_Seeking( i_actor );
			if( substate > 3 )
			{
				m_SubState = PURSUE_ENTER;
				return PURSUE;
			}
		break;
	}

	m_SubState = substate;

	// handle sub-state switch.
	return SEEK;
}

CtfAiPlayerComp::AiPlayerState CtfAiPlayerComp::Capture( GActorHandle i_actor )
{
	AiPlayerSubState substate;
	switch( m_SubState )
	{
		case CAPTURE_ENTER:
			substate = Capture_Enter( i_actor );
		break;
		case CAPTURE_PATH_WAIT:
			substate = Capture_Path_Wait( i_actor );
		break;
		case CAPTURE_CAPTURING:
			substate = Capture_Capturing( i_actor );
		break;
		case CAPTURE_EXIT:
			substate = CAPTURE_EXIT;
			m_State = SEEK;
			substate = SEEK_ENTER;
		break;
	}

	m_SubState = substate;

	return CAPTURE;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Capture_Enter( GActorHandle i_actor )
{
	m_PathToTarget.m_pathFound = false;
	m_PathToTarget.m_PathNodes.clear();
	GActor* actor = GActor::FromHandle( i_actor );
	assert( actor ); // this is us.

	// get the flag we want.
	GActorHandle flag = g_World::Get().GetActorByName( "TeamTwoFlag" );
	if( !flag )
		return CAPTURE_ENTER;

	FlagComponent* flagComp = GetComponent<FlagComponent>( flag );
	if( flag )
		g_WayPointManager.QueuePathFind( &m_PathToTarget, actor->m_position, flagComp->m_startLocation, GCtfHeuristic, this );
	m_nextOpponentQuery = g_Clock::Get().SecondsSinceStart() + 0.3f;

	return CAPTURE_PATH_WAIT;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Capture_Path_Wait( GActorHandle i_actor )
{

	if( !m_PathToTarget.m_pathFound )
		return CAPTURE_PATH_WAIT;

	HandlePathFound( i_actor );

	return CAPTURE_CAPTURING;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Capture_Capturing( GActorHandle i_actor )
{
	GActor* actor = GActor::FromHandle( i_actor );
	assert( i_actor );
	GActor* player = GActor::FromHandle( m_Player );
	if( !player )
		return CAPTURE_CAPTURING;

	if( m_nextOpponentQuery < g_Clock::Get().SecondsSinceStart() )
	{
		// if we're close to the player, try and get away?
		if( (player->m_position - actor->m_position).LengthSquared() < 500.0f * 500.0f )
		{
			return CAPTURE_ENTER;
		}
	}

	// for now, just go to the target.
	if( (actor->m_position - m_TargetNodePos).LengthXZSquared() < 20.0f )
	{
		if( m_PathIndex + 1 < m_PathToTarget.m_PathNodes.size() )
		{
			m_PathIndex++;
			m_LastNodePos = m_TargetNodePos;
			g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[m_PathIndex], m_TargetNodePos );
			if( m_PathIndex + 1 < m_PathToTarget.m_PathNodes.size() )
				g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[m_PathIndex+1], m_NextTargetNodePos );
			else
				m_NextTargetNodePos = m_TargetNodePos;

		}
		else
		{
			// we have reached our destination.  start captured state...since we hypothetically have our flag.
			return CAPTURE_EXIT;
		}
	}

	return CAPTURE_CAPTURING;
}

CtfAiPlayerComp::AiPlayerState CtfAiPlayerComp::Pursue( GActorHandle i_actor )
{
	AiPlayerSubState substate;
	if( !g_CTFManager::Get().m_teamTwoFlagTaken )
	{
		m_State = SEEK;
		m_SubState = SEEK_ENTER;
		return SEEK;
	}

	switch( m_SubState )
	{
		case PURSUE_ENTER:
			substate = Pursue_Enter( i_actor );
		break;
		case PURSUE_PATH_WAIT:
			substate = Pursue_Path_Wait( i_actor );
		break;
		case PURSUE_PURSUING:
			substate = Pursue_Pursuing( i_actor );
		break;
		case PURSUE_EXIT:
			m_State = SEEK;
			substate = SEEK_ENTER;
		break;
	}

	m_SubState = substate;

	return CAPTURE;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Pursue_Enter( GActorHandle i_actor )
{
	m_PathToTarget.m_pathFound = false;
	m_PathToTarget.m_PathNodes.clear();
	GActor* actor = GActor::FromHandle( i_actor ); // this is us.  should not be null.

	// get the flag we want.
	GActorHandle playerHandle = g_World::Get().GetActorByName( "PlayerOne" );
	if( !playerHandle )
		return PURSUE_EXIT;;

	GActor* player = GActor::FromHandle( playerHandle );

	FlagComponent* flagComp = GetComponent<FlagComponent>( playerHandle );
	if( player )
		g_WayPointManager.QueuePathFind( &m_PathToTarget, actor->m_position, player->m_position, GDefaultHeuristic, NULL );

	return PURSUE_PATH_WAIT;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Pursue_Path_Wait( GActorHandle i_actor )
{
	if( !m_PathToTarget.m_pathFound )
		return PURSUE_PATH_WAIT;

	HandlePathFound( i_actor );
	m_nextPursue = g_Clock::Get().SecondsSinceStart() + 0.5f;

	return PURSUE_PURSUING;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Pursue_Pursuing( GActorHandle i_actor )
{
	GActor* actor = GActor::FromHandle( i_actor );
	// we need to re-corect our path...
	if( g_Clock::Get().SecondsSinceStart() > m_nextPursue )
		return PURSUE_ENTER;


	// for now, just go to the target.
	if( (actor->m_position - m_TargetNodePos).LengthXZSquared() < 30.0f )
	{
		if( m_PathIndex + 1 < m_PathToTarget.m_PathNodes.size() )
		{
			m_PathIndex++;
			m_LastNodePos = m_TargetNodePos;
			g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[m_PathIndex], m_TargetNodePos );
			if( m_PathIndex + 1 < m_PathToTarget.m_PathNodes.size() )
				g_WayPointManager.NodePosition( m_PathToTarget.m_PathNodes[m_PathIndex+1], m_NextTargetNodePos );
			else
				m_NextTargetNodePos = m_TargetNodePos;

		}
		else
		{
			// we have reached our destination.  start captured state...since we hypothetically have our flag.
			return PURSUE_EXIT;
		}
	}

	return PURSUE_PURSUING;
}

CtfAiPlayerComp::AiPlayerSubState CtfAiPlayerComp::Pursue_Exiting( GActorHandle i_actor )
{
	return PURSUE_EXIT;
}

// alright.  this is the "state machine" :(
void CtfAiPlayerComp::Update( GActorHandle i_actor )
{
	GActor* actor = GActor::FromHandle( i_actor );

	// make sure the player is valid every frame.
	if( !m_Player )
	{
		m_Player = g_World::Get().GetActorByName( "PlayerOne" );
		if( !m_Player )
			return;
	}

	g_AudioManager.UpdateChannelPosition( m_walkOnGroundSlow, actor->m_position );

	StaminaBrain( i_actor );

	if( m_usingStamina )
		m_staminaMeter -= g_Clock::Get().SecondsSinceLastFrame() * 5.0f;
	else
		m_staminaMeter += g_Clock::Get().SecondsSinceLastFrame() * 1.66667f;

	GMath::Clamp( &m_staminaMeter, 0.0f, 10.0f );

	switch( m_State )
	{
		case SEEK:
			Seek( i_actor );
		break;
		case CAPTURE:
			Capture( i_actor );
		break;
		case PURSUE:
			Pursue( i_actor );
		break;
	}

	if( m_PathToTarget.m_pathFound )
	{
		GGroundFollowerComp* gFollower = GetComponent<GGroundFollowerComp>( i_actor );
		assert( gFollower ); // you fucked up.

		GVector3 velocity;

		if( m_PathToTarget.m_PathNodes.size() <= 1 && m_State == PURSUE )
		{
			GActorHandle playerHandle = g_World::Get().GetActorByName( "PlayerOne" );
			GActor* player = GActor::FromHandle( playerHandle );
			if( !player )
				velocity = GVector3( 0.0f, 0.0f, 0.0f ); 
			else
				velocity = player->m_position - actor->m_position;	
		}
		else
		{
			velocity = m_TargetNodePos - actor->m_position;
		}

		velocity.Normalize();
		if( m_usingStamina )
			velocity *= 1000.0f;
		else
			velocity *= 500.0f;

		m_LastVelocity = velocity;

		gFollower->m_velocity = velocity;
	}
}

void CtfAiPlayerComp::EndUpdate( GActorHandle i_actor)
{
}

void CtfAiPlayerComp::HandleNewState( AiPlayerState i_state )
{
	switch( m_State )
	{
		case SEEK:
		{
			switch( i_state )
			{
				case CAPTURE:
				{
					if( !m_PathToTarget.m_pathFound )
					{
						// remove path find.
						//g_WayPointManager.
					}
					m_State = CAPTURE;
					m_SubState = CAPTURE_ENTER;
				}
				break;
				case PURSUE:
				{
					m_State = PURSUE;
					m_SubState = PURSUE_ENTER;
				}
				break;
			}
		}
		break;
		case CAPTURE:
		{
			switch( i_state )
			{
				case SEEK:
				{
					m_State = SEEK;
					m_SubState = SEEK_ENTER;
				}
				break;
			}
		}
	}
}

void CtfAiPlayerComp::FlagTakenHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	int team = (int) i_data;
	CtfAiPlayerComp* comp = (CtfAiPlayerComp*) m_messageOwner;
	if( team != comp->m_teamNumber )
		comp->HandleNewState( CAPTURE );
}

void CtfAiPlayerComp::FlagReturnedHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	int team = (int) i_data;
	CtfAiPlayerComp* comp = (CtfAiPlayerComp*) m_messageOwner;
	if( team != comp->m_teamNumber )
		comp->HandleNewState( SEEK );
}

// when a player collides with the flag, if the flag is not their flag, mount it to the player.
void CtfAiPlayerComp::CollisionHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	// i_data is a collision contact...
	GContact* contact = (GContact*) i_data;

	assert( contact->m_first->m_owner ); // pointer is null
	assert( contact->m_second->m_owner );

	CtfAiPlayerComp* comp = (CtfAiPlayerComp*) m_messageOwner;
	/*
	GActorHandle captor;
	if( contact->m_first->m_owner->IsA( "PlayerOne" ) || contact->m_first->m_owner->IsA( "PlayerTwo" )) // hack
		captor = contact->m_first->m_owner;
	else if( contact->m_second->m_owner->IsA( "PlayerOne" ) || contact->m_first->m_owner->IsA( "PlayerTwo" ) )
		captor = contact->m_second->m_owner;

	if( captor )
	{
		TeamComponent* tComp = (TeamComponent*) GetComponent<TeamComponent>( captor );
		if( tComp->m_teamNumber != comp->m_teamNumber )
		{
			g_MessageManager::Get().ProcessMessage("FlagTaken", (void*) comp->m_teamNumber );
			tComp->SetFlag( true );
			comp->m_captor = captor;
		}
	}
	*/
}