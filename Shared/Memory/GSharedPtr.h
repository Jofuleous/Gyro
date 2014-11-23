#ifndef __SHARED_PTR_
#define __SHARED_PTR_

#include "GMemoryPool.h"


template<typename T>
class GSharedPtr
{
public:
	GSharedPtr() : m_Ptr( 0 ), m_Refs( s_refPool->Allocate(sizeof(int)) )
	{
	}

	GSharedPtr( T* i_Ptr ) : m_Ptr( i_Ptr ), m_Refs( s_refPool->Allocate(sizeof(int)) )
	{
	}

	GSharedPtr( const GSharedPtr& i_other )
	{
		assert( i_other.m_Refs );
		m_Ptr = i_other.m_Ptr;
		m_Refs = i_other.m_Refs;
		(*m_Refs)++;
	}

	GSharedPtr& GSharedPtr::operator=( const GSharedPtr& i_other )
	{
		if( m_Ptr != i_other.m_Ptr )
		{
			Release();
			assert( i_other.m_Refs );

			m_Refs = i_other.m_Refs;
			m_Ptr = i_other.m_Ptr;
			(*m_Refs)++;
		}

		return this;
	}

	T & operator*() 
	{
		assert( m_Ptr );
		
		return *m_Ptr;
	}
	
	T * operator->()
	{
		return m_Ptr;
	}

	bool operator==( const GSharedPtr& i_other ) const
	{
		if( m_Ptr == i_other.m_Ptr )
			return true;

		return false;
	}

private:

	void Release()
	{
		assert( m_Refs );
		assert( *m_Refs > 0 );
		
		if( --(*m_Refs) == 0 )
		{
			s_refPool->Free( m_Refs );

			m_Refs = 0;
			
			if( m_Ptr )
				delete [] m_Ptr;

			m_Ptr = 0;
		}
	}

	T*			m_Ptr;
	int*		m_Refs;
};

#endif