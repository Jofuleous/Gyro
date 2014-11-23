#include "GActor.h"
#include "ComponentManager.h"

GActorHandleManager GActor::s_actorHandleManager;

GActor::~GActor( void )
{

	std::vector< IActorComponent* >::iterator iter = m_components.begin();

	while( iter != m_components.end() )
	{	
		delete (*iter);
		iter++;
	}
}

void GActor::Kill( bool i_instant )
{
	// this should only ever happen when the world gets shut down.  DO NOT USE THIS.
	/*
	if( i_instant )
	{
		std::vector< IActorComponent* >::iterator iter = m_components.begin();

		while( iter != m_components.end() )
		{	
			delete (*iter);
			iter++;
		}
	}
	*/
	m_markedForDelete = true;
}

void GActor::RemoveAllComponents( void )
{
		
	std::vector< IActorComponent* >::iterator iter = m_components.begin();

	while( iter != m_components.end() )
	{	
		delete (*iter);
		iter++;
	}

	m_components.clear();
	
}

GActorHandle GActor::CreateActor( )
{
	GActor* actor = new GActor();

	return s_actorHandleManager.AddData( actor );
}


// I imagine this should be happening with some global lua state...but meh.
GActorHandle GActor::CreateActor( const char* i_characterFile, GVector3 i_position )
{
	LuaPlus::LuaState* theLuaState = LuaPlus::LuaState::Create( true );
	GActorHandle actor = CreateActor( i_characterFile, i_position, theLuaState );

	LuaPlus::LuaState::Destroy( theLuaState );

	return actor;
}

// i_characterFile format = "FolderNames/CharacterName"
GActorHandle GActor::CreateActor( const char* i_characterFile, GVector3 i_position, LuaPlus::LuaState* i_state )
{

	const char* chrName = strrchr( i_characterFile, '/' ) + 1;
	if( !chrName )
		chrName = i_characterFile;

	char characterKey[256];
	sprintf_s( characterKey, "data/Characters/%s.carx", i_characterFile );

	// if this character isn't already in the state ( could this be really expensive? ), then load the file.  potential hack.
	if( !i_state->GetGlobal( chrName ).IsTable() && i_state->DoFile( characterKey ) )
	{
		assert(0); // an error occured.
		if( i_state->GetTop() == 1 )
			assert( 0 ); // an error occured.
	}

	GActor* actor = new GActor();
	actor->m_name = GHashedString(chrName);
	actor->m_position = i_position;
	actor->m_rotation = GMatrix4::Identity;

	GActorHandle handle = s_actorHandleManager.AddData(actor);

	LuaPlus::LuaObject charRoot = i_state->GetGlobal( chrName );
	// iterate over the components of the character
	LuaPlus::LuaTableIterator it( charRoot.Get("Components") );
	for ( ; it; it.Next() )
	{
		const char* key = it.GetKey().GetString();
		IActorComponent* comp = g_ComponentManager::Get().CreateComponent( key );
		comp->Initialize( handle, it.GetValue() );
		actor->m_components.push_back( comp );
	}

	return handle;
}

void GActor::Update( void )
{
}

GVector3 GActor::Position( void )
{
	return m_position;
}