#include "GameStateManager.h"
#include "Utility/Clock.h"
#include "Core/World.h"
#include "Renderer/GRenderer.h"
#include "Collision/CollisionManager.h"
#include "../../Renderer/Source/RenderManager.h"
#include "QuakePlayerComp.h"
#include "Physics/GPhysics.h"
#include "SpawnManager.h"

void GameStateManager::Initialize()
{
	m_highScore = 0;
	m_currentScore = 0;
}

void GameStateManager::Update()
{
	switch( m_state )
	{
		case MENUINIT:
			LoadMenuBS();
			m_state = MENU;
		break;
		case MENU:
			UpdateMenu();
		break;
		case GAMEINIT:
			LoadLevel();
			m_state = GAME;
		break;
		case GAME:
			UpdateGame();
		break;
	}
}

void GameStateManager::HandleInput( unsigned int i_CharID, bool i_keyDown )
{
	switch( m_state )
	{
		case MENU:
			HandleInputMenu( i_CharID, i_keyDown );
		break;
		case GAMEINIT:
		break;
		case GAME:
			HandleInputGame( i_CharID, i_keyDown );
		break;
	}
}

/////////////////////////////////////////////////////
////////////////	MENU	/////////////////////////
/////////////////////////////////////////////////////


static const float SCORE_BOX_OFFSET_Y = 0.04f;
static const float SCORE_BOX_HEIGHT = 0.05;
static const float SCORE_BOX_WIDTH = 0.1f;

static const float RED_BOX_LEFT = 0.09f;
static const float BLUE_BOX_LEFT = 0.79f;
static char* GAME_NAME = "3D Geometry Wars";
static char* GAME_STATE = "Press Space to Play";
static char	m_highScoreBuffer[128];

void GameStateManager::LoadMenuBS()
{
	g_RenderManager.GetCurrentScene()->LoadBlankScene();
}

void GameStateManager::HandleInputMenu( unsigned int i_CharID, bool i_keyDown )
{
	if( i_keyDown && i_CharID == VK_SPACE )
	{
		LoadLevel();
		m_state = GAME;
	}
}

void GameStateManager::UpdateMenu()
{
	// draw the score.
	D3DXCOLOR	white( 1.0f, 1.0f, 1.0f, 1.0f ); // white
	//g_RenderManager.m_hud.AddHUDBox( 0.0f, 0.1f, 0.1f, 0.04f, red );

	g_RenderManager.m_hud.AddHUDText( 250, 30, 400, 60, white, (const char*) GAME_NAME );
	g_RenderManager.m_hud.AddHUDText( 250, 70, 400, 100, white, (const char*) GAME_STATE );

	sprintf( m_highScoreBuffer, "High Score: %d", m_highScore );
	g_RenderManager.m_hud.AddHUDText( 250, 110, 4000, 140, white, (const char*) m_highScoreBuffer );

}

/////////////////////////////////////////////////////
////////////////	LOAD!!1!	/////////////////////
/////////////////////////////////////////////////////

void GameStateManager::LoadLevel( )
{
	// has to be done first to get the lua state.  it's...very bad.
	g_SpawnManager::Get().Initialize();

	g_World::Get().LoadEditorObjects( "data/Levels/Dune.brox" );
	g_Renderer::Get().LoadBinaryScene("data/Levels/Dune.scx" );
	g_Collision::Get().LoadBinarySceneCollision( "data/Levels/Dune.scx" );

	Gyro::World& world = g_World::Get();
	m_player = world.GetActorByName( "QuakePlayer" );

}

void GameStateManager::UnloadLevel()
{
	g_World::Get().Cleanse();
	g_Renderer::Get().Cleanse();
	g_Physics::Get().Cleanse();
	g_Collision::Get().Cleanse();
	g_SpawnManager::Get().Cleanse();
}


/////////////////////////////////////////////////////
////////////////	GAMEZ!!!	/////////////////////
/////////////////////////////////////////////////////
void GameStateManager::HandleInputGame( unsigned int i_CharID, bool i_keyDown )
{
	if( m_player )
	{
		QuakePlayerComp* pC = GetComponent<QuakePlayerComp>( m_player );
		pC->HandleInput( m_player, i_CharID, i_keyDown );
	}
}

void GameStateManager::UpdateGame()
{
	g_SpawnManager::Get().Update( );

	// write the scores.
	D3DXCOLOR	white( 1.0f, 1.0f, 1.0f, 1.0f );

	sprintf( m_scoreBuffer, "Score: %d", m_currentScore );
	g_RenderManager.m_hud.AddHUDText( 200, 0, 300, 30, white, m_scoreBuffer );

	sprintf( m_highScoreBuffer, "High Score: %d", m_highScore );
	g_RenderManager.m_hud.AddHUDText( 500, 0, 600, 30, white, m_highScoreBuffer );
}

void GameStateManager::EndUpdate()
{
	if( m_state == GAME )
	{
		GActor* player = GActor::FromHandle( m_player );
		if( player && !player->m_markedForDelete  )
		{

		}
		else
		{
			UnloadLevel();
			m_state = MENUINIT;
		}
	}
}