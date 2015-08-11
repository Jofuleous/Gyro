#ifndef _DEBUGGING_H_
#define _DEBUGGING_H_

void ConsolePrint( short verbosity, const char* fmt, ... );

//I chose to make each verbosity level their own macro like I had discussed in class.
//I'm assuming that this is okay...

//Lowest verbosity level
#if defined(_DEBUG)
	#define DEBUG_PRINT( fmt, ... ) ConsolePrint( 0, fmt, __VA_ARGS__ );
#else
	#define DEBUG_PRINT ((void)0);
#endif

#if defined(_DEBUG) && DEBUG_LEVEL > 0
	#define DEBUG_PRINT_1( fmt, ... ) ConsolePrint( 1, fmt, __VA_ARGS__ );
#else
	#define DEBUG_PRINT_1 ((void)0);
#endif

#if defined(_DEBUG) && DEBUG_LEVEL > 1
	#define DEBUG_PRINT_2( fmt, ... ) ConsolePrint( 2, fmt, __VA_ARGS__ );
#else
	#define DEBUG_PRINT_2 ((void)0);
#endif

#endif