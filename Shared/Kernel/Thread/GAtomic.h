#ifndef __KERNEL_THREAD_GATOMIC_H_
#define __KERNEL_THREAD_GATOMIC_H_

//===================================================================
//	GAtomic.h
//	Wrapper on OS specific atomic synchronization functions.
//	Most platforms will have their own definitions of these functions...
//===================================================================

#include <stdint.h>

class GAtomic
{
public:
	static uint32_t CompareExchange( volatile uint32_t& o_Dest, uint32_t i_Exchange, uint32_t i_Compare );
};

#include "GAtomic.inl"

#endif