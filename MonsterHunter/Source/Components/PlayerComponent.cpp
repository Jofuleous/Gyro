#include "PlayerComponent.h"
#include "Components/GGroundFollowerComp.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Utility/Clock.h"
#include "../../../Renderer/Source/RenderManager.h"
#include "../../../Renderer/Source/Camera.h"
#include "Collision/CollisionManager.h"
#include "Misc/UserSettings.h"
#include "Misc/DebugConsole.h"
#include "Math/GTransform.h"
#include "Math/GQuat.h"
#include "Components/GAnimComponent.h"

const GHashedString PlayerComponent::m_typeName = "Player";
u32 PlayerComponent::m_typeId =ComponentManager::GetNextTypeId();

static const GVector3 DEFAULT_FOWARD( 0.0f, 0.0f, 1.0f );
static const float DEFAULT_CAMERA_XZ_DISTANCE = 11.0f; // was 600
static const float DEFAULT_CAMERA_Y_DISTANCE = 3.0f; // usual = 150
static const float MAX_STAMINA = 10.0f;
// not assigning this here because it's not safe to assume the compiler will execute in any certain order for globals.
static float DEFAULT_CAMERA_DISTANCE;
static float CAMERA_ZOOM_SPEED;

static const float DEFAULT_ANGLE = 3.14159f / 2.0f;
static const float DEFAULT_TURN_SPEED = 3.14159f * 1.5f; // turn 270 degrees in one second.
bool PlayerComponent::m_freeCam = false;

static float DEFAULT_YAW = 3.14159f /2.0f;
static float DEFAULT_YAW_SENSITIVITY = 1.0f;

static void PlayerCallback( const char* i_name, bool* i_value )
{
	// Place the first player at the camera location upon exit.
	std::vector<GActorHandle> actors;
	g_World::Get().GetActorsByName( "PlayerOne", actors );
	if( actors.size() > 0 )
	{
		GActor* thisActor = GActor::FromHandle( actors[0] );
		PlayerComponent* player = GetComponent<PlayerComponent>( actors[0] );
		if( player )
		{
			player->m_pathToTarget.m_PathNodes.clear();
			g_WayPointManager.QueuePathFind( &player->m_pathToTarget, thisActor->m_position, GVector3( -1500.0f, 176.090546f, -1500.000000 ), GDefaultHeuristic, NULL );
			player->m_drawPath = *i_value;
		}
	}
}

static void MusicCallback( const char* i_str, float* val )
{
	g_AudioManager.SetMusicVolume( *val );
}

static void SFXCallback( const char* i_str, float* val )
{
	g_AudioManager.SetEffectVolume( *val );
}

static void FreeCamCallback( const char* i_name, bool* i_value )
{
	if( i_value )
		g_RenderManager.GetCurrentScene()->GetCamera()->InitializeFreeCam();
	else
	{
		// Place the first player at the camera location upon exit.
		std::vector<GActorHandle> actors;
		g_World::Get().GetActorsByName( "PlayerOne", actors );
		if( actors.size() > 0 )
		{
			GActor* actor = GActor::FromHandle( actors[0] );
			assert( actor );
			actor->m_position = GVector3::FromD3DX( g_RenderManager.GetCurrentScene()->GetCamera()->m_position );
		}
	}
}


void PlayerComponent::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_camTargetOffsetY = i_obj["CameraOffsetY"].GetFloat();
	m_decelerate = 1.0f / i_obj["Decelerate"].GetFloat();
	m_maxSpeed = i_obj["MaxSpeed"].GetFloat();


	m_acceleration = m_originalAcceleration;
	//m_maxSpeed = m_originalMaxSpeed;
	m_inputPending = false;
	m_inputVector = GVector3::Zero;
	m_lastInputVector = m_inputVector;
	m_lastPosted = 0.0f;
	m_lookingDirection = DEFAULT_FOWARD;
	DEFAULT_CAMERA_DISTANCE = sqrt( DEFAULT_CAMERA_XZ_DISTANCE * DEFAULT_CAMERA_XZ_DISTANCE + DEFAULT_CAMERA_Y_DISTANCE * DEFAULT_CAMERA_Y_DISTANCE );
	CAMERA_ZOOM_SPEED = DEFAULT_CAMERA_DISTANCE * 3.0f; // zooming in takes 0.3333 seconds.
	m_cameraDistance = DEFAULT_CAMERA_DISTANCE;
	m_drawPath = false;
	m_staminaMeter = 0.0f;
	m_usingStamina = false;

	DebugConsole::AddCheckBox("Player FreeCamera", &m_freeCam, FreeCamCallback);
	DebugConsole::AddCheckBox( "Player PathToFlag", &m_drawPath, PlayerCallback );
	DebugConsole::AddSlider( "Audio MusicVolume", &g_AudioManager.m_musicVolume, 0.0f, 1.0f, 0.2f, MusicCallback );
	DebugConsole::AddSlider( "Audio SFXVolume", &g_AudioManager.m_sfxVolume, 0.0f, 1.0f, 0.2f, SFXCallback );

	m_walkOnGroundSlow = g_AudioManager.PlayEffect( "data/Audio/SFX/walkingonground.mp3", true, false, &m_walkOnGroundSound );
	m_walkOnMetalSlow = g_AudioManager.PlayEffect( "data/Audio/SFX/walkingonmetal.mp3", true, false, &m_walkOnGroundSound );
	m_pant = g_AudioManager.PlayEffect( "data/Audio/Dialog/heavy-breathing-off-mic-loop.mp3", true, false );
	m_pant->setVolume( 0.0f );
	m_walkOnMetalSlow->setVolume( 0.0f );
	//m_walkOnGroundFast = g_AudioManager.PlayEffect( "data/Audio/Dialog/heavy-breathing-off-mic-loop.mp3", true, true );
	m_pathToTarget.m_pathFound = false;
	m_mouse.Initialize();
}

PlayerComponent::~PlayerComponent( )
{

}

void PlayerComponent::Update( GActorHandle i_actor )
{
	//g_AudioManager.UpdateChannelPosition( m_walkOnGroundSlow, 
	if( m_drawPath )
		if( m_pathToTarget.m_pathFound )
			DrawFoundPath();

	D3DXVECTOR3 zero( 0.0f, 30.0f, 0.0f );
	D3DXVECTOR3 top( 0.0f, 90.0f, 0.0f );
	D3DXCOLOR	color( 1.0f, 0.0f, 0.0f, 1.0f );
	g_RenderManager.m_lines.AddLine( zero, top, 0.001f, 0.001f, 1.5f, color );

	
	D3DXVECTOR3 zero1( 10.0f, 10.0f, 0.0f );
	D3DXVECTOR3 top1( 10.0f, 20.0f, 0.0f );
	g_RenderManager.m_lines.AddLine( zero1, top1, 0.01f, 0.01f, 1.5f, color );

	D3DXCOLOR	color2( 1.0f, 1.0f, 1.0f, 1.0f );
	D3DXVECTOR3 zero2( 20.0f, 10.0f, 0.0f );
	D3DXVECTOR3 top2( 30.0f, 20.0f, 0.0f );
	g_RenderManager.m_lines.AddLine( zero2, top2, 0.01f, 0.007f, 1.5f, color2 );

	D3DXCOLOR	grey( 0.2f, 0.2f, 0.2f, 0.9f );
	g_RenderManager.m_hud.AddHUDBox( 0.0f, 0.0f, 1.0f, 0.04f, grey );

	u32 fps = (u32)(1000.0f / g_Clock::Get().MillisecondsSinceLastFrame());

	if( g_Clock::Get().MillisecondsSinceStart() - m_lastPosted > 50.0f )
	{
		sprintf( m_fpsBuffer, "FPS: %u", fps );
		m_lastPosted = g_Clock::Get().MillisecondsSinceStart();
	}

	g_RenderManager.m_hud.AddHUDText( 10, 0, 80, 30, color2, m_fpsBuffer );

	// do a simple reticle.
	// top
	g_RenderManager.m_hud.AddHUDBox( 0.498f, 0.48f, 0.502f, 0.495f, color2 );
	// bottom
	g_RenderManager.m_hud.AddHUDBox( 0.498f, 0.505f, 0.502f, 0.52f, color2 );
	// left
	g_RenderManager.m_hud.AddHUDBox( 0.488f, 0.497f, 0.498f, 0.503, color2 );
	// right
	g_RenderManager.m_hud.AddHUDBox( 0.502f, 0.497f, 0.512f, 0.503, color2 );

	if( m_usingStamina )
		m_staminaMeter -= g_Clock::Get().SecondsSinceLastFrame() * 5.0f;
	else
		m_staminaMeter += g_Clock::Get().SecondsSinceLastFrame() * 1.66667f;

	GMath::Clamp( &m_staminaMeter, 0.0f, 10.0f );

	D3DXCOLOR	green( 0.0f, 1.0f, 0.0f, 0.5f ); // green
	g_RenderManager.m_hud.AddHUDBox( 0.19,  0.04f, 0.79f * ( m_staminaMeter / MAX_STAMINA ), 0.06, green );



	HandleInputUpdate( i_actor );
}

void PlayerComponent::EndUpdate( GActorHandle i_actor)
{
	// Routing input to the renderer.
	if( m_freeCam )
	{
		Camera* renderCamera = g_RenderManager.GetCurrentScene()->GetCamera();
		renderCamera->HandleInput( m_inputVector.x(), m_inputVector.y(), m_inputVector.z(), g_Clock::Get().SecondsSinceLastFrame() );
		return;
	}

	GActor* actor = GActor::FromHandle( i_actor );

	// update the camera.  just for now, do basic zoom out.
	m_mouse.Update( );

	// set the cam target.
	GVector3 cameraTarget( actor->m_position );
	cameraTarget._y += m_camTargetOffsetY;
	cameraTarget.ToD3DX( &g_RenderManager.GetCurrentScene()->GetCamera()->m_lookAtPoint );

	GVector3 cameraPos;
	GVector3 rayDir;
	if( !m_mouse.CenterCursor() )
	{
		// set the cam position.
		cameraPos = -actor->m_rotation.Dir() * DEFAULT_CAMERA_DISTANCE;
		cameraPos += cameraTarget;
		cameraPos._y += 150.0f;
		rayDir = cameraPos - cameraTarget;
	
		float xPercent = (float) m_mouse.GetX() / (float)Gyro::UserSettings::GetScreenWidth();
		float yPercent = (float) m_mouse.GetY() / (float) Gyro::UserSettings::GetScreenHeight();
	
		//set the cam position.  for now let's make it dependent on resolution height?

		//DEBUG_PRINT( "X: %d \n", (int) cursorPos.x );
		//DEBUG_PRINT( "Y: %d \n", (int) cursorPos.y );
	}
	else
	{
		float pitchRads = ((float)m_mouse.GetTotalDeltaY() / (float) Gyro::UserSettings::GetScreenHeight() / 2.0f) * ( (3.14159f));
		float yawRads = -((float)m_mouse.GetTotalDeltaX() / (float) Gyro::UserSettings::GetScreenHeight() / 2.0f) * ( (3.14159f) ); /* ( (3.14159f) */;
		rayDir = GVector3( cos( yawRads )*cos( pitchRads ), sin( pitchRads), sin( yawRads )*cos( pitchRads ) );

		// check for world collisions.
		cameraPos = rayDir.Normal()*DEFAULT_CAMERA_DISTANCE;
		cameraPos += cameraTarget;
	}

	GRayQuery query( cameraTarget, rayDir.Normal(), g_Collision::Get().GetCollisionMask( "TERRAIN" ), DEFAULT_CAMERA_DISTANCE );
	if( g_Collision::Get().RayQuery( query ) )
	{

		if( query.m_hitDistance < m_cameraDistance )
			m_cameraDistance -= g_Clock::Get().SecondsSinceLastFrame() * CAMERA_ZOOM_SPEED;
		else if( query.m_hitDistance > m_cameraDistance )
		{
			m_cameraDistance += g_Clock::Get().SecondsSinceLastFrame() * CAMERA_ZOOM_SPEED;
			if( m_cameraDistance > query.m_hitDistance )
				m_cameraDistance = query.m_hitDistance;
		}

		if( m_cameraDistance < query.m_hitDistance )
			m_cameraDistance = query.m_hitDistance;

		if( m_cameraDistance > DEFAULT_CAMERA_DISTANCE )
			m_cameraDistance = DEFAULT_CAMERA_DISTANCE;
	}
	else
	{
		if( m_cameraDistance < DEFAULT_CAMERA_DISTANCE )
		{
			m_cameraDistance += g_Clock::Get().SecondsSinceLastFrame() * CAMERA_ZOOM_SPEED;	
		}

	}

	cameraPos = query.m_start + query.m_direction * m_cameraDistance;


	cameraPos.ToD3DX( &g_RenderManager.GetCurrentScene()->GetCamera()->m_position );

}

IActorComponent* PlayerComponent::Create( )
{
	return new PlayerComponent();
}

void PlayerComponent::HandleInputUpdate( GActorHandle i_actor )
{
	// We're passing input to the renderer.
	if( m_freeCam )
		return;

	GActor* actor = GActor::FromHandle( i_actor );
	GGroundFollowerComp* follower = GetComponent<GGroundFollowerComp>( i_actor );

	if( m_mouse.LMBDown() )
	{
		// bleh...
		Camera* c = g_RenderManager.GetCurrentScene()->GetCamera();
		GVector3 cameraPos = GVector3::FromD3DX( &c->m_position );
		GVector3 dir = GVector3::FromD3DX( c->m_lookAtPoint );
		dir = dir - cameraPos; 
		dir.Normalize();
		GVector3 origin( actor->m_position );
		origin.y( origin.y() + 150.0f );
		//m_weaponInv.FireBlaster( i_actor, origin, dir, follower->m_velocity );
	}

	if( follower )
	{
		// giant fucking dirty ass hack.
		if( actor->m_position.y() > -220.0f )
		{
			m_walkOnGroundSlow->setVolume( 0.0f );
			if( follower->m_velocity.LengthSquared() > 5.0f )
				m_walkOnMetalSlow->setVolume( 1.0f );
			else
				m_walkOnMetalSlow->setVolume( 0.0f );
		}
		else
		{
			m_walkOnMetalSlow->setVolume( 0.0f );
			if( follower->m_velocity.LengthSquared() > 5.0f )
				m_walkOnGroundSlow->setVolume( 1.0f );
			else
				m_walkOnGroundSlow->setVolume( 0.0f );
		}
		if( !follower->m_onGround )
		{
			m_walkOnGroundSlow->setVolume( 0.0f );
			m_walkOnMetalSlow->setVolume( 0.0f );
		}

		GVector3 movementVector( m_inputVector ); // multiply by direction vectors later...
		if( movementVector.z() == 0.0f )
		{

			//animcomponent
			if (follower->m_velocity.LengthSquared() > 0.1f)
			{
				GAnimComponent* animComp = GetComponent<GAnimComponent>(i_actor);
				if (animComp)
					animComp->PlayAnim("Avatar/Player/GoblinIdle.banm");
			}

			if( follower->m_onGround )
			{
				GMath::MoveToValue( follower->m_velocity._x, 0.0f, fabs( follower->m_velocity._x ) * m_decelerate * g_Clock::Get().SecondsSinceLastFrame() );
				GMath::MoveToValue( follower->m_velocity._z, 0.0f, fabs( follower->m_velocity._z ) * m_decelerate * g_Clock::Get().SecondsSinceLastFrame() );
				//follower->m_velocity._z = 0.0f;
			}
			else
			{
				m_walkOnGroundSlow->setVolume( 0.0f );
				/*
				if( m_mouse.LMBDown() )
				{
					Camera* c = g_RenderManager.GetCurrentScene()->GetCamera();
					GVector3 cameraPos = GVector3::FromD3DX( c->m_position );
					GVector3 dir = GVector3::FromD3DX( c->m_lookAtPoint );
					dir = dir - cameraPos; 
					dir.Normalize();
					dir = -dir * 4000.0f;
					if( fabs( dir.y() ) > 300.0f )
						dir.y( GMath::Sign( dir.y() ) * 300.0f );
					follower->ApplyAcceleration( dir );
				}
				*/
			}
		}

		// eventually, this should control strafing (when shane let's us), and will orient itself based on the camera view vector.
		// this is hacky.  it assumes that the player will only ever be oriented with an up vector of ( 0, 1, 0 )...

		GMatrix4 orientation;
		if( !m_mouse.CenterCursor() )
		{
			GVector3 lookingDir = actor->m_rotation.Dir();
			float rots = atan2( lookingDir._x, lookingDir._z );
			float difference = g_Clock::Get().SecondsSinceLastFrame() * DEFAULT_TURN_SPEED * movementVector._x ;
			rots += difference;
			orientation.SetRotY( rots );
			actor->m_rotation = orientation;
		}
		else
		{
			//float difference = g_Clock::Get().SecondsSinceLastFrame() * DEFAULT_TURN_SPEED * movementVector._x ;
			if( fabs(movementVector._z ) > 0.0f )
			{
				//animcomponent
				if (follower->m_velocity.LengthSquared() <= 0.1f)
				{
					GAnimComponent* animComp = GetComponent<GAnimComponent>(i_actor);
					if (animComp)
						animComp->PlayAnim("Avatar/Player/GoblinRun.banm");
				}
				float pitchRads = ((float)m_mouse.GetTotalDeltaY() / (float) Gyro::UserSettings::GetScreenHeight() / 2.0f) * ( (3.14159f));
				float yawRads = -((float)m_mouse.GetTotalDeltaX() / (float) Gyro::UserSettings::GetScreenHeight() / 2.0f) * ( (3.14159f) ); /* ( (3.14159f) */;
				GVector3 lookingDir( -cos( yawRads )*cos( pitchRads ), -sin( pitchRads), -sin( yawRads )*cos( pitchRads ) );
				float rots = atan2( lookingDir._x, lookingDir._z );
				float difference = asinf( movementVector._x ) ;
				rots+= difference;

				orientation.SetRotY(rots);
				GMatrix4 actorOrientHack;
				actorOrientHack.SetRotY(rots + GMath::Pi);
				actor->m_rotation = actorOrientHack;
			}
			else
			{
				GVector3 lookingDir = actor->m_rotation.Dir();
				float rots = atan2( lookingDir._x, lookingDir._z );
				float difference = g_Clock::Get().SecondsSinceLastFrame() * DEFAULT_TURN_SPEED * movementVector._x ;
				rots += difference;
				orientation.SetRotY( rots );
				actor->m_rotation = orientation;
			}
		}

		//if( follower->m_onGround )
		//{
			GVector3 direction = orientation.Dir();
			if( m_usingStamina && m_staminaMeter > 0.0f )
			{
				follower->m_maxSpeedXZ = m_maxSpeed * 2.5f;
				follower->ApplyAccelerationVectorXZ( 2.0f * direction.x() * movementVector.z(), 2.0f * direction.z() * movementVector.z() );
				m_pant->setVolume( 1.0f );
			}
			else
			{
				m_pant->setVolume( 0.0f );
				follower->m_maxSpeedXZ = m_maxSpeed;
				follower->ApplyAccelerationVectorXZ( direction.x() * movementVector.z(), direction.z() * movementVector.z() );
			}
		//}

		if( fabs( movementVector.y() ) > 0.0f )
			follower->ApplyJumpAccelerationY( movementVector.y() * 600.0f ); 
	}

	m_inputs.clear();
}

void PlayerComponent::DrawFoundPath( )
{
	for( int i = 0; i < m_pathToTarget.m_PathNodes.size()-1; i++ )
	{
		u16 nodeId = m_pathToTarget.m_PathNodes[i];
		D3DXCOLOR	color( 1.0f, 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 one, two;
		GVector3 v_one, v_two;
		g_WayPointManager.NodePosition( m_pathToTarget.m_PathNodes[i], v_one );
		v_one.ToD3DX( &one );
		g_WayPointManager.NodePosition( m_pathToTarget.m_PathNodes[i+1], v_two );
		v_two.ToD3DX( &two );
		g_RenderManager.m_lines.AddLine( one, two, 0.001f, 0.001f, 1.5f, color );
	}
}

void PlayerComponent::HandleInput( GActorHandle actor, unsigned int i_CharID, bool i_keyDown )
{
	if (i_CharID == 87 || i_CharID == 68 || i_CharID == 65 || i_CharID == 83 || i_CharID == VK_SPACE || i_CharID == VK_CONTROL || i_CharID == 0x59 || i_CharID == 0x55 || i_CharID == 0x54)
	{
		if( i_CharID == VK_CONTROL && i_keyDown )
		{
			m_mouse.CenterCursor( !m_mouse.CenterCursor() );
			m_mouse.HideCursor( m_mouse.CenterCursor() );
		}

		m_lastInputVector = m_inputVector;

		if( i_keyDown )
		{
			if( i_CharID == 87 ) // A/UP
				m_inputVector.z( 1.0f );
			if( i_CharID == 68 ) // D/RIGHT
				m_inputVector.x( 1.0f );
			if( i_CharID == 65 ) // A/LEFT
				m_inputVector.x( -1.0f );
			if( i_CharID == 83 ) // S/DOWN
				m_inputVector.z( -1.0f );
			if( i_CharID == VK_SPACE )
			{
				// locomotion is jacked up right now.  don't jump.  use stamina.
				m_usingStamina = true;
			//	m_inputVector.y( 1.0f );
			}
			if (i_CharID == 0x59 || i_CharID == 0x55 || i_CharID == 0x54 )
			{
				GAnimComponent* animComp = GetComponent<GAnimComponent>(actor);
				if (animComp && i_CharID == 0x54 )
					animComp->PlayAnim("Avatar/Player/GoblinDance.banm");
				if (animComp && i_CharID == 0x59 )
					animComp->PlayAnim("Avatar/Player/GoblinRunSlow.banm");
				else if (animComp && i_CharID == 0x55)
					animComp->PlayAnim("Avatar/Player/GoblinDanceSlow.banm");
			}
		}
		else
		{
			if( i_CharID == 87 ) // A/UP
			{
				if( m_inputVector.z() > 0.0f )
					m_inputVector.z( 0.0f );
			}
			if( i_CharID == 68 ) // D/RIGHT
			{
				if( m_inputVector.x() > 0.0f )
					m_inputVector.x( 0.0f );
			}
			if( i_CharID == 65 ) // A/LEFT
			{
				if( m_inputVector.x() < 0.0f )
					m_inputVector.x( 0.0f );
			}
			if( i_CharID == 83 ) // S/DOWN
			{
				if( m_inputVector.z() < 0.0f )
					m_inputVector.z( 0.0f );
			}
			if( i_CharID == VK_SPACE )
			{
				m_usingStamina = false;
				//m_inputVector.y( 0.0f );
			}
		}

		if( m_inputVector.LengthSquared() > 0.0f )
			m_inputVector.Normalize();
	}
	else if( m_freeCam && (i_CharID == VK_UP || i_CharID == VK_DOWN) )
	{
		if( i_keyDown )
		{
			if( i_CharID == VK_UP ) // arrow up
				m_inputVector.y( 1.0f );
			else if( i_CharID == VK_DOWN ) // arrow down
				m_inputVector.y( -1.0f );
		}
		else
			m_inputVector.y( 0.0f );
	}
}