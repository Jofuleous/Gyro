#include "ComponentManager.h"
#include <stdio.h>
#include <string.h>

// there shoud be a way of cleanly registering engine components.  not sure where the best place
// for this would be...
#include "Components/GTriggerComponent.h"
u32 ComponentManager::m_numCompHandles = 0;

u32 ComponentManager::GetNextTypeId( ) { return m_numCompHandles++; }

ComponentManager::ComponentManager()
{
}

void ComponentManager::Initialize()
{
	//RegisterComponent<GTriggerComponent>();
}

void ComponentManager::Shutdown()
{
	std::map<int, IActorComponent*, compare>::iterator it = m_componentMap.begin();

	while( it != m_componentMap.end() )
	{
		delete it->second;
		it++;
	}

	m_componentMap.clear();
}

void ComponentManager::RegisterComponent( u32 i_typeId, const GHashedString i_typeName, IActorComponent* i_factory )
{

	//for now, let's not worry about multiples...
	m_componentMap[ i_typeName.Get() ]  = i_factory;
}

IActorComponent* ComponentManager::CreateComponent( const char* i_typeName )
{
	//could have passed in hash created at compile time...but meh
	int hash = GHashedString::Hash( i_typeName, strlen( i_typeName ) );
	std::map<int,IActorComponent*,compare>::iterator it = m_componentMap.find( hash );

	if( it != m_componentMap.end() )
	{
		IActorComponent* component = it->second->Create();
		return component;
	}

	return NULL;
}
