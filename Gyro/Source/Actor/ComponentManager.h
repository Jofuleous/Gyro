#ifndef __COMPONENT_MANAGER_H_
#define __COMPONENT_MANAGER_H_

#include <vector>
#include <map>
#include "IActorComponent.h"
#include "Utility/GHashedString.h"
#include "Utility/Singleton.h"

#define REGISTRY_NUM_COMPONENTS 50 // bump this up as needed.  maybe figure out a better way at compile time?
#define HANDLE_TYPE_BITS 6 // 64.
#define HANDLE_COUNTER_BITS 16 // 65536
#define HANDLE_INDEX_BITS 10 // 1024 (1KB)

typedef void (*ComponentCreationFunc)( void* o_memAddr, size_t i_size );

class GRegisteredComponentType
{
#ifdef _DEBUG
	char	m_name[32];
#endif
	u32		m_type;
	ComponentCreationFunc m_creatorFunc;
};

class ComponentManager
{

	class compare
	{
	public:
		bool operator()(int lhs, int rhs)
		{
			if( lhs < rhs )
				return true;
			return false;
		}
	};

public:
													ComponentManager();
													~ComponentManager() {}
	void											Initialize();
	void											Shutdown();
	void											RegisterComponent( u32 i_typeId, const GHashedString i_typeName, IActorComponent* i_factory );
	IActorComponent*								CreateComponent( const char* i_typeName );

	std::map<int, IActorComponent*, ComponentManager::compare>		m_componentMap; // this gets killed when switching to m_registry.


	static u32										GetNextTypeId();
	static u32										m_numCompHandles;

	GHandleManager<IActorComponent, HANDLE_TYPE_BITS, HANDLE_COUNTER_BITS, HANDLE_INDEX_BITS> m_compHandles;

	GRegisteredComponentType		m_registry[ REGISTRY_NUM_COMPONENTS ];
};

typedef Singleton< ComponentManager> g_ComponentManager;


//do the fancier stuff later.  for now, just have a way of registering.
template<typename T>
void RegisterComponent(  )
{
	//force this at compile time
	T* t = new T();

	g_ComponentManager::Get().RegisterComponent( t->m_typeId, t->m_typeName, t );
}

#endif