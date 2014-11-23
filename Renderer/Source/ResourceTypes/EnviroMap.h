#ifndef __ENVIROMAP_H_
#define __ENVIROMAP_H_

#include "../ResourceManager.h"
#include "../ResourceMaster.h"
#include "../ResourceLoader.h"
#include "../ResourcePtr.h"
#include <d3d9.h>

class EnviroMapReference : public ResourceMaster<LPDIRECT3DCUBETEXTURE9>
{
public:
	~EnviroMapReference()
	{

	}
};

class EnviroMapLoader : public ResourceLoader<EnviroMapReference>
{
public:
	EnviroMapLoader(){}

	virtual EnviroMapReference* Load( const char* i_filename );
};

#endif