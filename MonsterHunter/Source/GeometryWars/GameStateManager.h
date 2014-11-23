#ifndef __HUNT_MANAGER_H_
#define __HUNT_MANAGER_H_

#include "Utility/Singleton.h"
#include "Utility/GSharedPtr.h"
#include "Actor/GActor.h"

class GameStateManager
{
public:
	friend Singleton<GameStateManager>;

	enum GameState
	{
		MENUINIT,
		MENU,
		GAMEINIT,
		GAME,
		LOSESCREEN,
	};

	GameStateManager() : m_state( MENUINIT ), m_highScore( 0 ), m_currentScore( 0 )
	{
	}

	void Initialize();
	void LoadMenuBS(); // just for loading blank scene.
	void LoadLevel();
	void UnloadLevel();
	void Update();
	void EndUpdate();
	void Shutdown( ) { }

	void HandleInput( unsigned int i_CharID, bool i_keyDown );
	void HandleInputMenu( unsigned int i_CharID, bool i_keyDown );
	void HandleInputGame( unsigned int i_CharID, bool i_keyDown );

	void UpdateMenu();
	void UpdateGame();
	void UpdateLoseScreen();
	void SetScore( int i_score ) { m_currentScore = i_score; }
	void SetHighScore( int i_score ) { m_highScore = i_score; }
	GActorHandle GetPlayer( );

	GActorHandle m_player;

	int m_currentScore;
	int m_highScore;
	char m_highScoreBuffer[32];
	char m_scoreBuffer[32];
	GameState m_state;

};

typedef Singleton< GameStateManager > g_GameStateManager;

#endif