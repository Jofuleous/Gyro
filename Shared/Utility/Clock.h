#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <windows.h>
#include "Utility/Debugging.h"
#include "Utility/Singleton.h"

namespace Gyro
{


class Clock
{
	friend Singleton<Clock>;
protected:
	
	u64			m_startCycles;
	u64			m_currentCycles;
	double		m_cyclesPerMillisecond;
	double		m_deltaTime;
	double		m_timeSinceStart;

	bool		m_isPaused;
	float		m_timeScale;

public:

	//wont worry about this too much for now.
	Clock( ) : m_isPaused( false )
	{
	}

	void Initialize()
	{
		LARGE_INTEGER li;

		//remove check later.
		if(!QueryPerformanceFrequency(&li))
			DEBUG_PRINT( "Query performance frequency failed." );

		m_cyclesPerMillisecond = double(li.QuadPart)/1000.0;

		QueryPerformanceCounter(&li);
		m_startCycles = li.QuadPart;
		m_currentCycles = li.QuadPart;
	}

	void Update()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );

		u64 lastFrameCycles = m_currentCycles;
		m_currentCycles = li.QuadPart;
		m_timeSinceStart = ( m_currentCycles - m_startCycles ) / m_cyclesPerMillisecond;
		m_deltaTime = ( m_currentCycles - lastFrameCycles ) / m_cyclesPerMillisecond;
		if( m_deltaTime > 200.0f ) // this basically implements pausing the clock...
			m_deltaTime = 0.0f;
	}

	// Performance warning: this polls the actual hardware...could take a while.
	float ActualMillisecondsSinceStart( )
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );

		return (float) (( li.QuadPart - m_startCycles ) / m_cyclesPerMillisecond);
	}

	double MillisecondsSinceStart()
	{
		return m_timeSinceStart;
	}

	double MillisecondsSinceLastFrame()
	{
		return m_deltaTime;
	}

	double SecondsSinceStart()
	{
		return m_timeSinceStart / 1000;
	}

	double SecondsSinceLastFrame()
	{
		return m_deltaTime / 1000;
	}

	u64	GetCurrentFrameTick()
	{
		return m_currentCycles;
	}

	u64 GetCurrentTick()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );
		return li.QuadPart;
	}

	double CyclesPerMillisecond()
	{
		return m_cyclesPerMillisecond;
	}

};


}

typedef Singleton<Gyro::Clock> g_Clock;

#endif