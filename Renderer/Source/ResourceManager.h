#ifndef __RESOURCE_MANAGER_
#define __RESOURCE_MANAGER_

#include "ResourceMaster.h"
#include "ResourcePtr.h"
#include "ResourceLoader.h"
#include <map>
#include <stdio.h>
#include <assert.h>

//I know you're looking at this and being a h8r.  Please don't h8.  I just like gamez. :(

class compare
{
public:
	bool operator()(const char* lhs, const char* rhs)
	{
		return std::lexicographical_compare( lhs, lhs + strlen(lhs), rhs, rhs + strlen(rhs));
	}
};


template<typename T>
class ResourceManager
{
	friend ResourcePtr<T>;

public:

	static ResourcePtr<T>		Load( const char* i_filename, ResourceLoader<T>* i_loader, bool i_keepLoaded )
	{
		std::map<const char*,T*,compare>::iterator it = m_resourceMap.find( i_filename );

		T* master;

		if( it != m_resourceMap.end() && (strcmp( it->second->rm_Filename, i_filename) == 0) )
		{
			master = it->second;
			// this is getting incremented when we pass it to the resourceptr...
			//master->rm_InUse++;

		}
		else
		{
			master = i_loader->Load( i_filename );
			assert( master );

			//fill name
			char* name = (char*) malloc( 128 );
			sprintf_s(name, 128, i_filename);
			master->rm_Filename = name;

			//fill in use
			master->rm_InUse = 0;
			//fill in keep loaded
			master->rm_KeepLoaded = i_keepLoaded;



			m_resourceMap[name] = master;

			m_nextId++;
		}

		ResourcePtr<T> ptr( master );
		return ptr;
	}

	static unsigned __int16						m_nextId; //todo: turn back to u16
	static std::map<const char*, T*, compare>	m_resourceMap;

private:
						ResourceManager()
						{
							
						}
						~ResourceManager()
						{

						}
};

template<typename T>
std::map<const char*, T*, compare>	ResourceManager<T>::m_resourceMap;

template<typename T>
unsigned __int16					ResourceManager<T>::m_nextId; //todo: turn back to u16

//template<typename Q>
//typedef Singleton<ResourceManager> g_ResourceManager<T>; 

#endif