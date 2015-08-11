#ifndef	__PLATFORM_TARGET_H_
#define	__PLATFORM_TARGET_H_

//===========================================================
// Target.h
// Creator: Jon Futch
// Created: Unknown 2012-2014
//
// Defines platform-specific #defines and other misc data.
// This file is force included for every project...
// TODO: Move this to the "Kernel" project.  Kill the "Platform" project?
// I'll have to think about this a bit more.
//===========================================================

//This is force included for all builds...
#include <stdint.h>

#ifdef _WIN32
#include "Target.Win32.h"
#elif _XBOX360
#include "Target.Xbox360.h"
#endif
#endif