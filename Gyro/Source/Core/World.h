#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include "Utility/Singleton.h"
#include "Utility/GSharedPtr.h"
#include "Actor/GActor.h"

//I will be making this class into a proper singleton next week.

namespace Gyro
{

	class World
	{
	public:

		friend Singleton<World>;
		bool									Initialize( );
		void									Shutdown( );
		void									Cleanse();
		void									Update( );
		void									EndUpdate( );

		//keeping it basic for now
		void									AddActor( GActorHandle i_actor );
		void									RemoveActor( GActorHandle i_actor );
		void									RemoveDeadActors( );
		bool									LoadEditorObjects( const char* i_filename );

		GActorHandle							GetActorByName( const char* i_actorName ) const;


		const std::vector<GActorHandle>&		GetActors( ) const { return m_actorDatabase; }
		void									GetActorsByName( const char* i_componentName, std::vector<GActorHandle>& i_actors );

	protected:
												World() {}
												World( World& i_word ) {}

		const char*								m_worldName;
		std::vector<GActorHandle>				m_actorDatabase;

	};
}

typedef Singleton<Gyro::World> g_World;

#endif