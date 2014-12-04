// Header Files
//=============

#include "AnimBuilder.h"

// Inherited Rendererlementation
//=========================

bool AnimBuilder::Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const
{
	// For now textures don't get "built";
	// instead, copy the source texture as-is to the target
	BOOL shouldFailIfDestinationFileAlreadyExists = FALSE;
	if ( CopyFile( i_fileName_source, i_fileName_target, shouldFailIfDestinationFileAlreadyExists ) == TRUE )
	{
		return true;
	}
	else
	{
		return false;
	}
}
