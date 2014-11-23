#ifndef __RESOURCE_PTR_
#define __RESOURCE_PTR_

#include "ResourcePtr.h"

template<typename T>
class ResourcePtr
{
public:


	ResourcePtr( ) : m_resourceMaster( 0 )
	{

	}

	explicit ResourcePtr( T* i_master )
	{
		i_master->rm_InUse++;
		m_resourceMaster = i_master;
	}

	~ResourcePtr()
	{
		Release();
	}

	ResourcePtr( const ResourcePtr& i_other )
	{
		m_resourceMaster = i_other.m_resourceMaster;
		if(m_resourceMaster)
			m_resourceMaster->rm_InUse++;
	}

	ResourcePtr&				operator=( const ResourcePtr& i_other );

	T&							operator*();
	T*							operator->();
	bool						operator==( const ResourcePtr& i_other ) const;
	//							operator bool();

private:
	T*							m_resourceMaster;
	void						Release();


};

template<typename T>
ResourcePtr<T>& ResourcePtr<T>::operator=( const ResourcePtr& i_other )
{
	if( m_resourceMaster != i_other.m_resourceMaster )
	{
		Release();
		m_resourceMaster = i_other.m_resourceMaster;
		m_resourceMaster->rm_InUse++;
	}

	return *this;
}

template<typename T>
T& ResourcePtr<T>::operator*() 
{
	assert( m_resourceMaster );
		
	return *m_resourceMaster;
}

template<typename T>
T* ResourcePtr<T>::operator->()
{
	return m_resourceMaster;
}

template<typename T>
bool ResourcePtr<T>::operator==( const ResourcePtr& i_other ) const
{
	if( m_resourceMaster == i_other.m_resourceMaster )
		return true;

	return false;
}
/*
template<typename T>
ResourcePtr<T>::operator bool() const
{
	if( m_resourceMaster != NULL )
		return true;
	return false;
}
*/

template<typename T>
void ResourcePtr<T>::Release( )
{
	//DEBUG_PRINT_2("Destructor.  Attempting release on %p \n", m_Ptr);
	
	if( m_resourceMaster )
	{
		m_resourceMaster->rm_InUse--;
		if( m_resourceMaster->rm_InUse <= 0 )
		{
			if( m_resourceMaster->rm_KeepLoaded )
			{
				//do nothing
			}
			else
			{
				//remove from the mapping.
				ResourceManager<T>::m_resourceMap.erase( m_resourceMaster->rm_Filename );
				free( m_resourceMaster->rm_Filename );
				delete m_resourceMaster;
				m_resourceMaster = 0;
			}
		}
	}
}

#endif