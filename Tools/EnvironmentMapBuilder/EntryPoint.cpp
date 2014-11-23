/*
	The main() function in this file is the entry point of this program
*/

// Header Files
//=============

#include "EnvironmentMapBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	cs6963::EnvironmentMapBuilder envMapBuilder;
	return envMapBuilder.Build( i_argumentCount, i_arguments ) ? 0 : -1;
}
