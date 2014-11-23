#include "GSharedPtrBase.h"

GMemoryPool* GSharedPtrBase::s_refPool = GMemoryPool::Create( sizeof( int ), /*was 200...*/2000 );

#ifdef _DEBUG
	int GSharedPtrBase::Created = 0;
	int GSharedPtrBase::Deleted = 0;
#endif