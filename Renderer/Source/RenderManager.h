#ifndef __RENDER_MANAGER_H_
#define __RENDER_MANAGER_H_

#include <vector>
#include "ResourceManager.h"
#include "ResourcePtr.h"
#include "ResourceTypes/Material.h"
#include "ResourceTypes/Effect.h"
#include "Scene.h"
#include "ResourceTypes/Lines.h"
#include "ResourceTypes/HUDElements.h"
#include "Utility/Singleton.h"

class Camera;
class Entity;

typedef unsigned __int64 SortKey;

struct SortElement
{
	SortElement( SortKey i_sortKey, Entity* i_entity ): se_key( i_sortKey ), se_entity( i_entity )
	{
	}
	SortKey		se_key;
	Entity*		se_entity;
};

struct AlphaSortElement
{
	float a_distanceSquared;
	Entity* a_entity;
};

class RenderManager : public Singleton<RenderManager>
{
public:
	// sets the sort key for the entity...may switch this over later to work differently...
									RenderManager();
									~RenderManager();
	void							GenerateSortKey( Entity* i_entity );
	void							Submit( Entity* i_entity );
	void							Cleanse(); // Remove all data.
	void							Clear( void );
	void							Initialize( int i_textureWidth, int i_textureHeight );
	void							Shutdown( void );
	Scene*							GetCurrentScene( void ) { return m_currentScene; }
	void							Present( void ); // swap buffers...

	void							Update( float i_dt ); // adds scene entities...
	void							Sort( void );
	void							Draw( void );
	void							DrawLine( D3DXVECTOR3* i_vertexList, D3DXMATRIX i_transform, D3DXCOLOR i_color );
	void							DrawHudBox( float i_topLeftXP, float i_topLeftYP, float i_widthP, float i_heightP, D3DXCOLOR i_color );
	void							SetSceneConstants( );
	void							SetSceneOrthoConstants( ); // kinda hacky.
	void							SetUniformData( Entity* currentEntity, ResourcePtr<EffectReference> i_effectRef, ResourcePtr<MaterialReference> mRef, bool sameEffect, bool sameMaterial, int pass );

	IDirect3DSurface9* 				m_backBuffer;
	Lines							m_lines;
	HudElements						m_hud;
	D3DCOLOR						m_clearColor;

	// These should be updated as the window is resized...would be nice to rely on g_mainWindow, but it doesn't exist with the map editor.
	int								m_textureWidth;
	int								m_textureHeight;

private:

	std::vector<AlphaSortElement>	m_alphaBucket;
	std::vector<SortElement>		m_keyList;
	std::vector<SortElement>		m_postList;
	std::vector<SortElement>		m_depthBucket;
	std::vector<Entity*>			m_UIList;
	IDirect3DTexture9*				m_opaqueTexture;
	IDirect3DTexture9*				m_depthTexture;
	IDirect3DTexture9*				m_finalBuffer;
	IDirect3DTexture9*				m_shadowTexture;

	Scene*							m_currentScene;
};

#define g_RenderManager Singleton<RenderManager>::Get()

#endif