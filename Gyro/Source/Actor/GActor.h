#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "Math/GVector3.h"
#include "Utility/GSharedPtr.h"
#include "Math/GMatrix4.h"
#include "Utility/GHashedString.h"
#include <vector>
#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaState.h"
#include "LuaPlus/LuaTableIterator.h"
#include "Utility/GHandleManager.h"

class IActorComponent;
class GActor;
CREATE_HANDLE_MANAGER( GActor, GActor, 1, 19, 12 );
#define INVALID_ACTOR_HANDLE 0

class GActor
{
	friend GActorHandle;

public:

	GVector3							m_position;

	GMatrix4							m_rotation;

	std::vector<IActorComponent*>		m_components;

	bool								m_markedForDelete;

	// the actor identifier/type...basically.
	GHashedString						m_name;

	void								Update( void );
	GVector3							Position( void );
	void								Kill( bool i_instant = false );
	void								RemoveAllComponents( void );




	// Just keeping these here until I Rendererlement the actual cache coherent way of doing it.
	// I may just switch back to using non-template if I wind up not doing that, though.
	// static GActorHandle				CreateActor( const char* i_characterFile );

	static GActorHandle			CreateActor( const char* i_characterFile, GVector3 i_position, LuaPlus::LuaState* i_state );

	// For now, this is going to suffer from load latency :(  Use other function if possible.
	static GActorHandle			CreateActor( const char* i_characterFile, GVector3 i_position );

	// Creates actor with no data...
	static GActorHandle			CreateActor(  );

	static GActor*				FromHandle( GActorHandle i_handle ) { return s_actorHandleManager.GetData( i_handle ); }


	bool								IsA( GHashedString i_string ) { return i_string == m_name ? true : false ; }

	static GActorHandleManager			s_actorHandleManager;

private:
										GActor( void ) : m_markedForDelete( false ), m_position(GVector3::Zero) {}
										~GActor( void );
};


template<typename T>
T* AttachComponent( GActorHandle i_actor )
{
	T* component = new T();

	GActor* actor = GActor::FromHandle( i_actor );
	actor->m_components.push_back( (IActorComponent*)component );

	return component;
}

template<typename T>
T* GetComponent( GActorHandle i_actor )
{
	//sucks without hashing.
	GActor* actor = GActor::FromHandle( i_actor );
	if( !actor )
		return NULL;

	for( u32 i = 0; i < actor->m_components.size(); i++ )
	{
		//enforced by template.
		if( actor->m_components[i]->Name() == T::m_typeName )
			return (T*)actor->m_components[i];
	}

	return NULL;
}

#endif