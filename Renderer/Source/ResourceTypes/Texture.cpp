#include "Texture.h"
#include "../RenderDevices.h"
#include <assert.h>

TextureReference* TextureLoader::Load( const char* i_filename )
{
		TextureReference* ref = new TextureReference();

		char* filepath = (char*) malloc( 128 );
		sprintf( filepath, "data/Textures/%s", i_filename );

		HRESULT result = D3DXCreateTextureFromFile( g_RenderDevices.GetDevice(), filepath, &(ref->rm_Reference) );
		if( !SUCCEEDED( result ) )
			assert( 0 );

		free( filepath );

		return ref;
}