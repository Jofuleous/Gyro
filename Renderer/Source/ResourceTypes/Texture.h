#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include "../ResourceManager.h"
#include "../ResourceMaster.h"
#include "../ResourceLoader.h"
#include "../ResourcePtr.h"
#include <d3d9.h>

class TextureReference : public ResourceMaster<LPDIRECT3DTEXTURE9>
{
};

class TextureLoader : public ResourceLoader<TextureReference>
{
public:
	TextureLoader(){}

	virtual TextureReference* Load( const char* i_filename );
};

#endif