#include <stdio.h>
#include <windows.h>
#include "Debugging.h"

void ConsolePrint( short verbosity, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );

	//Max 256 chars for now..
	char buffer[256];
		
	vsprintf_s( buffer, fmt, args );
	OutputDebugString( buffer );
	va_end( args );
}