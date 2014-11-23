/*
	The main() function in this file is the entry point of this program
*/

// Header Files
//=============

#include "AudioBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	AudioBuilder envMapBuilder;
	return envMapBuilder.Build( i_argumentCount, i_arguments ) ? 0 : -1;
}
