#include <math.h>
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include "AlignVector3.h"

AlignVector3::AlignVector3(float i_x, float i_y, float i_z):
	GVector3(i_x, i_y, i_z)
{
}

AlignVector3::AlignVector3(const AlignVector3& i_v):
	GVector3(i_v)
{
}

void* AlignVector3::operator new( size_t size )
{
	void* result = _aligned_malloc(size, CACHE_LINE);
	assert(result);

	return result;
}

void AlignVector3::operator delete( void* addr )
{
	assert((unsigned)addr % CACHE_LINE == 0);
	_aligned_free( addr );
}


void* AlignVector3::operator new[]( size_t size )
{
	void* result = _aligned_malloc( size, CACHE_LINE );
	assert( result );

	return result;
}

void AlignVector3::operator delete[]( void* addr )
{
	assert((unsigned)addr % CACHE_LINE == 0);
	_aligned_free( addr );
}

void* AlignVector3::operator new(size_t size, void* mem)
{
	assert((unsigned)mem % CACHE_LINE == 0);
	return mem;
}

void AlignVector3::operator delete(void* pMem, void* tag)
{
	_aligned_free( pMem );
}

void* AlignVector3::operator new[](size_t size, void* mem)
{
	assert((unsigned)mem % CACHE_LINE == 0);
	return mem;
}


void AlignVector3::operator delete[](void* pMem, void* tag)
{
	_aligned_free( pMem );
}
