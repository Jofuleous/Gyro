#include "QuakePlayerComp.h"
#include "Components/GGroundFollowerComp.h"
#include "Actor/ComponentManager.h"
#include "Core/World.h"
#include "Utility/Clock.h"
#include "../../../Renderer/Source/RenderManager.h"
#include "../../../Renderer/Source/Camera.h"
#include "Collision/CollisionManager.h"
#include "Misc/UserSettings.h"
#include "GameStateManager.h"

#include "Misc/DebugConsole.h"

const GHashedString QuakePlayerComp::m_typeName = "QuakePlayer";
u32 QuakePlayerComp::m_typeId =ComponentManager::GetNextTypeId();

static const GVector3 DEFAULT_FOWARD( 0.0f, 0.0f, 1.0f );
static const float DEFAULT_CAMERA_XZ_DISTANCE = 1000.0f; // was 600
static const float DEFAULT_CAMERA_Y_DISTANCE = 500.0f; // usual = 150
// not assigning this here because it's not safe to assume the compiler will execute in any certain order for globals.
static float DEFAULT_CAMERA_DISTANCE;
static float CAMERA_ZOOM_SPEED;

static const float DEFAULT_ANGLE = 3.14159f / 2.0f;
static const float DEFAULT_TURN_SPEED = 3.14159f * 1.5f; // turn 270 degrees in one second.
bool QuakePlayerComp::m_freeCam = false;

static float DEFAULT_YAW = 3.14159f /2.0f;
static float DEFAULT_YAW_SENSITIVITY = 1.0f;

void PlayerCallback( const char* i_name, float* i_value )
{

}

void FreeCamCallback( const char* i_name, bool* i_value )
{
	if( i_value )
		g_RenderManager.GetCurrentScene()->GetCamera()->InitializeFreeCam();
	else
	{
		// Place the first player at the camera location upon exit.
		std::vector<GActorHandle> actors;
		g_World::Get().GetActorsByName( "QuakePlayer", actors );
		if( actors.size() > 0 )
		{
			GActor* actor = GActor::FromHandle( actors[0] );
			actor->m_position = GVector3::FromD3DX( g_RenderManager.GetCurrentScene()->GetCamera()->m_position );
		}
	}
}


void QuakePlayerComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_camTargetOffsetY = i_obj["CameraOffsetY"].GetFloat();
	m_decelerate = 1.0f / i_obj["Decelerate"].GetNumber();


	m_acceleration = m_originalAcceleration;
	m_maxSpeed = m_originalMaxSpeed;
	m_inputPending = false;
	m_inputVector = GVector3::Zero;
	m_lastInputVector = m_inputVector;
	m_lastPosted = 0.0f;
	m_lookingDirection = DEFAULT_FOWARD;
	DEFAULT_CAMERA_DISTANCE = sqrt( DEFAULT_CAMERA_XZ_DISTANCE * DEFAULT_CAMERA_XZ_DISTANCE + DEFAULT_CAMERA_Y_DISTANCE * DEFAULT_CAMERA_Y_DISTANCE );
	CAMERA_ZOOM_SPEED = DEFAULT_CAMERA_DISTANCE * 3.0f; // zooming in takes 0.3333 seconds.
	m_cameraDistance = DEFAULT_CAMERA_DISTANCE;
	//m_weaponInv = new PlayerWeaponInv();
	m_weaponInv.Initialize( i_obj["PlayerWeaponInv"] );
	//m_lastHitDistance = DEFAULT_CAMERA_DISTANCE;
	m_lives = 3;
	m_enemyHandler.m_messageOwner = this;
	g_MessageManager::Get().RegisterHandler( "EnemyCollided", &m_enemyHandler );

	//DebugConsole::AddSlider( "General RunSpeed", &m_maxSpeed, 0.0f, m_originalMaxSpeed * 10, m_originalMaxSpeed, PlayerCallback );
	//DebugConsole::AddCheckBox( "Camera FreeCamera", &m_freeCam, FreeCamCallback );

	m_mouse.Initialize();
}

QuakePlayerComp::~QuakePlayerComp( )
{
	// potentially bad to do this here?
	if( g_GameStateManager::Get().m_highScore < m_weaponInv.m_totalKilled )
		g_GameStateManager::Get().SetHighScore( m_weaponInv.m_totalKilled );

	g_MessageManager::Get().RemoveHandler( "EnemyCollided", &m_enemyHandler );
}

void QuakePlayerComp::EnemyCollisionHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	// i_data is a collision contact...
	GContact* contact = (GContact*) i_data;

	assert( contact->m_first->m_owner ); // pointer is null
	assert( contact->m_second->m_owner );

	QuakePlayerComp* comp = (QuakePlayerComp*) m_messageOwner;

	GActorHandle playerHandle;
	GActorHandle otherHandle;
	GActor* first = GActor::FromHandle( contact->m_first->m_owner );
	GActor* second = GActor::FromHandle( contact->m_second->m_owner );
	if( !first || !second )
		return;

	if( first->IsA( "QuakePlayer" ))
	{
		playerHandle = contact->m_first->m_owner;
		otherHandle = contact->m_second->m_owner;
	}
	else
	{
		playerHandle = contact->m_second->m_owner;
		otherHandle = contact->m_first->m_owner;
	}

	GActor* other = GActor::FromHandle( otherHandle );
	GActor* player = GActor::FromHandle( playerHandle );

	if( comp && !other->m_markedForDelete)
	{
		other->Kill();
		comp->m_lives--;
		if( comp->m_lives <= 0 )
		{
			player->Kill();
		}
	}
}

void QuakePlayerComp::Update( GActorHandle i_actor )
{

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

	sprintf( m_livesBuffer, "Lives: %d", m_lives );
	g_RenderManager.m_hud.AddHUDText( 80, 0, 150, 30, color2, m_livesBuffer );

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

	g_GameStateManager::Get().SetScore( m_weaponInv.m_totalKilled );

	HandleInputUpdate( i_actor );

	m_weaponInv.Update( );
}

void QuakePlayerComp::EndUpdate( GActorHandle i_actor)
{
	GActor* actor = GActor::FromHandle( i_actor );
	// Routing input to the renderer.
	if( m_freeCam )
	{
		Camera* renderCamera = g_RenderManager.GetCurrentScene()->GetCamera();
		renderCamera->HandleInput( m_inputVector.x(), m_inputVector.y(), m_inputVector.z(), g_Clock::Get().SecondsSinceLastFrame() );
		return;
	}

	// update the camera.  just for now, do basic zoom out.
	m_mouse.Update( );

	// set the cam target.
	GVector3 cameraTarget( actor->m_position );
	cameraTarget._y += m_camTargetOffsetY;
	cameraTarget.ToD3DX( g_RenderManager.GetCurrentScene()->GetCamera()->m_lookAtPoint );

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


	cameraPos.ToD3DX( g_RenderManager.GetCurrentScene()->GetCamera()->m_position );

}

IActorComponent* QuakePlayerComp::Create( )
{
	return new QuakePlayerComp();
}

void QuakePlayerComp::HandleInputUpdate( GActorHandle i_actor )
{
	// We're passing input to the renderer.
	if( m_freeCam )
		return;

	GGroundFollowerComp* follower = GetComponent<GGroundFollowerComp>( i_actor );
	GActor* actor = GActor::FromHandle( i_actor );

	if( m_mouse.LMBDown() )
	{
		// bleh...
		Camera* c = g_RenderManager.GetCurrentScene()->GetCamera();
		GVector3 cameraPos = GVector3::FromD3DX( c->m_position );
		GVector3 dir = GVector3::FromD3DX( c->m_lookAtPoint );
		dir = dir - cameraPos; 
		dir.Normalize();
		GVector3 origin( actor->m_position );
		origin.y( origin.y() + 150.0f );
		m_weaponInv.FireBlaster( i_actor, origin, dir, follower->m_velocity );
	}

	if( follower )
	{
		// check to see if signs are different.  kill velocity if so...

		GVector3 movementVector( m_inputVector ); // multiply by direction vectors later...
		if( movementVector.z() == 0.0f )
		{
			if( follower->m_onGround )
			{
				GMath::MoveToValue( follower->m_velocity._x, 0.0f, fabs( follower->m_velocity._x ) * m_decelerate * g_Clock::Get().SecondsSinceLastFrame() );
				GMath::MoveToValue( follower->m_velocity._z, 0.0f, fabs( follower->m_velocity._z ) * m_decelerate * g_Clock::Get().SecondsSinceLastFrame() );
				//follower->m_velocity._z = 0.0f;
			}
			else
			{
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
				float pitchRads = ((float)m_mouse.GetTotalDeltaY() / (float) Gyro::UserSettings::GetScreenHeight() / 2.0f) * ( (3.14159f));
				float yawRads = -((float)m_mouse.GetTotalDeltaX() / (float) Gyro::UserSettings::GetScreenHeight() / 2.0f) * ( (3.14159f) ); /* ( (3.14159f) */;
				GVector3 lookingDir( -cos( yawRads )*cos( pitchRads ), -sin( pitchRads), -sin( yawRads )*cos( pitchRads ) );
				float rots = atan2( lookingDir._x, lookingDir._z );
				float difference = asinf( movementVector._x ) ;
				rots+= difference;

				orientation.SetRotY( rots );
				actor->m_rotation = orientation;
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
			follower->ApplyAccelerationVectorXZ( direction.x() * movementVector.z(), direction.z() * movementVector.z() );
		//}

		if( fabs( movementVector.y() ) > 0.0f )
			follower->ApplyJumpAccelerationY( movementVector.y() * 5000.0f ); 
	}

	m_inputs.clear();
}

void QuakePlayerComp::HandleInput( GActorHandle actor, unsigned int i_CharID, bool i_keyDown )
{
	if( i_CharID == 87 || i_CharID == 68 || i_CharID == 65 || i_CharID == 83 || i_CharID == VK_SPACE || i_CharID == VK_CONTROL )
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
				m_inputVector.y( 1.0f );
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
				m_inputVector.y( 0.0f );
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