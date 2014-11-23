/*
	This class manages all time-related functionality
*/

#ifndef CS6963_CTIME_H
#define CS6963_CTIME_H

// Header Files
//=============

#include "Initialization.h"

// Class Definition
//=================

namespace cs6963
{
	class cTime
	{
		// Interface
		//==========

	public:

		// Singleton Access
		//-----------------

		static inline cTime& GetSingleton()
		{
			static cTime singleton;
			return singleton;
		}
		#define g_time cs6963::cTime::GetSingleton()

		// Time Functions
		//---------------

		float GetTotalSecondsElapsed() const;
		float GetSecondsElapsedThisFrame() const;

		void OnNewFrame();

		// Initialization / Shutdown
		//--------------------------

		bool Initialize();

		// Data
		//=====

	private:

		LARGE_INTEGER m_countsPerSecond;
		double m_frequency;
		LARGE_INTEGER m_totalCountsElapsed_atInitializion;
		LARGE_INTEGER m_totalCountsElapsed_duringRun;
		LARGE_INTEGER m_totalCountsElapsed_previousFrame;

		// Rendererlementation
		//===============

	private:

		// Initialization / Shutdown
		//--------------------------

		cTime();

	};
}

#endif	// CS6963_CTIME_H
