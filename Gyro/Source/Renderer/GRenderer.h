#ifndef _RENDERER_H_
#define _RENDERER_H_

// Really, this class only exists because of smart pointers right now...
// I'm going to try to rip these out soon for smart handles.

#include <vector>
#include "Actor/GActor.h"
#include "Utility/Singleton.h"
#include "Utility/GSharedPtr.h"
#include "Math/GMatrix4.h"
#include "../../Renderer/Source/ResourceTypes/Entity.h"

namespace Gyro
{

	class RendererObject
	{
	public:
		GActorHandle						m_actor;
		Entity*								m_entity;
		//GMatrix4							m_transform;


											RendererObject( ) {}
											RendererObject( GActorHandle i_actor ): m_actor( i_actor ){ }
											~RendererObject()
											{
												m_entity->Unload();
												delete m_entity;
											}

	private:
	};

	class GRenderer
	{

		friend Singleton<GRenderer>;

	public:
		void								Shutdown( );
		void								Cleanse();

		bool								Initialize( HWND i_windowHandle );
		void								Update( );
		void								BeginUpdate( );
		void								EndUpdate( );
		void								RemoveDeadObjects( );
		void								RemoveRendererObject( RendererObject* i_object );
		void								AddRendererObject( RendererObject* i_object );
		bool								LoadScene( const char* i_filename );
		bool								LoadBinaryScene( const char* i_filename );

	private:
											GRenderer( ) { }
											GRenderer( GRenderer& physics ) {}
		
		std::vector<RendererObject*>		m_objectsDatabase;
		
	};
}

typedef Singleton< Gyro::GRenderer > g_Renderer;

#endif