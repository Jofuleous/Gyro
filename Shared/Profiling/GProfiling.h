#ifndef __GPROFILING_H_
#define __GPROFILING_H_

#include "Utility/Clock.h"
#include "Containers/GHashMap.h"
#include "Utility/Singleton.h"
#include "Utility/GHashedString.h"
#include "Utility/Debugging.h"

#define ENABLE_PROFILING

struct _GAccumulator
{
	const char* m_Name;
	u64 m_Max;
	u64 m_Min;
	u64	m_Count;
	u64 m_Sum;
};

class GProfiler
{
public:

	friend class Singleton<GProfiler>;

	GProfiler() : m_accumulators( 20 ) {}
	~GProfiler()
	{ 
		m_accumulators.Clear();
	}
	void		AddTime( const char* i_ScopeName, u64 i_Start, u64 i_End )
	{
		unsigned hash = GHashedString::Hash( i_ScopeName );
		GHashNode< u32, _GAccumulator>* node = m_accumulators[ hash ];

		u64 difference = i_End - i_Start;

		if( node )
		{
			if( difference > node->m_data.m_Max )
				node->m_data.m_Max = difference;
			if( difference < node->m_data.m_Min )
				node->m_data.m_Min = difference;

			node->m_data.m_Count++;
			node->m_data.m_Sum += difference;
		}
		else
		{
			_GAccumulator a;
			a.m_Name = i_ScopeName;
			a.m_Count = 1;
			a.m_Max = difference;
			a.m_Min = difference;
			a.m_Sum = difference;
			m_accumulators.Set( hash, a );
		}
	}

	void	PrintAccumulators( void )
	{
		// hack until i make a decent iterator for my hashmaps...
		for( u32 i = 0; i < m_accumulators.Size(); i++ )
		{
			GHashNode< u32, _GAccumulator >* node = m_accumulators.Index( i );
			while( node != NULL )
			{
#ifdef _WIN32
				ConsolePrint( 1, "Scope \"%s\" - Min = %llu - Max = %llu - Count = %llu - Average MS = %lf \n", node->m_data.m_Name,
					node->m_data.m_Min, node->m_data.m_Max, node->m_data.m_Count, (((double)node->m_data.m_Sum) / (double)node->m_data.m_Count) / g_Clock::Get().CyclesPerMillisecond() );
#endif
				node = node->m_next;
			}
		}
	}

	GHashMap< u32, _GAccumulator > m_accumulators;
};

typedef Singleton<GProfiler> g_Profiler;

class GScopedTimer
{
public:
	GScopedTimer( const char* i_ScopeName ) :
		m_ScopeName( i_ScopeName ),
		m_Start( g_Clock::Get().GetCurrentTick() )
		{
		}

	~GScopedTimer( )
	{
		g_Profiler::Get().AddTime( m_ScopeName, m_Start, g_Clock::Get().GetCurrentTick() );
	}

	const char* m_ScopeName;
	u64			m_Start;
};


#if defined( ENABLE_PROFILING )
#define PROFILE_SCOPE_BEGIN(str) { GScopedTimer __Timer( str );
#define PROFILE_SCOPE_END() }

#define PROFILE_UNSCOPED(str) GScopedTimer __Timer( str );
#define PROFILE_PRINT_RESULTS() g_Profiler::Get().PrintAccumulators();
#else

#define PROFILE_SCOPE_BEGIN(str) __noop;
#define PROFILE_SCOPE_END() __noop;

#define PROFILE_UNSCOPED(str) __noop;
#define PROFILE_PRINT_RESULTS() __noop;

#endif

#endif
