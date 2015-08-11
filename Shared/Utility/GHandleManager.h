#ifndef __GHANDLE_MANAGER_H_
#define __GHANDLE_MANAGER_H_

#include "Containers/GArray.h"
#include <assert.h>

template< u32 TypeBits, u32 CounterBits, u32 IndexBits > 
class GHandle
{
public:
	GHandle() : m_index( 0 ), m_counter( 0 ), m_type( 0 )
	{
		assert( sizeof( this ) != 32 ); // handle was not 32 bits...
	}

	GHandle( u32 index, u32 counter, u32 type )
		: m_index( index ), m_counter( counter ), m_type( type )
	{}

	inline operator u32() const
	{
		return (m_type << 32 - TypeBits) | (m_counter << 32 - TypeBits - CounterBits) | m_index;
	}
	
	u32 m_index : IndexBits;
	u32 m_counter : CounterBits;
	u32 m_type : TypeBits;
};

template< typename DataType, u32 TypeBits, u32 CounterBits, u32 IndexBits >
class GHandleManager
{
public:
	typedef GHandle< TypeBits, CounterBits, IndexBits > TypeHandle;

	template< typename DataType >
	class GHandleEntry
	{
	public:
		GHandleEntry( ) : m_counter( 0 ), m_nextFree( 0 ), m_active( 0 ), m_endOfList( 0 ) { }
		GHandleEntry( u32 i_nextFree ) : m_counter( 0 ), m_nextFree( i_nextFree ), m_active( 0 ), m_endOfList( 0 ) { }

		DataType*			m_data;
		u32					m_counter	: CounterBits;
		u32					m_nextFree	: IndexBits;
		u32					m_active	: 1;
		u32					m_endOfList	: 1;				
	};

	GHandleManager( ) : m_nextFree( 0 )
	{
		int i = 0;
		for( ; i < 1 << IndexBits ; i++ )
			m_entries[ i ] = GHandleEntry<DataType>( i + 1 ); // maybe not do a copy.  in release, is the constructor called?

		m_entries[ i ].m_nextFree = 0;
		m_entries[ i ].m_endOfList = true;
	}

	DataType*										GetData( TypeHandle i_handle );
	GHandle< TypeBits, CounterBits, IndexBits >		AddData( DataType*	i_data );
	void											RemoveData( TypeHandle i_handle );

	u32				m_nextFree;

	GHandleEntry< DataType > m_entries[ 1 << IndexBits ]; // 4096 if 12, right?  been a while.
};

template< typename DataType, u32 TypeBits, u32 CounterBits, u32 IndexBits >
DataType* GHandleManager<DataType, TypeBits, CounterBits, IndexBits>::GetData( TypeHandle i_handle )
{
	assert( i_handle.m_index < 1 << IndexBits );
	if( m_entries[ i_handle.m_index ].m_counter == i_handle.m_counter && m_entries[ i_handle.m_index ].m_active )
		return m_entries[ i_handle.m_index ].m_data;
	else
		return 0;
}

template< typename DataType, u32 TypeBits, u32 CounterBits, u32 IndexBits >
GHandle< TypeBits, CounterBits, IndexBits > GHandleManager< DataType, TypeBits, CounterBits, IndexBits>::AddData( DataType* i_data )
{
	//assert( !m_enties[m_nextFree].m_endOfList ); // end of list reached.
	m_entries[ m_nextFree ].m_data = i_data;
	m_entries[ m_nextFree ].m_counter++;
	m_entries[ m_nextFree ].m_active = true;
	u32 index = m_nextFree;
	m_nextFree = m_entries[ m_nextFree ].m_nextFree;

	return GHandle< TypeBits, CounterBits, IndexBits >( index, m_entries[ index ].m_counter, 0 );
}

template< typename DataType, u32 TypeBits, u32 CounterBits, u32 IndexBits >
void GHandleManager< DataType, TypeBits, CounterBits, IndexBits>::RemoveData( TypeHandle i_handle )
{
	assert( i_handle.m_index < 1 << IndexBits );
	m_entries[ i_handle.m_index ].m_active = false;
	u32 nextFree = m_nextFree;
	m_nextFree = i_handle.m_index;
	m_entries[ i_handle.m_index ].m_nextFree = nextFree;
}


#define CREATE_HANDLE_MANAGER( DataType, Name, TypeBits, CounterBits, IndexBits )	\
	typedef GHandle< TypeBits, CounterBits, IndexBits > Name##Handle; \
	typedef GHandleManager< DataType, TypeBits, CounterBits, IndexBits > Name##HandleManager;
#endif