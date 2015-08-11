#ifndef _GMEMORY_POOL_H_
#define _GMEMORY_POOL_H_

//===================================================================================================================
// GMemoryPool.h
// Author: Jon Futch
// Created: Fall 2012
//
// A fix-sized memory allocator.
//
// TODO: This makes no optimizations in terms of storing of the "next free" item.  It's really kind of stupid.
// We should get rid of using the "BitField" and store the "next free" item in a header on each memory pool entry...
// Much like what you would do with a memory manager.
// The only way it was written like this was to fulfill an engineering assignment.
//===================================================================================================================

#include "Platform/Target.h"

class GBitField;

//may align this later.
class GMemoryPool
{
public:
	~GMemoryPool();
	static GMemoryPool*			Create( u32 i_itemSize, u32 i_numItems );
	inline void*				Allocate( u32 size );
	inline void					Free( void* i_addr );
	inline u32					InUseCount( void ) const { return m_inUseCount; }
	inline u32					NumOfItems( void ) const { return m_numItems; }
	inline bool					VerifyItem( void* addr ) const;
	inline void					Clear( void );

	template<typename T>
	static inline void			GMemoryPool_UnitTest( void );

private:
	GMemoryPool( GBitField* i_freeList, unsigned char* i_memoryBlock, u32 i_itemSize, u32 i_numItems );
	GMemoryPool( const GMemoryPool& memoryPool );


	u32							m_inUseCount;
	const u32					m_itemSize;
	const u32					m_numItems;
	GBitField* const			m_freeList;
	unsigned char* const		m_memoryBlock;

};

#include "GMemoryPool.inl"

#endif