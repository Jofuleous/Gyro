#ifndef __CTF_PLAYERCOMP_H_
#define __CTF_PLAYERCOMP_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Core/GMessageManager.h"
#include "Navigation/GWayPointManager.h"
#include "Audio/GAudioManager.h"

class CtfAiPlayerComp : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	CtfAiPlayerComp( )
	{
	}

	enum AiPlayerState
	{
		CAPTURE,  // decide between returning flag 
		SEEK,
		PURSUE,
	};

	enum AiPlayerSubState
	{
		SEEK_ENTER = 0, // need a new path
		SEEK_PATH_WAIT, // waiting for path
		SEEK_SEEKING,
		SEEK_EXIT,
		CAPTURE_ENTER,
		CAPTURE_PATH_WAIT,
		CAPTURE_CAPTURING,
		CAPTURE_EXIT,
		PURSUE_ENTER,
		PURSUE_PATH_WAIT,
		PURSUE_PURSUING,
		PURSUE_EXIT,
	};

	virtual								~CtfAiPlayerComp( void ) {}

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }
	void								StaminaBrain( GActorHandle i_actor );

	//AI
	void					HandleNewState( AiPlayerState i_state );
	void					HandlePathFound( GActorHandle i_actor );
	// SEEK
	AiPlayerState			Seek( GActorHandle i_actor );
	AiPlayerSubState		Seek_Enter( GActorHandle i_actor );
	AiPlayerSubState		Seek_Path_Wait( GActorHandle i_actor );
	AiPlayerSubState		Seek_Seeking( GActorHandle i_actor );

	// CAPTURING
	AiPlayerState			Capture( GActorHandle actor );
	AiPlayerSubState		Capture_Enter( GActorHandle actor );
	AiPlayerSubState		Capture_Path_Wait( GActorHandle actor );
	AiPlayerSubState		Capture_Capturing( GActorHandle actor ); 

	// PURSUE
	AiPlayerState			Pursue( GActorHandle actor );
	AiPlayerSubState		Pursue_Enter( GActorHandle actor );
	AiPlayerSubState		Pursue_Path_Wait( GActorHandle actor );
	AiPlayerSubState		Pursue_Pursuing( GActorHandle actor ); 
	AiPlayerSubState		Pursue_Exiting( GActorHandle actor ); 

	class CollisionHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~CollisionHandler();
	};


	class FlagTakenHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~FlagTakenHandler();
	};

	class FlagReturnedHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~FlagReturnedHandler();
	};

	// message handlers
	CollisionHandler				m_collisionHandler;
	FlagTakenHandler				m_flagTakenHandler;
	FlagReturnedHandler				m_returnHandler;
	AiPlayerState					m_State;
	AiPlayerSubState				m_SubState;

	GActorHandle	m_Player;
	GWayPath						m_PathToTarget;
	u16								m_CurrentPathNode;
	u16								m_PathIndex;
	GVector3						m_NextTargetNodePos;
	GVector3						m_TargetNodePos;
	GVector3						m_LastNodePos;	// the position we were at when transitioning nodes.  could be path start pos.
	GVector3						m_LastVelocity;

	//stam
	float			m_staminaMeter;
	bool			m_usingStamina;
	float			m_lastStamina;
	FMOD::Channel*	m_pantingChannel;
	FMOD::Channel*	m_walkOnGroundSlow;

	float		m_nextOpponentQuery;
	GVector3						m_startLocation;
	float							m_nextPursue;
	int								m_teamNumber;

protected:

};

#endif