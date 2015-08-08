#ifndef _RENDERER_H_
#define _RENDERER_H_

// =========================================================================
// GRenderer.h
// Provides an interface to the renderer from the engine/game layers.
// Allows for the user to not have to worry about renderer implementions.
// =========================================================================

#include <vector>
#include "Actor/GActor.h"
#include "Utility/Singleton.h"
#include "Math/GMatrix4.h"
#include "../../Renderer/Source/ResourceTypes/Entity.h"

namespace Gyro
{

	class RendererObject
	{
	public:
		GActorHandle						m_actor;
		Entity*								m_entity;

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

		static void							DrawDebugLine( const GVector3& i_vStart, const GVector3& i_vEnd, float i_fStartRadius, float i_fEndRadius, uint64_t i_iColor );

	private:
											GRenderer( ) { }
											GRenderer( GRenderer& physics ) {}
		
		std::vector<RendererObject*>		m_objectsDatabase;
	};
}

typedef Singleton< Gyro::GRenderer > g_Renderer;

#endif