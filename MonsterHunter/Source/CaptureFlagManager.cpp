#include "CaptureFlagManager.h"
#include "../../Renderer/Source/RenderManager.h"
#include "Audio/GAudioManager.h"

static const float SCORE_BOX_OFFSET_Y = 0.04f;
static const float SCORE_BOX_HEIGHT = 0.05;
static const float SCORE_BOX_WIDTH = 0.1f;

static const float RED_BOX_LEFT = 0.09f;
static const float BLUE_BOX_LEFT = 0.79f;

void CaptureFlagManager::Initialize()
{
	m_teamOneFlagTaken = false;
	m_teamTwoFlagTaken = false;

	m_takenHandler.m_messageOwner = this;
	m_returnHandler.m_messageOwner = this;

	m_teamOneScore = 0;
	m_teamTwoScore = 0;

	g_MessageManager::Get().RegisterHandler( "FlagReturned", &m_returnHandler );
	g_MessageManager::Get().RegisterHandler( "FlagTaken", &m_takenHandler );
}

CaptureFlagManager::FlagReturnedHandler::~FlagReturnedHandler()
{
	g_MessageManager::Get().RemoveHandler( "FlagReturned", this );
}

CaptureFlagManager::FlagTakenHandler::~FlagTakenHandler()
{
	g_MessageManager::Get().RemoveHandler( "FlagTaken", this );
}

void CaptureFlagManager::Update()
{
	// draw the score.
	D3DXCOLOR	white( 1.0f, 1.0f, 1.0f, 1.0f ); // white
	//g_RenderManager.m_hud.AddHUDBox( 0.0f, 0.1f, 0.1f, 0.04f, red );
	sprintf( m_teamOneBuffer, "Team One: %d", m_teamOneScore );
	g_RenderManager.m_hud.AddHUDText( 100, 30, 150, 60, white, (const char*) m_teamOneBuffer );
	sprintf( m_teamTwoBuffer, "Team Two: %d", m_teamTwoScore );
	g_RenderManager.m_hud.AddHUDText( 800, 30, 150, 60, white, (const char*) m_teamTwoBuffer );

	// TEAM ONE ( RED )
	D3DXCOLOR	red( 1.0f, 0.0f, 0.0f, 0.5f ); // red
	g_RenderManager.m_hud.AddHUDBox( RED_BOX_LEFT, SCORE_BOX_OFFSET_Y, RED_BOX_LEFT + SCORE_BOX_WIDTH, SCORE_BOX_OFFSET_Y + SCORE_BOX_HEIGHT, red );

	// TEAM TWO ( BLUE )
	D3DXCOLOR	blue( 0.0f, 0.0f, 1.0f, 0.5f ); // blue
	g_RenderManager.m_hud.AddHUDBox( BLUE_BOX_LEFT, SCORE_BOX_OFFSET_Y, BLUE_BOX_LEFT + SCORE_BOX_WIDTH, SCORE_BOX_OFFSET_Y + SCORE_BOX_HEIGHT, blue );
}

void CaptureFlagManager::FlagTakenHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	int team = (int) i_data;
	CaptureFlagManager* manager = (CaptureFlagManager*) m_messageOwner;
	if( team == 1 )
		manager->m_teamOneFlagTaken = true;
	else
		manager->m_teamTwoFlagTaken = true;
}

void CaptureFlagManager::FlagReturnedHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	int team = (int) i_data;
	CaptureFlagManager* manager = (CaptureFlagManager*) m_messageOwner;
	if( team == 1 )
		manager->m_teamOneFlagTaken = false;
	else
		manager->m_teamTwoFlagTaken = false;
}


void CaptureFlagManager::ScoreTeamOne()
{
	m_teamOneScore++;
	g_AudioManager.PlayEffect( "data/Audio/Dialog/flag_captured.mp3", false, false );
	g_MessageManager::Get().ProcessMessage( "FlagReturned", (void*) 2 );
}

void CaptureFlagManager::ScoreTeamTwo()
{
	m_teamTwoScore++;
	g_AudioManager.PlayEffect( "data/Audio/Dialog/flag_captured.mp3", false, false );
	g_MessageManager::Get().ProcessMessage( "FlagReturned", (void*) 1 );
}