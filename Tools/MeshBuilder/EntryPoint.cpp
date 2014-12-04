/*
	The main() function in this file is the entry point of this program
*/

// Header Files
//=============

#include "cMeshBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	cs6963::cMeshBuilder meshBuilder;
	//char* testArgs[3];
	//testArgs[0] = "";
	//testArgs[1] = "../../Assets/Meshes/Dude.mod";
	//testArgs[2] = "../../bin/Win32/Debug/data/Meshes/Dude.bmod";
	//int testCount = 3;
	return meshBuilder.Build(i_argumentCount, i_arguments) ? 0 : -1;
}
