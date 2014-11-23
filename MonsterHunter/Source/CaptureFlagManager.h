// expand later
#ifndef __CAPTURE_FLAG_MANAGER_H_
#define __CAPTURE_FLAG_MANAGER_H_

#include "Utility/Singleton.h"
#include "Core/GMessageManager.h"

class CaptureFlagManager
{
public:

	friend Singleton<CaptureFlagManager>;

	void Initialize();
	void Update();
	void ScoreTeamOne();
	void ScoreTeamTwo();

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

	FlagTakenHandler	m_takenHandler;
	FlagReturnedHandler	m_returnHandler;

	int m_teamOneScore;
	int m_teamTwoScore;

	bool m_teamOneFlagTaken;
	bool m_teamTwoFlagTaken;

	char m_teamOneBuffer[32];
	char m_teamTwoBuffer[32];
};

typedef Singleton< CaptureFlagManager > g_CTFManager;

#endif