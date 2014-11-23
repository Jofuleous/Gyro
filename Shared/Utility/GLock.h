#ifndef __GLOCK_H_
#define __GLOCK_H_

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