#ifndef _GHASHMAP_H_
#define _GHASHMAP_H_

//==================================================
// GHashMap.h
// Author:	Jon Futch
// Created:	Unknown 2012-2014
//
// A hashmap container.  Currently uses chaining for collision resolution.
// Potentially switch to linear/quadratic probing if there's any performance degradation.
//==================================================

#include "Platform/Target.h"

#ifdef _DEBUG
#include "Utility/Debugging.h"
#endif

// T will typically be u32 or u16.  D could be anything.
template<typename T, typename D>
class GHashNode
{
public:
						GHashNode( T i_key, D i_data, GHashNode<T,D>* i_next )
						{
							m_key = i_key;
							m_data = i_data;
							m_next = i_next;
						}

	T					m_key;
	D					m_data;
	GHashNode<T, D>*	m_next;
};

template<typename T, typename D>
class GHashMap
{
public:

	GHashMap( u32 i_tableSize )
	{
		m_tableSize = i_tableSize;
		m_count = 0;
		m_table = new GHashNode<T,D>*[i_tableSize];

		for( u32 i = 0; i < i_tableSize; i++ )
			m_table[i] = 0;
	}

	GHashMap( )
	{
		// default value.
		m_tableSize = 20;
		m_count = 0;
		m_table = new GHashNode<T,D>*[m_tableSize];

		for( u32 i = 0; i < m_tableSize; i++ )
			m_table[i] = 0;
	}

	~GHashMap( );

						// generates an index for the key.
	u32					HashFunc( const T i_key );

						// inserts data into the map
	void				Set( const T i_key, D i_data );
	void				Remove( const T i_key );
	void				Clear( void );
	GHashNode<T,D>*		operator[]( const T i_key );
	GHashNode<T,D>*		Index( u32 i_index ) { return m_table[ i_index ]; }
	u32					Size( void ) { return m_tableSize; }
	u32					Count( void ) { return m_count; }

#ifdef _DEBUG
	void				PrintStats( bool i_detailed );
#else
	void				PrintStats( bool i_detailed ) {}
#endif

private:
	u32					m_count;
	u32					m_tableSize;
	GHashNode<T,D>**	m_table;



public:
	class iterator
	{
		void			operator++( void );
						operator bool() const;
		GHashNode<T,D>*	Value();

	private:
		iterator( GHashMap<T,D>* i_map) : m_map( i_map ), m_index( -1 ), m_chainNode( 0 )
		{ 
			this++;
		}
		GHashMap<T,D>*	m_map;
		u32				m_index;
		GHashNode<T,D>*	m_chainNode;
	};
};

// this seems bad, but it will work for now...
template<typename T, typename D>
GHashMap<T,D>::~GHashMap()
{
	delete m_table;
}

template<typename T, typename D>
void GHashMap<T,D>::Clear( void )
{
	for( unsigned i = 0; i < m_tableSize; i++ )
	{
		if( m_table[i] != 0 )
		{
			GHashNode<T,D>* node = m_table[i];
			while( node != 0 )
			{
				GHashNode<T,D>* next = node->m_next;
				delete node;
				node = next;
			}
		}
		m_table[i] = 0;
	}

	m_count = 0;
}

template<typename T, typename D>
u32 GHashMap<T,D>::HashFunc( const T i_key )
{
	return i_key % m_tableSize;
}

template<typename T, typename D>
GHashNode<T,D>* GHashMap<T,D>::operator[]( const T i_key )
{
	u32 index = HashFunc( i_key );
	GHashNode<T,D>* current = m_table[index];

	// for now, just loop through.  it would be cool to be able to do something like a binary search.
	// that's hard to do with a singlely linked list, though...
	while( current != 0 )
	{
		if( current->m_key == i_key )
			return current;
		current = current->m_next;
	}

	return 0;
}

template<typename T, typename D>
void GHashMap<T,D>::Set( const T i_key, D i_data )
{
	u32 index = HashFunc( i_key );
	
	// insert.
	GHashNode<T,D>* current = m_table[index];
	if( current == 0 )
	{
		current = 0;
		GHashNode<T,D>* node = new GHashNode<T,D>( i_key, i_data, current ); // we should...make this a vector...
		m_table[index] = node;
		m_count++;
		return;
	}

	// for now, just loop through.  it would be cool to be able to do something like a binary search.
	// that's hard to do with a singlely linked list, though...
	GHashNode<T,D>* prev;
	while( current != 0 )
	{
		if( current->m_key == i_key )
		{
			current->m_data = i_data;
			return;
		}
		prev = current;
		current = current->m_next;
	}

	// this didn't exist, so we need to create a new node for it.
	GHashNode<T,D>* node = new GHashNode<T,D>( i_key, i_data, current );
	prev->m_next = node;
	m_count++;

	return;

}

template<typename T, typename D>
void GHashMap<T,D>::Remove( const T i_key )
{
	u32 index = HashFunc( i_key );
	
	// insert.
	GHashNode<T,D>* current = m_table[index];
	if( current == NULL )
		return;
	else if( current->m_key == i_key )
	{
		m_table[index] = NULL;
		m_count--;
		return;
	}
	GHashNode<T,D>* prev = current;
	current = current->m_next;

	while( current != NULL )
	{
		if( current->m_key == i_key )
		{
			prev->m_next = current->m_next;
			m_count--;
			return;
		}
		prev = current;
		current = current->m_next;
	}
}

#ifdef _DEBUG
template<typename T, typename D>
void GHashMap<T,D>::PrintStats( bool i_detailed )
{
	int maxChainLinks = 0;
	int averageChainLinks = 0;
	int minChainLinks = 0;

	for( unsigned i = 0; i < m_tableSize; i++ )
	{
		if( m_table[i] != 0 )
		{
			GHashNode<T,D>* node = m_table[i];
			int chainLinks = 0;
			while( node != 0 )
			{
				GHashNode<T,D>* next = node->m_next;
				node = next;
				chainLinks++;
			}

			if( i_detailed )
				DEBUG_PRINT( "Bucket %d: %d nodes", i, chainLinks );
			if( chainLinks > maxChainLinks )
				maxChainLinks = chainLinks;
			if( chainLinks < minChainLinks )
				minChainLinks = chainLinks;

			averageChainLinks += chainLinks;
			chainLinks = 0;
		}
		m_table[i] = 0;
	}

	averageChainLinks /= m_tableSize;
	DEBUG_PRINT( "Average ChainLinks: %d", averageChainLinks );
	DEBUG_PRINT( "Maximum ChainLinks: %d", maxChainLinks );
	DEBUG_PRINT( "Minimum ChainLinks: %d", minChainLinks );
}
#endif

template<typename T, typename D>
GHashNode<T,D>* GHashMap<T,D>::iterator::Value( void )
{
	return m_chainNode;
}

template<typename T, typename D>
void GHashMap<T,D>::iterator::operator++( void )
{
	if( m_chainNode && m_chainNode->m_next != 0 )
		m_chainNode = m_chainNode->m_next;

	if( m_index >= m_tableSize )
		m_chainNode = 0;

	for( m_index++; m_index < m_tableSize; m_index++ )
	{
		if( m_table[m_index] != 0 )
		{
			GHashNode<T,D>* node = m_table[i];
			while( node != 0 )
			{
				m_chainNode = node->m_next;
				break;
			}
		}
	}
}

template<typename T, typename D>
GHashMap<T,D>::iterator::operator bool() const
{
	return m_chainNode;
}

#endif