#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <assert.h>
#include "EffectsBuilder.h"

bool cs6963::EffectsBuilder::Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const
{
	//until we get a binary format...
	BOOL exists = CopyFile( i_fileName_source, i_fileName_target, false);
	assert( exists );

	return true;
}
