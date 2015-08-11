#ifndef __UTILITY_GSHAREDPTRBASE_H_
#define __UTILITY_GSHAREDPTRBASE_H_

//============================================================================================
// GSharedPtrBase.h
// Author:	Jon Futch
// Created:	Unknown 2012-2014
//============================================================================================

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