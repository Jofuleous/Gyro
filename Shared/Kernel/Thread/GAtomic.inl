#ifdef _WIN32 || _WIN64
#include <Windows.h>
#endif

uint32_t GAtomic::CompareExchange(volatile uint32_t& o_Dest, uint32_t i_Exchange, uint32_t i_Compare)
{
	return static_cast<uint32_t>(_InterlockedCompareExchange(
															reinterpret_cast<volatile unsigned long*>( o_Dest ),	// No casting should occur for any of these
															static_cast<unsigned long>( i_Exchange ),				// uint32_t is the equivalent of unsigned long.
															static_cast<unsigned long>( i_Compare ) ) );
}