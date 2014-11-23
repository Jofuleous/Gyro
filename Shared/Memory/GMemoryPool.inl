#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "GBitField.h"

void* GMemoryPool::Allocate( u32 size )
{
	assert( size == m_itemSize );
	u32 freeAddr = m_freeList->GetNextFree();
	assert( freeAddr < m_numItems );
	m_freeList->SetBitInUse( freeAddr );
	m_inUseCount++;
	return m_memoryBlock + m_itemSize * freeAddr;
}

bool GMemoryPool::VerifyItem( void* i_addr ) const
{
	u32 location = (u32) i_addr;
	u32 start = (u32) m_memoryBlock;
	u32 offset = location - start;
	if( offset < m_numItems * m_itemSize )
	{
		u32 bit = offset / m_itemSize;
		if( m_freeList->IsBitSet( bit ) )
			return true;
	}
	return false;
}

void GMemoryPool::Free( void* i_addr )
{
	//I believe we can do this so long as we're 32 bit...
	u32 location = (u32) i_addr;
	u32 start = (u32) m_memoryBlock;
	u32 offset = location - start;
	u32 bit = offset / m_itemSize;

	//indicate trying to free memory that isn't set.
	assert( m_freeList->IsBitSet( bit ) );
	m_freeList->SetBitFree( bit );
	m_inUseCount--;
}

void GMemoryPool::Clear( void )
{
	m_freeList->Clear();
	m_inUseCount = 0;
}

#ifdef _DEBUG
template<typename T>
void GMemoryPool::GMemoryPool_UnitTest( void )
{
	Gyro::GMemoryPool* pool = Gyro::GMemoryPool::Create( sizeof( T ), 200);
	T* item = (T*) pool->Allocate( sizeof( T ) );

	//first item will have an address that is cache aligned.
	assert( (u32) item % CACHE_LINE == 0 );

	T* items[15];
	for(int i = 0; i < 15; i++)
	{
		items[i] = (T*)pool->Allocate( sizeof( T ) );
	}

	for(int i = 0; i < 50; i++)
	{
		if(i < 15)
			assert(pool->VerifyItem( items[i] ));
		else
			assert(!pool->VerifyItem( items[15] + i * sizeof( T ) ));
	}

	for(int i = 0; i < 15; i++)
		pool->Free( (void*) items[i] );

	for(int i = 0; i < 15; i++)
		assert(!pool->VerifyItem( items[i] ));

	assert(pool->InUseCount() == 1);
	pool->Free( item );
	assert( pool->InUseCount() == 0);
	assert(!pool->VerifyItem( item ));

	//random freeing and allocations
	T* rdem[200];
	for(int i = 0; i < 200; i++)
	{
		rdem[i] = (T*)pool->Allocate( sizeof( T ) );
	}

	for(int i = 0; i < 50; i++)
	{
		int val = rand() % 200;
		while( !pool->VerifyItem( rdem[val] ))
		{
			val = rand() % 200;
		}
		pool->Free( rdem[val] );
	}

	//This should fill up our freed locations
	for(int i = 0; i < 50; i++)
	{
		pool->Allocate( sizeof( T ) );
	}
	//and this is bad, but our 200 locations should still be valid!! ;)
	for(int i = 0; i < 200; i++)
	{
		assert(pool->VerifyItem( rdem[i] ));
	}

	delete pool;

}
#else
template<typename T>
void GMemoryPool::GMemoryPool_UnitTest( void )
{
	return;
}
#endif