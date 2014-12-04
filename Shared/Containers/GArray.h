#ifndef __GARRAY_H_
#define __GARRAY_H_

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

//#define _PROFILE
#ifdef _PROFILE
#include <vector>
#include "Profiling/GProfiling.h"
#endif

template< typename T >
class GArray
{
public:
	GArray( u32 i_size = 1 ) : m_size( i_size ), m_count( 0 ), m_elements( new T[i_size] ) { }
	~GArray()
	{
		delete[] m_elements;
	}
	GArray(const GArray& i_other)
	{
		Copy(i_other);
	}

	void	Push( const T& i_value );
	u32		Count() const { return m_count; }
	u32		Size() const { return m_size; }

	// Make a deep copy.
	void	Copy( const GArray<T>& i_other );
	void	SetCount( int i_count );

	// These are not portable.  Replace later...
	void	Serialize( FILE* io_file );
	void	DeSerialize(FILE * i_file);

	void	Resize( u32 i_size )
	{
		if( i_size > m_size )
		{
			T* newElements = new T[ i_size ];

			// Copy data...
			for (u32 i = 0; i < m_size; i++)
				newElements[i] = m_elements[i];

			delete[] m_elements;
			m_elements = newElements;
			m_size = i_size;
		}
		else if( i_size == m_size ) {}
		else if( i_size < m_size )
		{
			assert(0);
			delete[] &m_elements[ m_size - (m_size - i_size )]; // not sure that this is legal...
			if( i_size < m_count )
				m_count = i_size;
		}
	}

	void	Clear()
	{
		// maybe clear out?
		if (m_size > 0)
			m_count = 0;
	}

	void	Remove( u32 i_index )
	{
		assert( i_index < m_count );
		m_elements[ i_index ] = m_elements[ m_count - 1 ];
		m_count--;
	}

	T&			operator[]( const u32 i_index ) const;
	GArray<T>&	operator=( const GArray<T>& i_other )
	{
		Copy( i_other );
		return *this;
	}

#ifdef _PROFILE
	static void Profile( void );
#else if
	static void Profile( void ) {}
#endif

private: 

	T*	m_elements;
	u32 m_count;
	u32 m_size;
};

template<typename T>
T& GArray<T>::operator[]( const u32 i_index ) const
{
	assert( i_index < m_size );
	return m_elements[ i_index ];
}

template<typename T>
void GArray<T>::Push( const T& i_value )
{
	if( m_count >= m_size )
	{
		T* newElements = new T[ m_size * 2 + 1 ];
		assert( newElements );

		// Copy data...
		for (u32 i = 0; i < m_size; i++)
			newElements[i] = m_elements[i];

		delete[] m_elements;
		m_elements = newElements;
		m_size = m_size * 2 + 1;
	}

	m_elements[m_count] = i_value;
	m_count++;
}

// Create a deep copy.
template<typename T>
void GArray<T>::Copy( const GArray<T>& i_other )
{
	if (i_other.Size() != Size())
	{
		delete[] m_elements;
		m_size = i_other.Size();
		m_elements = new T[m_size];
	}
	m_count = i_other.Count();
	for (u32 i = 0; i < m_count; i++)
		m_elements[i] = i_other.m_elements[i];
}

// Only use this if you know what you are doing.
template<typename T>
void GArray<T>::SetCount( int i_count)
{
	m_count = i_count;
}

template<typename T>
void GArray<T>::Serialize( FILE* io_file )
{
	fwrite(&m_size, sizeof(u32), 1, io_file);
	fwrite(&m_count, sizeof(u32), 1, io_file);
	fwrite(m_elements, sizeof(T), m_count, io_file);
}

template<typename T>
void GArray<T>::DeSerialize(FILE * i_file)
{
	u32 size;
	fread(&size, sizeof(u32), 1, i_file );
	Resize(size);
	int success = fread(&m_count, sizeof(u32), 1, i_file);
	success = fread(m_elements, sizeof(T), m_count, i_file);
}

#ifdef _PROFILE
template<typename T>
void GArray<T>::Profile( void )
{
	GArray<int> nums;
	PROFILE_SCOPE_BEGIN("GArray<int> Add");
	for(int i = 0; i < 100000; i++ )
		nums.Push( i );
	PROFILE_SCOPE_END();
	
	std::vector<int> vNums;
	PROFILE_SCOPE_BEGIN( "std::vector<int> Add" );
	for(int i = 0; i < 100000; i++ )
		vNums.push_back( i );
	PROFILE_SCOPE_END();

	int numsRaw[100000];
	PROFILE_SCOPE_BEGIN( "int[] Add" );
	for(int i = 0; i < 100000; i++ )
		numsRaw[i] = i;
	PROFILE_SCOPE_END();

	PROFILE_SCOPE_BEGIN("GArray<int> Iterate");
	for(int i = 0; i < 100000; i++ )
		int q = nums[ i ];
	PROFILE_SCOPE_END();
	
	PROFILE_SCOPE_BEGIN( "std::vector<int> Iterate" );
	for(int i = 0; i < 100000; i++ )
		int z = vNums[i];
	PROFILE_SCOPE_END();

	PROFILE_SCOPE_BEGIN( "int[] Iterate" );
	for(int i = 0; i < 100000; i++ )
		int q = numsRaw[i];
	PROFILE_SCOPE_END();

}
#endif

#endif