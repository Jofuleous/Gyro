/*
	The main() function in this file is the entry point of this program
*/

// Header Files
//=============

#include "cTextureBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	cs6963::cTextureBuilder textureBuilder;
	return textureBuilder.Build( i_argumentCount, i_arguments ) ? 0 : -1;
}
