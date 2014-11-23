#ifndef _ALIGN_VECTOR3_H_
#define _ALIGN_VECTOR3_H_
#include "GVector3.h"

namespace Gyro
{

CACHE_ALIGNED class AlignVector3 : GVector3
{

public:
	//constructors
								AlignVector3( void ) {}
	inline						AlignVector3( float i_x, float i_y, float i_z );
	//copy
	inline						AlignVector3(const AlignVector3& i_v);

	
	inline void*				operator new( size_t size );
	inline void					operator delete( void* addr );
	inline void*				operator new[]( size_t size );
	inline void					operator delete[]( void* addr );
	inline void*				operator new (size_t sz, void* v);
	inline void					operator delete(void* pMem, void* tag);
	inline void*				operator new[] (size_t sz, void* v);
	inline void					operator delete[]( void* pMem, void* tag );

};

#include "AlignVector3.inl"

}


#endif