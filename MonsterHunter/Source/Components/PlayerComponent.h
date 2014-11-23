#ifndef __PLAYERCOMPONENT_H_
#define __PLAYERCOMPONENT_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Utility/GSharedPtr.h"
#include "Math/GVector3.h"
#include "Misc/GMouse.h"
#include "Misc/RBuffer.h"
#include "Core/GMessageManager.h"
#include "Navigation/GWayPointManager.h"
#include "Audio/GAudioManager.h"

// I'll need to see how camera's are *supposed* to be done later...
//#include "../../../Renderer/Source/

class PlayerComponent : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	struct _keyboardInput
	{
		unsigned int					m_CharID;
		bool							m_keyDown;
	};

	virtual								~PlayerComponent( void );

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	void								HandleInput( GActorHandle actor, unsigned int i_CharID, bool i_keyDown );
	void								HandleInputUpdate( GActorHandle actor );
	void								HandleMouseButtonInput( u32 i_message );
	void								DrawFoundPath();

	void								ReturnNormalSpeed( void )
										{ 
											m_maxSpeed = m_originalMaxSpeed;
											m_acceleration = m_originalAcceleration;
										}
	void								SetSpeedMultiplier( float i_multiplier )
										{
											m_maxSpeed *= i_multiplier;
											m_acceleration *= i_multiplier;
										}

	GMouse						m_mouse;
	std::vector<_keyboardInput>	m_inputs;
	bool						m_inputPending;
	static bool					m_freeCam;
	float						m_maxSpeed; // vector magnitude...
	float						m_originalMaxSpeed;
	float						m_originalAcceleration;
	float						m_acceleration;
	float						m_decelerate;
	char						m_fpsBuffer[32];
	char						m_levelBuffer[32];
	float						m_lastPosted;
	float						m_camTargetOffsetY;
	float						m_cameraPosY;
	float						m_cameraDistance;
	float						m_sprintValue;
	float						m_staminaMeter;
	bool						m_usingStamina;
	GWayPath					m_pathToTarget;
	bool						m_drawPath;
	FMOD::Sound*				m_walkOnGroundSound;
	FMOD::Channel*				m_walkOnGroundSlow;
	FMOD::Channel*				m_walkOnMetalSlow;
	FMOD::Channel*				m_pant;

	GVector3				m_inputVector;
	GVector3				m_lastInputVector;
	GVector3				m_lookingDirection;


};

#endif