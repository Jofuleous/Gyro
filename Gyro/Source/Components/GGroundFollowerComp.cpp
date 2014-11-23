#include "GGroundFollowerComp.h"
#include "Actor/ComponentManager.h"
#include "Collision/CollisionManager.h"
#include "Utility/Clock.h"
#include "Profiling/GProfiling.h"

const GHashedString GGroundFollowerComp::m_typeName = "GroundFollower";
u32 GGroundFollowerComp::m_typeId =ComponentManager::GetNextTypeId();


GVector3	DEFAULT_FORWARD( 0.0f, 0.0f, 1.0f );
GVector3	DEFAULT_DOWN( 0.0f, -1.0f, 1.0f );

void GGroundFollowerComp::Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj )
{
	LuaPlus::LuaObject& temp = i_obj["RayOrigin"];
	m_rayOrigin._x = temp[1].GetFloat();
	m_rayOrigin._y = temp[2].GetFloat();
	m_rayOrigin._z = temp[3].GetFloat();

	m_downLength = i_obj["RayDownLength"].GetFloat();
	m_forwardLength = i_obj["RayForwardLength"].GetFloat();

	// let's get these out of the physics system...
	m_maxSpeedXZ = i_obj["MaxSpeedXZ"].GetFloat();
	m_maxSpeedY = i_obj["MaxSpeedY"].GetFloat();
	m_accelerationXZ = i_obj["AccelerationXZ"].GetFloat();
	m_gravity._x = i_obj["Gravity"][1].GetFloat();
	m_gravity._y = i_obj["Gravity"][2].GetFloat();
	m_gravity._z = i_obj["Gravity"][3].GetFloat();

	// setup initial values
	m_movingDir = DEFAULT_FORWARD;
	m_onGround = false;
	m_velocity = GVector3::Zero;
	m_acceleration = GVector3::Zero;

	return;
}

// eventually, I want to handle this like KinematicStateData...
void GGroundFollowerComp::Update( GActorHandle i_actor )
{

	PROFILE_SCOPE_BEGIN( "Ground Follower - Update" )
	
	GActor* actor = GActor::FromHandle( i_actor );
	float dt = (float)g_Clock::Get().SecondsSinceLastFrame();

	// update the velocity...
	m_acceleration += (m_gravity * dt );
	
	GVector3 finalVelocity = m_velocity + ( m_acceleration * dt );
	// this could be higher than max speed.  for instance, when exiting a max speed volume.

	float speed = finalVelocity.LengthXZ();

	if( speed > m_maxSpeedXZ )
	{
		finalVelocity.x( (finalVelocity.x() / speed) * m_maxSpeedXZ );
		finalVelocity.z( (finalVelocity.z() / speed) * m_maxSpeedXZ );

	}

	GVector3 xzThisFrame( finalVelocity.x() * dt, 0.0f, finalVelocity.z() * dt );

	if( xzThisFrame.LengthSquared() > 0.0f )
	{
		m_movingDir = xzThisFrame;
		m_movingDir.Normalize();
	}
	if( finalVelocity.y() > m_maxSpeedY )
		finalVelocity.y( m_maxSpeedY );

	m_velocity = finalVelocity;

	// don't worry about other collisions for now...

	// possibly don't do this if velocity is 0...

	PROFILE_SCOPE_BEGIN( "Ground Follower - Forward" )
	GRayQuery queryForward( actor->m_position + m_rayOrigin, m_movingDir, g_Collision::Get().GetCollisionMask( "TERRAIN" ), m_forwardLength + xzThisFrame.Length() );
	if( !g_Collision::Get().RayQuery( queryForward ) )
	{
		m_finalPosition.x( actor->m_position.x() + xzThisFrame.x() );
		m_finalPosition.z( actor->m_position.z() + xzThisFrame.z() );
	}
	else
	{
		m_velocity.x( 0.0f );
		m_velocity.z( 0.0f );
		m_acceleration.x( 0.0f );
		m_acceleration.z( 0.0f );
		m_finalPosition.x( actor->m_position.x() );
		m_finalPosition.z( actor->m_position.z() );
	}

	PROFILE_SCOPE_END()


	PROFILE_SCOPE_BEGIN( "Ground Follower - Down" )
	// this seems to be working 100%, but I can totally see it messing up eventually.  This should potentially be like a continuous line query...
	GRayQuery query( m_finalPosition + m_rayOrigin, GVector3::Down, g_Collision::Get().GetCollisionMask( "TERRAIN" ), m_downLength ); // assumes we can never move faster than down length...

	if( g_Collision::Get().RayQuery( query ) )
	{
		//do the thing!  for now, just put the actor at the height of the ground...don't bother with being clever...too tired for that.
		if( m_jumpEscape <= 0.0f )
		{
			m_finalPosition.y( (actor->m_position.y() + m_rayOrigin.y()) + (GVector3::Down.y() * query.m_hitDistance) );

			m_velocity.y( 0.0f );
			m_acceleration.y( 0.0f );
			//m_accumulatedJump = 0.0f;
			m_onGround = true;
		}
		else
		{
			//if( m_velocity.y() > 0.0f )
			//	m_accumulatedJump += (m_velocity.y() * dt);
			m_finalPosition.y( (m_finalPosition.y() + m_rayOrigin.y()) + (GVector3::Down.y() * (query.m_hitDistance )) + 5.0f /*+ m_accumulatedJump*/ );
			m_onGround = false;
		}
	}
	else
	{
		m_onGround = false;
		m_finalPosition.y( actor->m_position.y() + (finalVelocity.y() * dt ) );
	}

	PROFILE_SCOPE_END()

	GMath::MoveToValue( m_jumpEscape, 0.0f, dt );

	PROFILE_SCOPE_END()
}

void GGroundFollowerComp::ApplyJumpAccelerationY( float i_accelY )
{
	if( m_onGround )
		m_acceleration.y( i_accelY );

	m_jumpEscape = 0.001f;
}

void GGroundFollowerComp::ApplyAccelerationVectorXZ( float x, float z )
{
	m_acceleration.x( x * m_accelerationXZ );
	m_acceleration.z( z * m_accelerationXZ );
}

void GGroundFollowerComp::ApplyAcceleration( const GVector3& i_direction )
{
	m_acceleration = i_direction;
}

void GGroundFollowerComp::ApplyAccelerationY( float i_y )
{
	m_acceleration.y( i_y );
}

void GGroundFollowerComp::SetVelocity( const GVector3& i_velocity )
{
	m_velocity = i_velocity;
}

void GGroundFollowerComp::EndUpdate( GActorHandle i_actor )
{
	// override the physics for now.
	GActor* actor = GActor::FromHandle( i_actor );
	assert( actor );
	actor->m_position = m_finalPosition;
}

IActorComponent* GGroundFollowerComp::Create( )
{
	return new GGroundFollowerComp();
}