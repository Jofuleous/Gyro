#include "EnviroMap.h"
#include "../RenderDevices.h"
#include <assert.h>

EnviroMapReference* EnviroMapLoader::Load( const char* i_filename )
{
		EnviroMapReference* ref = new EnviroMapReference();

		char* filepath = (char*) malloc( 128 );
		sprintf( filepath, "data/EnvironmentMaps/%s", i_filename );

		HRESULT result = D3DXCreateCubeTextureFromFile( g_RenderDevices.GetDevice(), filepath, &(ref->rm_Reference) );
		if( !SUCCEEDED( result ) )
			assert( 0 );

		free( filepath );

		return ref;
}