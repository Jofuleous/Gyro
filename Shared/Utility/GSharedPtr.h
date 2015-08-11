#ifndef __UTILITY_GSHAREDPTR_
#define __UTILITY_GSHAREDPTR_

//============================================================================================
// GSharedPtr.h
// Author:	Jon Futch
// Created: Unknown 2012-2014
//
// A basic shared/smart pointer implementation.  
// TODO: Figure out why I chose to use a pool for the ref counts?  Was this an assignment requirement
// or something?
//============================================================================================

#include "GSharedPtrBase.h"
#include "Debugging.h"

template<typename T>
class GSharedPtr : public GSharedPtrBase
{

public:
	GSharedPtr() : m_Ptr( 0 ), m_Refs( (int*)s_refPool->Allocate(sizeof(int)) )
	{
		(*m_Refs) = 1;
			
		#ifdef _DEBUG
			GSharedPtrBase::Created++;
		#endif
	}

	explicit GSharedPtr( T* i_Ptr ) : m_Ptr( i_Ptr ), m_Refs( (int*)s_refPool->Allocate(sizeof(int)) )
	{
		//until i figure out a more clever way.
		(*m_Refs) = 1;

		#ifdef _DEBUG
			GSharedPtrBase::Created++;
		#endif
	}

	GSharedPtr( const GSharedPtr& i_other )
	{
		assert( i_other.m_Refs );
		if( i_other.m_Ptr != m_Ptr )
		{
			m_Ptr = i_other.m_Ptr;
			m_Refs = i_other.m_Refs;
			(*m_Refs)++;
		}
		//DEBUG_PRINT_2("Copy constructor called on %p.  Ref count = %d \n", m_Ptr, (*m_Refs) );	
	}

	~GSharedPtr()
	{
		Release();
		m_Ptr = NULL;
	}

	GSharedPtr&		operator=( const GSharedPtr& i_other );
	T&				operator*();
	T*				operator->();
	bool			operator==( const GSharedPtr& i_other ) const;
					operator bool() const;

private:
	T*				m_Ptr;
	int*			m_Refs;
	void			Release();

};

template<typename T>
GSharedPtr<T>& GSharedPtr<T>::operator=( const GSharedPtr& i_other )
{
	if( m_Ptr != i_other.m_Ptr )
	{
		Release();
		assert( i_other.m_Refs );

		m_Refs = i_other.m_Refs;
		m_Ptr = i_other.m_Ptr;
		(*m_Refs)++;
	}

	return *this;
}

template<typename T>
T& GSharedPtr<T>::operator*() 
{
	assert( m_Ptr );
		
	return *m_Ptr;
}

template<typename T>
T* GSharedPtr<T>::operator->()
{
	return m_Ptr;
}

template<typename T>
bool GSharedPtr<T>::operator==( const GSharedPtr& i_other ) const
{
	if( m_Ptr == i_other.m_Ptr )
		return true;

	return false;
}

template<typename T>
GSharedPtr<T>::operator bool() const
{
	if( m_Ptr != NULL )
		return true;
	return false;
}


template<typename T>
void GSharedPtr<T>::Release( )
{
	//DEBUG_PRINT_2("Destructor.  Attempting release on %p \n", m_Ptr);

	assert( m_Refs );
	assert( *m_Refs > 0 );
		
	if( --(*m_Refs) == 0 )
	{
		s_refPool->Free( m_Refs );

		m_Refs = 0;
			
		if( m_Ptr )
		{
		#ifdef _DEBUG
			GSharedPtrBase::Deleted++;
		#endif
			delete m_Ptr;
		}

		m_Ptr = 0;
	}
}

#endif