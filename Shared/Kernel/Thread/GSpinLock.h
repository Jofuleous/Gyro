#ifndef __KERNEL_THREAD_GSPINLOCK_
#define __KERNEL_THREAD_GSPINGLOCK_

//======================================================
// GSpinLock.h
// Threads wait until the lock is acquired.
// I haven't tested this too hard yet.  TODO: Test this hard.
//======================================================

#include "GAtomic.h"

class GSpinLock
{
public:
	GSpinLock() : m_Locked(true) {}
	~GSpinLock(){}
	void Acquire()
	{
		while (GAtomic::CompareExchange( m_Locked, 1, 0 ) == 1)
		{
		}
	}

	void Release()
	{
		m_Locked = 0;
	}

private:
	volatile uint32_t m_Locked;
};

#endif