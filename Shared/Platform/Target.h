#ifndef	_TARGET_H_
#define	_TARGET_H_

//This is force included for all builds...

#ifdef _WIN32
#include "Target.Win32.h"
#elif _XBOX360
#include "Target.Xbox360.h"
#endif
#endif