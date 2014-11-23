#include "World.h"
#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Utility/GSharedPtr.h"
#include "../Components/GTriggerComponent.h"
#include "../Components/GCollisionComp.h"
#include "Navigation/GWayPointManager.h"
#include <assert.h>
#include <vector>

#include "Profiling/GProfiling.h"

namespace Gyro
{

	bool World::Initialize( )
	{
		return true;
	}

	// this is used for loading actors
	bool World::LoadEditorObjects( const char* i_filePath )
	{
		LuaPlus::LuaState* theLuaState = LuaPlus::LuaState::Create( true );
		if( theLuaState->DoFile( i_filePath ) )
			if( theLuaState->GetTop() == 1 )
				assert( 0 ); // an error occured.


		// get the name of this file.  we need it to find the root.
		const char* i_filename = strrchr( i_filePath, '/' ) + 1;
		int nameLength = strrchr( i_filename, '.' ) - i_filename;
		assert( nameLength < 63 );
		char* sceneName = new char[64];
		memcpy( sceneName, i_filename, nameLength );
		sceneName[nameLength] = '\0'; // null terminator.
		m_worldName = (const char*) sceneName;

		{
			LuaPlus::LuaObject& root = theLuaState->GetGlobal( m_worldName );

			LuaPlus::LuaObject& category = root[ "Objects" ];
			for ( LuaPlus::LuaTableIterator it( category ); it; it.Next() )
			{
				LuaPlus::LuaObject obj = it.GetValue();
				// change this to an integer later.
				const char* key = obj["Type"].GetString();
				if( strcmp( key, "trigger" ) == 0 )
				{
					GVector3 position;
					position._x = obj["Translation"][1].GetFloat();
					position._y = obj["Translation"][2].GetFloat();
					position._z = obj["Translation"][3].GetFloat();

					GActorHandle actorHandle = GActor::CreateActor();
					GActor* actor = GActor::FromHandle( actorHandle );
					actor->m_position = position;
					actor->m_name = GHashedString( it.GetKey().GetString() );
					GTriggerComponent* trigComp = AttachComponent<GTriggerComponent>( actorHandle );
					trigComp->Initialize( actorHandle, obj["Trigger"] );
					GCollisionComp* colComp = AttachComponent<GCollisionComp>( actorHandle );
					colComp->Initialize( actorHandle, obj["Trigger"] );
					m_actorDatabase.push_back( actorHandle );
				}
				else if( strcmp( key, "character" ) == 0 )
				{
					// no rotation for now.
					GVector3 position;
					position._x = obj["Translation"][1].GetFloat();
					position._y = obj["Translation"][2].GetFloat();
					position._z = obj["Translation"][3].GetFloat();

					GActorHandle actor = GActor::CreateActor( obj["Character"]["CharacterFile"].GetString(), position, theLuaState ); // see if this messes things up?
					m_actorDatabase.push_back( actor );
				}
			}

			category = root[ "Navigation" ];
			g_WayPointManager.m_Graph.LoadFromLua( category );
		}

		LuaPlus::LuaState::Destroy( theLuaState );
		theLuaState = NULL;
		return true;
	}

	void World::RemoveActor( GActorHandle i_actor )
	{
		//for now...since we don't have our actor hashes...

		for( int i = 0; i < m_actorDatabase.size(); i++ )
		{
			if( m_actorDatabase[i] == i_actor )
			{
				m_actorDatabase[i] = m_actorDatabase.back();
				m_actorDatabase.pop_back();
			}
		}
	}

	void World::RemoveDeadActors( )
	{
		for( int i = 0; i < m_actorDatabase.size(); i++ )
		{
			GActor* actor = GActor::FromHandle( m_actorDatabase[i] );
			if( actor && actor->m_markedForDelete )
			{
				m_actorDatabase[i] = m_actorDatabase.back();
				m_actorDatabase.pop_back();
			}
		}
	}

	void World::Update( )
	{
		PROFILE_SCOPE_BEGIN( "Actor Component Update" );

		//Perform logic updates and propose movement
		for( u32 i = 0; i < m_actorDatabase.size(); i++ )
		{
			GActor* actor = GActor::FromHandle( m_actorDatabase[i] );
			if( !actor )
				continue;

			for( u32 j = 0; j < actor->m_components.size(); j++ )
			{
				if( actor->m_components[j]->HasUpdate() )
					actor->m_components[j]->Update( m_actorDatabase[i] );
			}
		}

		PROFILE_SCOPE_END( );
	}

	void World::EndUpdate( )
	{

		//Finish logic update and finalize movement
		for( int i = 0; i < m_actorDatabase.size(); i++ )
		{
			GActor* actor = GActor::FromHandle( m_actorDatabase[i] );
			if( !actor )
				continue;

			for( int j = 0; j < actor->m_components.size(); j++ )
			{
				if( actor->m_components[j]->HasEndUpdate() )
					actor->m_components[j]->EndUpdate( m_actorDatabase[i] );
			}
		}
	}

	void World::Cleanse()
	{
		for( unsigned i = 0; i < m_actorDatabase.size(); i++ )
		{
			GActor* actor = GActor::FromHandle( m_actorDatabase[i] );
			if( actor )
				actor->RemoveAllComponents();	
		}

		m_actorDatabase.clear();
	}

	void World::Shutdown()
	{
		Cleanse();
	}

	GActorHandle World::GetActorByName( const char* i_componentName ) const
	{

		for( int i = 0; i < m_actorDatabase.size(); i++ )
		{
			//Forced to do this?  So weird...
			GActor* actor = GActor::FromHandle( m_actorDatabase[i] );

			if( actor && actor->IsA( i_componentName ) )
				return m_actorDatabase[i];

		}

		return GActorHandle();
	}

	void World::GetActorsByName( const char* i_actorName, std::vector<GActorHandle>& i_actors)
	{
		for( int i = 0; i < m_actorDatabase[i]; i++ )
		{
			GActor* actor = GActor::FromHandle( m_actorDatabase[i] );
			assert( actor );
			if( actor->IsA( i_actorName ) )
				i_actors.push_back( m_actorDatabase[i] );
		}
	}

	void World::AddActor( GActorHandle i_actor )
	{
		m_actorDatabase.push_back( i_actor );
	}
}