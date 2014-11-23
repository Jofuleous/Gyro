// Header Files
//=============

#include "cTime.h"

#include <cassert>

// Interface
//==========

// Time Functions
//---------------

float cs6963::cTime::GetTotalSecondsElapsed() const
{
	return static_cast<float>( static_cast<double>( m_totalCountsElapsed_duringRun.QuadPart ) * m_frequency );
}

float cs6963::cTime::GetSecondsElapsedThisFrame() const
{
	return static_cast<float>(
		static_cast<double>( m_totalCountsElapsed_duringRun.QuadPart - m_totalCountsElapsed_previousFrame.QuadPart )
		* m_frequency );
}

void cs6963::cTime::OnNewFrame()
{
	// Update the previous frame
	{
		m_totalCountsElapsed_previousFrame = m_totalCountsElapsed_duringRun;
	}
	// Update the current frame
	{
		LARGE_INTEGER totalCountsElapsed;
		BOOL result = QueryPerformanceCounter( &totalCountsElapsed );
		assert( result == TRUE );
		m_totalCountsElapsed_duringRun.QuadPart = totalCountsElapsed.QuadPart - m_totalCountsElapsed_atInitializion.QuadPart;
	}
}

// Initialization / Shutdown
//--------------------------

bool cs6963::cTime::Initialize()
{
	// Get the frequency of the high-resolution performance counter
	if ( QueryPerformanceFrequency( &m_countsPerSecond ) == TRUE )
	{
		m_frequency = 1.0 / static_cast<double>( m_countsPerSecond.QuadPart );
	}
	else
	{
		LogMessage( "Failed to query the frequency of counts-per-second" );
		return false;
	}

	// Make sure it's supported on this hardware
	if ( m_countsPerSecond.QuadPart != 0 )
	{
		if ( QueryPerformanceCounter( &m_totalCountsElapsed_atInitializion ) == TRUE )
		{
			LogMessage( "Initialized high resolution performance counter" );
			return true;
		}
		else
		{
			LogMessage( "Failed to query high resolution performance counter during initialization... Contact John-Paul" );
			return false;
		}
	}
	else
	{
		LogMessage( "High resolution performance counters aren't supported on this hardware... Contact John-Paul" );
		return false;
	}
}

// Rendererlementation
//===============

// Initialization / Shutdown
//--------------------------

cs6963::cTime::cTime()
	:
	m_frequency( 0.0 )
{
	m_countsPerSecond.QuadPart = 0;
	m_totalCountsElapsed_atInitializion.QuadPart = 0;
	m_totalCountsElapsed_duringRun.QuadPart = 0;
	m_totalCountsElapsed_previousFrame.QuadPart = 0;
}
