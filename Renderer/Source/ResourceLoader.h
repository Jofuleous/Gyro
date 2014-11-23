#ifndef __RESOURCE_LOADER_
#define __RESOURCE_LOADER_

#include "ResourceMaster.h"

template<typename T>
class ResourceLoader
{
public:
	virtual T* Load( const char* i_filename ) { return 0; };
};

#endif