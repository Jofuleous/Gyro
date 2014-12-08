#include "RenderManager.h"
#include <algorithm>
#include "ResourceTypes/Entity.h"
#include <assert.h>
#include "Camera.h"
#include "d3dx9.h"
#include <Strsafe.h>
#include <wchar.h>
#include <math.h>
//#include "EnviroMap.h"
#include "ResourceTypes/Technique.h"
#include "ResourceTypes/VertexShader.h"
#include "d3d9.h"
#include "RenderDevices.h"
#include "MainWindow.h"
#include "Animation/GSkeleton.h"

RenderManager::RenderManager() : m_lines( 5000 ), m_hud( 30 ), m_currentScene( NULL )
{

}

RenderManager::~RenderManager()
{
	if( m_currentScene)
	{
		m_currentScene->Unload();
		delete m_currentScene;
	}

	m_alphaBucket.clear();
	m_keyList.clear();
	m_postList.clear();
	m_depthBucket.clear();
	m_UIList.clear();

	// clean up the managers.
	ResourceManager<MaterialReference>::m_resourceMap.clear();
	ResourceManager<PixelShaderReference>::m_resourceMap.clear();
	ResourceManager<VertexShaderReference>::m_resourceMap.clear();
	ResourceManager<TextureReference>::m_resourceMap.clear();
	ResourceManager<EffectReference>::m_resourceMap.clear();
	ResourceManager<EnviroMapReference>::m_resourceMap.clear();
}

void RenderManager::Update( float i_dt )
{
	m_currentScene->Update( i_dt );
	m_currentScene->Draw( i_dt );
}

void RenderManager::Present( )
{
	const RECT* noSourceRectangle = NULL;
	const RECT* noDestinationRectangle = NULL;
	HWND useDefaultWindow = NULL;
	const RGNDATA* noDirtyRegion = NULL;
	HRESULT result = g_RenderDevices.GetDevice()->Present( noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion );
	assert( SUCCEEDED( result ) );
}

void RenderManager::Initialize( int i_textureWidth, int i_textureHeight )
{
	//god this is disgusting...but let's get it working...
	m_currentScene = new Scene();
	m_lines.OnResetDevice();
	m_hud.OnDeviceReset();

	m_textureWidth = i_textureWidth;
	m_textureHeight = i_textureHeight;

	g_RenderDevices.GetDevice()->CreateTexture( i_textureWidth, i_textureHeight, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_opaqueTexture, NULL );

	g_RenderDevices.GetDevice()->CreateTexture( i_textureWidth, i_textureHeight, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_R32F, D3DPOOL_DEFAULT, &m_depthTexture, NULL );

	g_RenderDevices.GetDevice()->CreateTexture( i_textureWidth, i_textureHeight, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_finalBuffer, NULL );

	g_RenderDevices.GetDevice()->CreateTexture( i_textureWidth , i_textureHeight , 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_R16F, D3DPOOL_DEFAULT, &m_shadowTexture, NULL );

	g_RenderDevices.GetDevice()->GetRenderTarget( 0, &m_backBuffer );

	m_clearColor = D3DCOLOR_XRGB( 80, 120, 254 );

	/* this doesn't work lol
	float noZBuffer = 1.0f;
	DWORD noStencilBuffer = 1;
	HRESULT result;

	// run one frame just to get the clear color :)
	g_RenderDevices.GetDevice()->BeginScene();
	g_RenderDevices.GetDevice()->SetRenderTarget( 0, m_backBuffer );
	g_RenderDevices.GetDevice()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_clearColor, noZBuffer, noStencilBuffer );
	g_RenderDevices.GetDevice()->EndScene();
	Present();
	*/
}

void RenderManager::Shutdown( void )
{
	if( m_currentScene)
	{
		m_currentScene->Unload();
		delete m_currentScene;
		m_currentScene = NULL;
	}

	m_alphaBucket.clear();
	m_keyList.clear();
	m_postList.clear();
	m_depthBucket.clear();
	m_UIList.clear();

	// clean up the managers.
	ResourceManager<MaterialReference>::m_resourceMap.clear();
	ResourceManager<PixelShaderReference>::m_resourceMap.clear();
	ResourceManager<VertexShaderReference>::m_resourceMap.clear();
	ResourceManager<TextureReference>::m_resourceMap.clear();
	ResourceManager<EffectReference>::m_resourceMap.clear();
	ResourceManager<EnviroMapReference>::m_resourceMap.clear();
}


void RenderManager::Submit( Entity* i_entity )
{
	//this is bad keeping all these lists at teh same time possibly lol!!!  get the grade lol!!!!

	//depth bucket
	if( i_entity->m_material->rm_Reference.m_effect->rm_Reference.m_depth )
	{
		SortElement se( i_entity->m_sortKey, i_entity );
		m_depthBucket.push_back( se );
	}

	//opaque bucket
	if( i_entity->m_material->rm_Reference.m_effect->rm_Reference.m_opaque)
	{
		SortElement se( i_entity->m_sortKey, i_entity );
		m_keyList.push_back( se );
	}
	//post bucket...for now.
	else if( i_entity->m_material->rm_Reference.m_effect->rm_Reference.m_postOpaque )
	{
		SortElement se( i_entity->m_sortKey, i_entity );
		m_postList.push_back( se );
	}
	else if( i_entity->m_material->rm_Reference.m_effect->rm_Reference.m_alpha )
	{
		//alpha bucket
		AlphaSortElement se;
		D3DXVECTOR3 displacement = i_entity->m_position - m_currentScene->GetCamera()->m_position;
		se.a_distanceSquared = displacement.x * displacement.x + displacement.y * displacement.y + displacement.z * displacement.z;
		se.a_distanceSquared = abs( se.a_distanceSquared );
		se.a_entity = i_entity;
		m_alphaBucket.push_back( se );
	}
	else if( i_entity->m_material->rm_Reference.m_effect->rm_Reference.m_UI )
	{
		m_UIList.push_back( i_entity );
	}
}

void RenderManager::GenerateSortKey( Entity* i_entity )
{
	i_entity->m_sortKey = 0;
	i_entity->m_sortKey |= i_entity->m_material->rm_Reference.m_effect->rm_Reference.m_effectId;
	//give us 13 bits for the material id

	i_entity->m_sortKey <<= 13;
	i_entity->m_sortKey |= i_entity->m_material->rm_Reference.m_materialId;
}

static int cmpare (const void * a, const void * b)
{
  return (int)( (*(SortElement*)a).se_key - (*(SortElement*)b).se_key );
}

int compareDistance( const void * a, const void * b )
{
	return (int) ((*(AlphaSortElement*)b).a_distanceSquared - (*(AlphaSortElement*)a).a_distanceSquared );
}

void RenderManager::Sort( void )
{
	if( m_depthBucket.size() > 0 )
		qsort( &m_depthBucket[0], m_depthBucket.size(), sizeof(SortElement), cmpare );

	//i'm not going to worry about having a shadow bucket right now, since we're only really ever going to have one shadow technique (since i'm also not considering binary alpha).
	//for now, i assume that all things opaque are what's casting shadows.  i'll use the shadow technique of the materials that have submitted to this bucket.
	if( m_keyList.size() > 0 )
		qsort( &m_keyList[0], m_keyList.size(), sizeof(SortElement), cmpare );

	if( m_alphaBucket.size() > 0 )
		qsort( &m_alphaBucket[0], m_alphaBucket.size(), sizeof(AlphaSortElement), compareDistance );

	if( m_postList.size() > 0 )
		qsort( &m_postList[0], m_postList.size(), sizeof(SortElement), cmpare );
}

//Draw the bucketz
void RenderManager::Draw( void )
{
	unsigned __int16		lastEffect =  65000;
	unsigned __int16		lastMaterial = 65000;
	bool	sameEffect = false;
	bool	sameMaterial = false;

	IDirect3DSurface9* opaqueSurface;
	m_opaqueTexture->GetSurfaceLevel( 0, &opaqueSurface );

	IDirect3DSurface9* depthSurface;
	m_depthTexture->GetSurfaceLevel( 0, &depthSurface );

	IDirect3DSurface9* shadowSurface;
	m_shadowTexture->GetSurfaceLevel( 0, &shadowSurface );


	IDirect3DSurface9* finalSurface = NULL;
	if( Scene::m_postProcessing )
	{
		m_finalBuffer->GetSurfaceLevel( 0, &finalSurface );
	}

	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	const D3DRECT* subRectanglesToClear = NULL;
	DWORD subRectangleCount = 0;
	DWORD clearTheRenderTargetAndZ = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
	DWORD clearTheRenderTargetNoZ = D3DCLEAR_TARGET;
	D3DCOLOR clearColor;
	{
		clearColor = m_clearColor;
	}
	float noZBuffer = 1.0f;
	DWORD noStencilBuffer = 1;
	HRESULT result;

	result = g_RenderDevices.GetDevice()->BeginScene();
	assert( SUCCEEDED( result ) );

	// SET SCENE CONSTANTS...
	RenderManager::SetSceneConstants( );

	//===============DRAW SHADOW (use depth technique)=====================
	if( GetCurrentScene()->m_shadowMap && m_depthBucket.size() > 0 )
	{
#ifdef _DEBUG
		D3DPERF_EndEvent( );
		D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"Shadow Bucket");
#endif

		//render the depth bucket.  still not the way i want to be doing it, but let's get the grade...
		//SET DEPTH SURFACE AS RENDER TARGET
		g_RenderDevices.GetDevice()->SetRenderTarget( 0, shadowSurface );
		//CLEAR IT
		result = g_RenderDevices.GetDevice()->Clear( subRectangleCount, subRectanglesToClear,
			clearTheRenderTargetAndZ, clearColor, noZBuffer, noStencilBuffer );

		assert( SUCCEEDED( result ) );

		for(int i = 0; i < (int)m_depthBucket.size(); i++ )
		{
			ResourcePtr<EffectReference> effectRef = m_depthBucket[i].se_entity->m_material->rm_Reference.m_effect;
			unsigned __int16 check = effectRef->rm_Reference.m_effectId;
			unsigned __int16 check2 = (unsigned __int16) m_depthBucket[i].se_key >> 13;
			Entity* currentEntity = m_depthBucket[i].se_entity;

			if( effectRef->rm_Reference.m_effectId != lastEffect )
				sameEffect = false;
			else
				sameEffect = true;

			lastEffect = effectRef->rm_Reference.m_effectId;

			unsigned __int16 matCheck = m_depthBucket[i].se_entity->m_material->rm_Reference.m_materialId;
			unsigned __int16 matCheck2 = 0x0000000D & m_depthBucket[i].se_key;

			if( matCheck == lastMaterial )
				sameMaterial = true;
			else
				sameMaterial = false;

			lastMaterial = matCheck;

			ResourcePtr<MaterialReference> mRef = m_depthBucket[i].se_entity->m_material;

			RenderManager::SetUniformData( currentEntity, effectRef, mRef, sameEffect, sameMaterial, 7 );
			currentEntity->Draw();
		}
	}

	//===============DRAW DEPTH=====================
	//if(  m_depthBucket.size() > 0 )
	{
#ifdef _DEBUG
		D3DPERF_EndEvent( );
		D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"Depth Bucket");
#endif

		//render the depth bucket.  still not the way i want to be doing it, but let's get the grade...
		//SET DEPTH SURFACE AS RENDER TARGET
		g_RenderDevices.GetDevice()->SetRenderTarget( 0, depthSurface );
		//CLEAR IT
		result = g_RenderDevices.GetDevice()->Clear( subRectangleCount, subRectanglesToClear,
			clearTheRenderTargetAndZ, clearColor, noZBuffer, noStencilBuffer );

		assert( SUCCEEDED( result ) );

		for(int i = 0; i < (int)m_depthBucket.size(); i++ )
		{
			ResourcePtr<EffectReference> effectRef = m_depthBucket[i].se_entity->m_material->rm_Reference.m_effect;
			unsigned __int16 check = effectRef->rm_Reference.m_effectId;
			unsigned __int16 check2 = (unsigned __int16) m_depthBucket[i].se_key >> 13;
			Entity* currentEntity = m_depthBucket[i].se_entity;

			if( effectRef->rm_Reference.m_effectId != lastEffect )
				sameEffect = false;
			else
				sameEffect = true;

			lastEffect = effectRef->rm_Reference.m_effectId;

			unsigned __int16 matCheck = m_depthBucket[i].se_entity->m_material->rm_Reference.m_materialId;
			unsigned __int16 matCheck2 = 0x0000000D & m_depthBucket[i].se_key;

			if( matCheck == lastMaterial )
				sameMaterial = true;
			else
				sameMaterial = false;

			lastMaterial = matCheck;

			ResourcePtr<MaterialReference> mRef = m_depthBucket[i].se_entity->m_material;

			RenderManager::SetUniformData( currentEntity, effectRef, mRef, sameEffect, sameMaterial, 0 );
			currentEntity->Draw();
		}
	}

	///////////////////////////////DRAW OPAQUE BUCKET////////////////////////////
	//if(  m_keyList.size() > 0 )
	{
#ifdef _DEBUG
		D3DPERF_EndEvent( );
		D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"Opaque Bucket");
#endif
		//reset values
		lastEffect =  65000;
		lastMaterial = 65000;
		sameEffect = false;
		sameMaterial = false;

		//SET OPAQUE SURFACE AS RENDER TARGET.
		g_RenderDevices.GetDevice()->SetRenderTarget( 0, opaqueSurface );
		//CLEAR IT
		result = g_RenderDevices.GetDevice()->Clear( subRectangleCount, subRectanglesToClear,
			clearTheRenderTargetNoZ, clearColor, noZBuffer, noStencilBuffer );
		assert( SUCCEEDED( result ) );
		
		// anti-alias
		//g_RenderDevices.GetDevice()->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );

		for(int i = 0; i < (int)m_keyList.size(); i++ )
		{
			ResourcePtr<EffectReference> effectRef = m_keyList[i].se_entity->m_material->rm_Reference.m_effect;
			unsigned __int16 check = effectRef->rm_Reference.m_effectId;
			unsigned __int16 check2 = (unsigned __int16) m_keyList[i].se_key >> 13;
			Entity* currentEntity = m_keyList[i].se_entity;

			if( effectRef->rm_Reference.m_effectId != lastEffect )
				sameEffect = false;
			else
				sameEffect = true;

			lastEffect = effectRef->rm_Reference.m_effectId;

			unsigned __int16 matCheck = m_keyList[i].se_entity->m_material->rm_Reference.m_materialId;
			unsigned __int16 matCheck2 = 0x0000000D & m_keyList[i].se_key;

			if( matCheck == lastMaterial )
				sameMaterial = true;
			else
				sameMaterial = false;

			lastMaterial = matCheck;

			ResourcePtr<MaterialReference> mRef = m_keyList[i].se_entity->m_material;

			RenderManager::SetUniformData( currentEntity, effectRef, mRef, sameEffect, sameMaterial, 1 );
			currentEntity->Draw();
		}
	}

	m_lines.Render( m_currentScene->GetCamera()->ViewTransform(), m_currentScene->GetCamera()->ViewTransform(),
		m_currentScene->GetCamera()->ProjectionTransform( ), false );

	g_RenderDevices.GetDevice()->SetVertexDeclaration( g_RenderDevices.GetDefaultVertexDeclaration() );

	
	//SET BACKBUFFER AS RENDER TARGET...or final surface if we're post processing...

	if( finalSurface )
		g_RenderDevices.GetDevice()->SetRenderTarget( 0, finalSurface );
	else
		g_RenderDevices.GetDevice()->SetRenderTarget( 0, m_backBuffer );

	//CLEAR IT
	result = g_RenderDevices.GetDevice()->Clear( subRectangleCount, subRectanglesToClear,
		clearTheRenderTargetNoZ, clearColor, noZBuffer, noStencilBuffer );
	assert( SUCCEEDED( result ) );

	if( finalSurface )
		g_RenderDevices.GetDevice()->StretchRect( opaqueSurface, NULL, finalSurface, NULL, D3DTEXF_LINEAR );
	else
		g_RenderDevices.GetDevice()->StretchRect( opaqueSurface, NULL, m_backBuffer, NULL, D3DTEXF_LINEAR );

	
	lastEffect =  65000;
	lastMaterial = 65000;
	sameEffect = false;
	sameMaterial = false;

	if( m_postList.size() > 0 )
	{
#ifdef _DEBUG
		D3DPERF_EndEvent( );
		D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"Post-Alpha-Opaque Bucket");
#endif
		//render the post 
		for(int i = 0; i < (int)m_postList.size(); i++ )
		{
			ResourcePtr<EffectReference> effectRef = m_postList[i].se_entity->m_material->rm_Reference.m_effect;
			unsigned __int16 check = effectRef->rm_Reference.m_effectId;
			unsigned __int16 check2 = (unsigned __int16) m_postList[i].se_key >> 13;
			Entity* currentEntity = m_postList[i].se_entity;

			if( effectRef->rm_Reference.m_effectId != lastEffect )
				sameEffect = false;
			else
				sameEffect = true;

			lastEffect = effectRef->rm_Reference.m_effectId;

			unsigned __int16 matCheck = m_postList[i].se_entity->m_material->rm_Reference.m_materialId;
			unsigned __int16 matCheck2 = 0x0000000D & m_postList[i].se_key;

			if( matCheck == lastMaterial )
				sameMaterial = true;
			else
				sameMaterial = false;

			lastMaterial = matCheck;

			ResourcePtr<MaterialReference> mRef = m_postList[i].se_entity->m_material;

			RenderManager::SetUniformData( currentEntity, effectRef, mRef, sameEffect, sameMaterial, 2 );
			currentEntity->Draw();
		}
	}

	lastEffect =  65000;
	lastMaterial = 65000;
	sameEffect = false;
	sameMaterial = false;

		//Draw alpha bucket
	if(  m_alphaBucket.size() > 0 )
	{
#ifdef _DEBUG
		D3DPERF_EndEvent( );
		D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"Alpha Bucket");
#endif

		for(int i = 0; i < (int)m_alphaBucket.size(); i++ )
		{
			ResourcePtr<EffectReference> effectRef = m_alphaBucket[i].a_entity->m_material->rm_Reference.m_effect;
			float displacement = m_alphaBucket[i].a_distanceSquared;
			Entity* currentEntity = m_alphaBucket[i].a_entity;

			if( effectRef->rm_Reference.m_effectId == lastEffect )
				sameEffect = true;
			else
				sameEffect = false;
			lastEffect = effectRef->rm_Reference.m_effectId;

			unsigned __int16 matCheck = m_alphaBucket[i].a_entity->m_material->rm_Reference.m_materialId;
			if( matCheck == lastMaterial )
				sameMaterial = true;
			else
				sameMaterial = false;
			lastMaterial = matCheck;

			ResourcePtr<MaterialReference> mRef = m_alphaBucket[i].a_entity->m_material;

			SetUniformData( currentEntity, effectRef, mRef, sameEffect, sameMaterial, 3 );
			currentEntity->Draw();
		}
	}

	if( Scene::m_postProcessing )
	{
		g_RenderDevices.GetDevice()->SetRenderTarget( 0, m_backBuffer );

		//CLEAR IT
		result = g_RenderDevices.GetDevice()->Clear( subRectangleCount, subRectanglesToClear,
			clearTheRenderTargetNoZ, clearColor, noZBuffer, noStencilBuffer );
		assert( SUCCEEDED( result ) );

		SetUniformData( Scene::m_postProcessEntity, Scene::m_postProcessEntity->m_material->rm_Reference.m_effect, Scene::m_postProcessEntity->m_material, false, sameMaterial, 4 );
		Scene::m_postProcessEntity->Draw();
	}

	//draw UI elements
	if( m_UIList.size() > 0 )
	{
		for(int i = 0; i < (int)m_UIList.size(); i++ )
		{
			SetUniformData( m_UIList[i], m_UIList[i]->m_material->rm_Reference.m_effect, m_UIList[i]->m_material, false, false, 6 );
			m_UIList[i]->Draw();
		}
	}

	// draw debug hud
	m_hud.Render();
	g_RenderDevices.GetDevice()->SetVertexDeclaration( g_RenderDevices.GetDefaultVertexDeclaration() );
	g_RenderDevices.GetDevice()->SetRenderState(D3DRS_ZENABLE, true);

	//===============END DRAW===============================
	result = g_RenderDevices.GetDevice()->EndScene();
	assert( SUCCEEDED( result ) );

	depthSurface->Release();
	opaqueSurface->Release();
	shadowSurface->Release();
	if( Scene::m_postProcessing )
		finalSurface->Release();
}

void RenderManager::SetSceneConstants( )
{
	D3DXMATRIX  viewProjectionMatrix =  RenderManager::GetCurrentScene()->GetCamera()->ViewTransform() * RenderManager::GetCurrentScene()->GetCamera()->ProjectionTransform();
	D3DXMatrixTranspose( &viewProjectionMatrix, &viewProjectionMatrix );
	HRESULT result = g_RenderDevices.GetDevice()->SetVertexShaderConstantF( 4, (float*) &viewProjectionMatrix, 4 );
	assert( SUCCEEDED( result ) );
}

void RenderManager::SetSceneOrthoConstants( )
{
	D3DXMATRIX  viewProjectionMatrix =  RenderManager::GetCurrentScene()->GetCamera()->ViewTransform() * RenderManager::GetCurrentScene()->GetCamera()->OrthoProjection();
	D3DXMatrixTranspose( &viewProjectionMatrix, &viewProjectionMatrix );
	HRESULT result = g_RenderDevices.GetDevice()->SetVertexShaderConstantF( 4, (float*) &viewProjectionMatrix, 4 );
	assert( SUCCEEDED( result ) );
}

void RenderManager::SetUniformData( Entity* currentEntity, ResourcePtr<EffectReference> effectRef, ResourcePtr<MaterialReference> mRef, bool sameEffect, bool sameMaterial, int pass )
{
	VertexShaderData* vData;
	PixelShaderData* pData;

	// enable this for zelda scene...
	//g_RenderDevices.GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE  );

	switch( pass )
	{
	case 0:
		vData = &effectRef->rm_Reference.m_depth->m_vertexShader->rm_Reference;
		pData = &effectRef->rm_Reference.m_depth->m_pixelShader->rm_Reference;
		break;
	case 1:
		vData = &effectRef->rm_Reference.m_opaque->m_vertexShader->rm_Reference;
		pData = &effectRef->rm_Reference.m_opaque->m_pixelShader->rm_Reference;			
		break;
	case 2:
		vData = &effectRef->rm_Reference.m_postOpaque->m_vertexShader->rm_Reference;
		pData = &effectRef->rm_Reference.m_postOpaque->m_pixelShader->rm_Reference;			
		break;
	case 3:
		vData = &effectRef->rm_Reference.m_alpha->m_vertexShader->rm_Reference;
		pData = &effectRef->rm_Reference.m_alpha->m_pixelShader->rm_Reference;			
		break;
	case 4:
		vData = &effectRef->rm_Reference.m_postProcess->m_vertexShader->rm_Reference;
		pData = &effectRef->rm_Reference.m_postProcess->m_pixelShader->rm_Reference;			
		break;
	case 6:
		vData = &effectRef->rm_Reference.m_UI->m_vertexShader->rm_Reference;
		pData = &effectRef->rm_Reference.m_UI->m_pixelShader->rm_Reference;			
		break;
	case 7:
		vData = &effectRef->rm_Reference.m_depth->m_vertexShader->rm_Reference;
		pData = &effectRef->rm_Reference.m_depth->m_pixelShader->rm_Reference;
		break;

	}


	HRESULT result;

	if( !sameEffect )
	{
		sameEffect = false;
#ifdef _DEBUG
		D3DPERF_EndEvent( );
		D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"New Effect");
#endif
		// set the vertex and pixel shaders and eventually renderstate.
		result = g_RenderDevices.GetDevice()->SetVertexShader( vData->m_vertexShader );
		assert( SUCCEEDED( result ) );
		result = g_RenderDevices.GetDevice()->SetPixelShader( pData->m_pixelShader );
		assert( SUCCEEDED( result ) );

		if ( !effectRef->rm_Reference.m_skinHack )
			g_RenderDevices.GetDevice()->SetVertexDeclaration(g_RenderDevices.GetDefaultVertexDeclaration());
		else
			g_RenderDevices.GetDevice()->SetVertexDeclaration(g_RenderDevices.GetSkinnedVertexDeclaration());

		if( effectRef->rm_Reference.m_alpha || effectRef->rm_Reference.m_UI )
		{
			//alpha type
			int swt = 0;
			if( effectRef->rm_Reference.m_alpha )
				swt = effectRef->rm_Reference.m_alpha->m_type;
			else
				swt = effectRef->rm_Reference.m_UI->m_type;
			switch( swt )
			{
			case 1: // additive
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE  );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				//g_RenderDevices.GetDevice()->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLEND_SRCALPHA );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				break;
			
			case 2: // partial
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE  );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				break;
			case 3: // test
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE   );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE  );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHAREF, (DWORD)0x00000001 );
				g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				break;
			case 4: //opaque sample alpha
				break;
			}
		}
		else
		{
			g_RenderDevices.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		}
	}

	if( !sameMaterial )
	{
#ifdef _DEBUG
		D3DPERF_EndEvent( );
		D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"New Material");
#endif
		// VIEW MATRIX
		{
			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;
			const char* variableName = "g_view";

			variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );

			if( variable )
			{
				D3DXMATRIX view;
				//not shadow
				m_currentScene->GetLightViewMatrix( view );

				result =  pData->m_pixelShaderConstantTable->SetMatrix( g_RenderDevices.GetDevice(), variable, &view );
				assert( SUCCEEDED( result ) );
			}

		}

		D3DXHANDLE variable;
		D3DXHANDLE topLevelVariable = NULL;
		const char* variableName = "g_projection";
		variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );

		if( variable )
		{
			D3DXMATRIX projection;
			//not shadow
			m_currentScene->GetLightViewToProjection( projection );

			result =  pData->m_pixelShaderConstantTable->SetMatrix( g_RenderDevices.GetDevice(), variable, &projection );
			assert( SUCCEEDED( result ) );
		}


		if( pass > 0 )
		{

			// LIGHT POSITION.  For now, let's make the assumption that all of our effects use a light...
			{
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;
				const char* variableName = "g_lightPosition";
				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
				if ( variable != NULL )
				{
					D3DXVECTOR4 lightPos = m_currentScene->m_pointLight->m_position;
					result =  pData->m_pixelShaderConstantTable->SetVector( g_RenderDevices.GetDevice(), variable, &lightPos );
					assert( SUCCEEDED( result ) );
				}
			}

			// SECONDS ELAPSED
			{
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;
				const char* variableName = "g_secondsElapsed";
				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
				if ( variable != NULL )
				{
					float time = 1.0f; // todo: integrate timer back in...
					result =  pData->m_pixelShaderConstantTable->SetFloat( g_RenderDevices.GetDevice(), variable, time );
					assert( SUCCEEDED( result ) );
				}
			}

			// LIGHT PARMS
			{
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;
				const char* variableName = "g_lightParms";
				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
				if ( variable != NULL )
				{
					D3DXVECTOR4 lightParms = m_currentScene->m_pointLight->m_color;
					lightParms.w = m_currentScene->m_pointLight->m_radius;
					result =  pData->m_pixelShaderConstantTable->SetVector( g_RenderDevices.GetDevice(), variable, &lightParms );
					assert( SUCCEEDED( result ) );
				}

				variableName = "g_dirLightDir";
				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				if( variable != NULL )
				{
					D3DXVECTOR4 dir = m_currentScene->m_directionalLightDir;
					dir.w = 1;
					result = pData->m_pixelShaderConstantTable->SetVector( g_RenderDevices.GetDevice(), variable, &dir );
				}

				variableName = "g_lightIntensity";
				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				if( variable != NULL )
				{
					float intensity = m_currentScene->m_pointLight->m_intensity;
					result = pData->m_pixelShaderConstantTable->SetFloat( g_RenderDevices.GetDevice(), variable, intensity );
				}

				variableName = "g_dirLightColor";
				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				if( variable != NULL )
				{
					D3DXVECTOR4 dir = m_currentScene->m_dirLightColor;
					dir.w = m_currentScene->m_dirLightIntensity; //intensity;
					result = pData->m_pixelShaderConstantTable->SetVector( g_RenderDevices.GetDevice(), variable, &dir );
				}
			}

			// AMBIENT LIGHT
			{
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;
				const char* variableName = "g_ambientLight";
				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
				if ( variable != NULL )
				{
					D3DXVECTOR4 light = m_currentScene->m_ambientLight;
					result =  pData->m_pixelShaderConstantTable->SetVector( g_RenderDevices.GetDevice(), variable, &light );
					assert( SUCCEEDED( result ) );
				}
			}

			//Environment map?
			{
				const char* variableName = "g_environmentMap";
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
				if ( variable != NULL )
				{
					LPDIRECT3DCUBETEXTURE9 texture = m_currentScene->m_enviroMap->rm_Reference;
					UINT index =  pData->m_pixelShaderConstantTable->GetSamplerIndex( variable );
					g_RenderDevices.GetDevice()->SetTexture( index, texture );
					assert( SUCCEEDED( result ) );
				}
			}
			

			//result of opaque pass
			{
				const char* variableName = "g_opaque";
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );

				if ( variable != NULL )
				{
					if( m_opaqueTexture )
					{
						UINT index =  pData->m_pixelShaderConstantTable->GetSamplerIndex( variable );
						g_RenderDevices.GetDevice()->SetTexture( index, m_opaqueTexture );
						assert( SUCCEEDED( result ) );
					}
				}
			}

			//pass shadow map if needed
			{
				const char* variableName = "g_shadowMap";
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );

				if ( variable != NULL )
				{
					if( m_opaqueTexture )
					{
						UINT index =  pData->m_pixelShaderConstantTable->GetSamplerIndex( variable );
						g_RenderDevices.GetDevice()->SetTexture( index, m_shadowTexture );
						assert( SUCCEEDED( result ) );
					}
				}
			}

			//result of depth pass
			{
				const char* variableName = "g_depth";
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );

				if ( variable != NULL )
				{
					if( m_depthTexture )
					{
						UINT index =  pData->m_pixelShaderConstantTable->GetSamplerIndex( variable );
						g_RenderDevices.GetDevice()->SetTexture( index, m_depthTexture );
						assert( SUCCEEDED( result ) );
					}
				}
			}

			//result of final
			{
				const char* variableName = "g_postTexture";
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );

				if ( variable != NULL )
				{
					if( RenderManager::m_depthTexture )
					{
						UINT index =  pData->m_pixelShaderConstantTable->GetSamplerIndex( variable );
						g_RenderDevices.GetDevice()->SetTexture( index, RenderManager::m_finalBuffer );
						assert( SUCCEEDED( result ) );
					}
				}
			}

			// SET FLOATS..
			for( unsigned p = 0; p < mRef->rm_Reference.m_floatNames.size(); p++ )
			{
				const char* variableName = mRef->rm_Reference.m_floatNames[p];
				float value = mRef->rm_Reference.m_floats[p];

				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
				if ( variable != NULL )
				{
					result =  pData->m_pixelShaderConstantTable->SetFloat( g_RenderDevices.GetDevice(), variable, value );
					assert( SUCCEEDED( result ) );
				}
			}

			// SET PIXEL CAMERA
			if( mRef->rm_Reference.m_requiresCamera )
			{
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, "g_cameraPosition" );
				
				if ( variable != NULL )
				{
					D3DXVECTOR4 cameraPos = m_currentScene->m_camera->m_position;
					result =  pData->m_pixelShaderConstantTable->SetVector( g_RenderDevices.GetDevice(), variable, &cameraPos );
					assert( SUCCEEDED( result ) );
				}
			}

			if( true )
			{
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, "g_alpha" );
				
				if ( variable != NULL )
				{
					float value = mRef->rm_Reference.m_alpha;
					result =  pData->m_pixelShaderConstantTable->SetFloat( g_RenderDevices.GetDevice(), variable, value );
					assert( SUCCEEDED( result ) );
				}
			}
			else
			{
				D3DXHANDLE variable;
				D3DXHANDLE topLevelVariable = NULL;

				variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, "g_alpha" );
				
				if ( variable != NULL )
				{
					float value = 1.0f;
					result =  pData->m_pixelShaderConstantTable->SetFloat( g_RenderDevices.GetDevice(), variable, value );
					assert( SUCCEEDED( result ) );
				}
			}
		}

		//far plane
		topLevelVariable = NULL;

		variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, "g_farPlaneDistance" );
				
		if ( variable != NULL )
		{
			float value = mRef->rm_Reference.m_alpha;
			result =  pData->m_pixelShaderConstantTable->SetFloat( g_RenderDevices.GetDevice(), variable, Scene::GetCamera()->FarPlaneDistance() );
			assert( SUCCEEDED( result ) );
		}

		//Set the textures mRef->rm_Reference.m_textureMap.begin();
		for( unsigned t = 0; t < mRef->rm_Reference.m_textureNames.size(); t++ )
		{

			char* variableName =  mRef->rm_Reference.m_textureNames[t];
			LPDIRECT3DTEXTURE9 texture = mRef->rm_Reference.m_textures[t]->rm_Reference;

			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;

			variable = pData->m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				UINT index =  pData->m_pixelShaderConstantTable->GetSamplerIndex( variable );

				g_RenderDevices.GetDevice()->SetSamplerState( index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				g_RenderDevices.GetDevice()->SetSamplerState( index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				g_RenderDevices.GetDevice()->SetSamplerState( index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

				g_RenderDevices.GetDevice()->SetTexture( index, texture);
				assert( SUCCEEDED( result ) );
			}
		}

		// hack hack hack
		if ( currentEntity && currentEntity->m_mesh->m_reference->rm_Reference.m_skeletonInstance)
		{
			int count = currentEntity->m_mesh->m_reference->rm_Reference.m_skeletonInstance->m_DeltaBoneTransform.Count();
			if (count > 0)
			{
				variable = vData->m_vertexShaderConstantTable->GetConstantByName(topLevelVariable, "g_bones");

				if (variable != NULL)
				{
					GMatrix4* begin = &currentEntity->m_mesh->m_reference->rm_Reference.m_skeletonInstance->m_DeltaBoneTransform[0];
					int count = currentEntity->m_mesh->m_reference->rm_Reference.m_skeletonInstance->m_DeltaBoneTransform.Count();
					vData->m_vertexShaderConstantTable->SetMatrixArray(g_RenderDevices.GetDevice(), variable, (D3DXMATRIX*)begin, count);
					assert(SUCCEEDED(result));
				}

				//GMatrix4* begin = &currentEntity->m_mesh->m_reference->rm_Reference.m_skeletonInstance->m_DeltaBoneTransform[0];
				//result = g_RenderDevices.GetDevice()->SetVertexShaderConstantF(8, (float*)begin, count * 4);
			}
		}
	}

#ifdef _DEBUG
	D3DPERF_EndEvent( );
	D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"New Entity");
#endif

	// WORLD MATRIX
	{
		D3DXMATRIX scale;
		D3DXMATRIX worldRotation;
		D3DXMATRIX worldTranslation;
		D3DXMATRIX worldTransformation;
		if( pass == 6 )
		{
#ifdef _DEBUG
			D3DPERF_EndEvent( );
			D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"New UI");
#endif
			D3DXMatrixScaling( &scale, currentEntity->m_scale.x / m_textureWidth, currentEntity->m_scale.y / m_textureHeight, 1.0f );
			D3DXMatrixRotationY( &worldRotation, 0.0f );
			D3DXMatrixTranslation( &worldTranslation, currentEntity->m_position.x / m_textureWidth, currentEntity->m_position.y / m_textureHeight, currentEntity->m_position.z );
			worldTransformation = scale* worldRotation * worldTranslation;
		}
		else
		{
			D3DXMatrixScaling( &scale, currentEntity->m_scale.x, currentEntity->m_scale.y, currentEntity->m_scale.z );
			D3DXMatrixRotationY( &worldRotation, currentEntity->m_orientationY );
			D3DXMatrixTranslation( &worldTranslation, currentEntity->m_position.x, currentEntity->m_position.y, currentEntity->m_position.z );
			if( currentEntity->m_cameraBound )
			{
				D3DXMATRIX cameraXZTransTransform;
				D3DXMatrixTranslation( &cameraXZTransTransform, m_currentScene->GetCamera()->m_position.x, 0.0f, m_currentScene->GetCamera()->m_position.z );
				worldTransformation = scale* worldRotation * worldTranslation ;
				worldTransformation *= cameraXZTransTransform;
			}
			else
			{
				worldTransformation = scale* worldRotation * worldTranslation;
			}
		}

		D3DXMatrixTranspose( &worldTransformation, &worldTransformation );
		result = g_RenderDevices.GetDevice()->SetVertexShaderConstantF( 0, (float*) &worldTransformation, 4 );
		assert( SUCCEEDED( result ) );
	}

	// DRAW DEBUG LINES.
	//currentEntity->Draw();
}


// fill these in later, i guess.
void RenderManager::DrawLine( D3DXVECTOR3* i_vertexList, D3DXMATRIX i_transform, D3DXCOLOR i_color )
{
	
}

void RenderManager::DrawHudBox( float i_topLeftXP, float i_topLeftYP, float i_widthP, float i_heightP, D3DXCOLOR i_color )
{
}

void RenderManager::Cleanse()
{
	Clear();
	if( m_currentScene )
		m_currentScene->Unload();

	// clean up the managers.
	ResourceManager<MaterialReference>::m_resourceMap.clear();
	ResourceManager<PixelShaderReference>::m_resourceMap.clear();
	ResourceManager<VertexShaderReference>::m_resourceMap.clear();
	ResourceManager<TextureReference>::m_resourceMap.clear();
	ResourceManager<EffectReference>::m_resourceMap.clear();
	ResourceManager<EnviroMapReference>::m_resourceMap.clear();
	ResourceManager<MeshReference>::m_resourceMap.clear();
}

void RenderManager::Clear()
{
	m_keyList.clear();
	m_depthBucket.clear();
	m_postList.clear();
	m_alphaBucket.clear();
	m_UIList.clear();

	m_lines.Reset();
	m_hud.Reset();
}

