#ifndef __GROUNDFOLLOWER_H_
#define __GROUNDFOLLOWER_H_

#include "Utility/GSharedPtr.h"
#include "Utility/GHashedString.h"
#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Audio/GAudioManager.h"

/*
GroundFollower Component

The idea for ground following is to be able to move across geometry without going through the physics system.
This is great for avatars and characters, whose movement while moving across the world should not behave like a physics object.

*/

class GGroundFollowerComp : public IActorComponent
{

public:
	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	GVector3							m_rayOrigin; // only doing this because this may change to support crouching or something.
	GVector3							m_finalPosition;
	GVector3							m_gravity;
	GVector3							m_velocity;
	GVector3							m_acceleration;
	GVector3							m_movingDir;

	float								m_downLength;
	float								m_forwardLength;
	float								m_jumpEscape;
	float								m_accumulatedJump;

	// hacks for stepping audio.  remove later.
	FMOD::Channel*						m_solidGround;


	// TO REMOVE
	float								m_maxSpeedXZ;
	float								m_maxSpeedY;
	float								m_accelerationXZ;
	bool								m_onGround;

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual IActorComponent*		Create( );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	virtual bool						HasUpdate() const { return true; };
	virtual bool						HasEndUpdate() const { return true; };

	void								ApplyAccelerationVectorXZ( float x, float z );
	void								ApplyAccelerationY( float i_y );
	void								ApplyAcceleration( const GVector3& i_vector );
	void								ApplyJumpAccelerationY( float i_y );

	void								SetVelocity( const GVector3& i_vector );

protected:


};

#endif