#ifndef __KERNEL_THREAD_GLOCK_H_
#define __KERNEL_THREAD_GLOCK_H_

//===========================================================================================
// GProfiling.h
// Author:	Jon Futch
// Created:	Unknown 2012-2014
//
// A thread lock class.  This was originally used when writing my ray tracer for my raytracing class.
// TODO: CRITICAL_SECTION is a Windows construct.  Figure out if this allows for rescheduling that we may not want.
// I believe this is the equivalent to other OSs "mutex" functions, which do cause rescheduling.
//===========================================================================================

#include <windows.h>
#include <iostream>

class GLock
{
private:

	CRITICAL_SECTION m_critsec;
public:
	GLock()
	{
		InitializeCriticalSection(&m_critsec);
	}
	~GLock()
	{
		DeleteCriticalSection(&m_critsec);
	}
	void Lock()
	{
		EnterCriticalSection(&m_critsec);
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_critsec);
	}
};

#endif