#include "GMemoryPool.h"



GMemoryPool::GMemoryPool( GBitField* i_freeList, unsigned char* i_memoryBlock, u32 i_itemSize, u32 i_numItems ): m_freeList( i_freeList ),
	m_memoryBlock( i_memoryBlock ), m_itemSize( i_itemSize ), m_numItems( i_numItems ), m_inUseCount( 0 )
{
	assert( m_itemSize != 0 );
}

GMemoryPool::~GMemoryPool()
{
	delete m_freeList;
	_aligned_free( m_memoryBlock );
}

GMemoryPool* GMemoryPool::Create( u32 i_itemSize, u32 i_numItems )
{
	unsigned char* memBlock = (unsigned char*)_aligned_malloc( i_itemSize * i_numItems, CACHE_LINE);
	assert(memBlock);

	GBitField* freeList = GBitField::Create( i_numItems );
	assert(freeList);

	GMemoryPool* pool = new GMemoryPool( freeList, memBlock, i_itemSize, i_numItems );
	assert( pool );

	return pool;
}