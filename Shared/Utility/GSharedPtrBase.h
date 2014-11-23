#ifndef __SHAREDPTR_BASE_
#define __SHAREDPTR_BASE_

#include "Memory/GMemoryPool.h"

class GSharedPtrBase
{
public:
	#ifdef _DEBUG
	static int			Created;
	static int			Deleted;
	#endif
	static GMemoryPool* s_refPool;

	GSharedPtrBase() {};
	~GSharedPtrBase() {}; //this should never be called...
};

#endif