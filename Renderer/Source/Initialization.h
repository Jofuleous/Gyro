/*
	This file should be #included before anything else in the project
*/

// Windows
//========

// Exclude extraneous Windows stuff
#define WIN32_LEAN_AND_MEAN
// Prevent Windows from creating min/max macros
#define NOMINMAX

// Initialize Windows
#include <Windows.h>

#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

#include <cstdint>
#define UINT8 uint8_t
typedef unsigned __int8			u8;
typedef unsigned __int16		u16;
typedef unsigned __int32		u32;
typedef unsigned __int64		u64;

typedef u64 SortKey;

// Logging
//========

#include "Logging.h"

// User Settings
//==============

//#include "UserSettings.h"
