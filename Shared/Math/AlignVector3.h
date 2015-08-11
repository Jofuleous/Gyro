#ifndef __MATH_ALIGN_VECTOR3_H_
#define __MATH_ALIGN_VECTOR3_H_

//========================================================================================================
// AlignedVector3.h
// Author: Jon Futch 
// Created: Unknown 2012-2014
// 
// A cache aligned Vector3 class.  I'm not sure what purpose this really has except like maybe for SIMD...?
// Apparently, it's something that I made to fulfill one of my engineering assignments.
//=========================================================================================================

#include "GVector3.h"

namespace Gyro
{

CACHE_ALIGNED class AlignVector3 : GVector3
{

public:
								AlignVector3( void ) {}
	inline						AlignVector3( float i_x, float i_y, float i_z );

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