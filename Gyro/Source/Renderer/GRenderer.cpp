#include "GRenderer.h"
#include "Components/GRenderComponent.h"
#include "Utility/Clock.h"
#include "Actor/ComponentManager.h"
#include <assert.h>
#include "Math/GMath.h"
#include "../../../Renderer/Source/MainWindow.h"
#include "../../../Renderer/Source/RenderDevices.h"
#include "../../../Renderer/Source/RenderManager.h"

#include "Misc/DebugConsole.h"
#include "Profiling/GProfiling.h"

namespace Gyro
{

	void RendererButtonCallback( const char* i_name )
	{
		g_RenderManager.m_clearColor = D3DXCOLOR( 0.1f, 0.1f, 0.1f, 1.0f );
	}

	void RendererBoolCallback( const char* i_name, bool* i_value )
	{
		if( *i_value )
			g_RenderDevices.GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		else
			g_RenderDevices.GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	bool GRenderer::Initialize( HWND i_windowHandle )
	{
		// todo: ...
		RegisterComponent<GRenderComponent>();
		
		g_RenderDevices.Initialize( i_windowHandle, g_MainWindow.GetWidth(), g_MainWindow.GetHeight() );
		g_RenderManager.Initialize( g_MainWindow.GetWidth(), g_MainWindow.GetHeight() );


		// Debug Console commands
		static bool check = false;
		DebugConsole::AddCheckBox( "Renderer Wireframe", &check, RendererBoolCallback );
		DebugConsole::AddButton( "Renderer Black Sky", RendererButtonCallback );

		return true;
	}

	bool GRenderer::LoadScene( const char* i_filename )
	{
		return g_RenderManager.GetCurrentScene()->Load( i_filename );
	}

	bool GRenderer::LoadBinaryScene( const char* i_filename )
	{
		return g_RenderManager.GetCurrentScene()->LoadBinaryScene( i_filename );
	}

	void GRenderer::BeginUpdate( )
	{

		int count = m_objectsDatabase.size();

		// i guess we'll update the entity right here.  kind of awkward, but it will work fine for now.
		//Nothing right now.
		for( int i = 0; i < count; i++ )
		{
			GActor* actor = GActor::FromHandle( m_objectsDatabase[i]->m_actor );

			actor->m_position.ToD3DX( m_objectsDatabase[i]->m_entity->m_position );
			//m_objectsDatabase[i]->m_actor->m_rotation.ToD3DX( m_objectsDatabase[i]->m_entity->m_

			// blah..have to do this for now...
			GVector3 dir = actor->m_rotation.Dir();
			m_objectsDatabase[i]->m_entity->m_orientationY = atan2( dir._x, dir._z );
		}
	}

	void GRenderer::Update( )
	{
		PROFILE_SCOPE_BEGIN( "Renderer Update" )
		//update position
		float dt = g_Clock::Get().SecondsSinceLastFrame();

		std::vector< RendererObject* >::iterator iter = m_objectsDatabase.begin();

		int count = m_objectsDatabase.size();

		g_RenderManager.Update( dt );


		while( count-- )
		{
			RendererObject* rObj = (*iter);	

			/*
			Cheesy::Point2D center;
			center.x = rObj->m_actor->m_position.x();
			center.y = rObj->m_actor->m_position.y();

			//since we're just accounting for the y-rotation right now...
			float heading = atan2( rObj->m_actor->m_rotation(0,1), rObj->m_actor->m_rotation(0,0) );
			heading = GMath::Rad2Deg( heading );
			rObj->m_sprite->Draw( Cheesy::Point2D( rObj->m_actor->m_position.x(), rObj->m_actor->m_position.y()), heading );
			*/
			g_RenderManager.Submit( rObj->m_entity );

			iter++;
		}

		PROFILE_SCOPE_BEGIN( "Render Sort" );
		g_RenderManager.Sort();
		PROFILE_SCOPE_END();

		PROFILE_SCOPE_BEGIN( "Render Draw" );
		g_RenderManager.Draw();
		PROFILE_SCOPE_END();

		PROFILE_SCOPE_BEGIN( "Render Present" );
		g_RenderManager.Present();
		PROFILE_SCOPE_END( );

		PROFILE_SCOPE_END();
	}

	void GRenderer::EndUpdate( )
	{
		g_RenderManager.Clear();
	}

	void GRenderer::Cleanse()
	{
		if( m_objectsDatabase.size() > 0 )
			m_objectsDatabase.clear();

		g_RenderManager.Cleanse();
	}
	void GRenderer::Shutdown( )
	{
		Cleanse();
		//Singleton<RenderManager>::Release();
	}

	void GRenderer::AddRendererObject( RendererObject* i_object )
	{
		m_objectsDatabase.push_back( i_object );
	}



	// there really needs to be a faster and safer way of performing this operation.  
	void GRenderer::RemoveRendererObject( RendererObject* i_object )
	{
		for( u32 i = 0; i < m_objectsDatabase.size(); i++ )
		{
			RendererObject* last = m_objectsDatabase.back();
			m_objectsDatabase[i] = last;
			m_objectsDatabase.pop_back();
			continue;
		}
	}



	void GRenderer::RemoveDeadObjects( )
	{
		std::vector< RendererObject* >::iterator iter = m_objectsDatabase.begin();

		int count = m_objectsDatabase.size();

		for( u32 i = 0; i < m_objectsDatabase.size(); )
		{
			GActor* actor = GActor::FromHandle( m_objectsDatabase[i]->m_actor );
			if( actor->m_markedForDelete )
			{
				RendererObject* last = m_objectsDatabase.back();
				delete m_objectsDatabase[i];
				m_objectsDatabase[i] = last;
				m_objectsDatabase.pop_back();
				continue;
			}
			i++;
		}
	}

}