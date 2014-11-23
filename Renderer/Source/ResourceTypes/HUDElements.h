/*
Manages HUD boxes and font sprites.
*/

#ifndef __HUDELEMENTS_H_
#define __HUDELEMENTS_H_

#include <d3d9.h>
#include <d3dx9math.h>

#include "VertexShader.h"
#include "PixelShader.h"

#include <vector>

class Entity;

class gHUDQuad
{
public:
	gHUDQuad( D3DXVECTOR2 i_point, D3DXVECTOR4 i_color ) : m_point( i_point), m_color( i_color )
	{
	}

	D3DXVECTOR2     m_point;
	D3DXVECTOR4		m_color;
};

class GText;

class HudElements
{
public:

	// Box percentage specifications.
	HudElements( int m_maxBoxes );
	~HudElements();

	void AddHUDBox( float i_topLeftXP, float i_topLeftYP, float i_widthP, float i_heightP, D3DXCOLOR i_color );

	void AddHUDText( long i_left, long i_top, long i_right, long i_bottom, D3DXCOLOR i_color, const char* i_text );

	// Right now, this is for entites that always render on top.
	// May need a better way of handling this later.  These entities get freed by those who allocated it...they're responsible for removing from this list.
	void AddHUDEntity( Entity* i_entity );

	// Since things will not be added and removed often, we're not going to worry about the performance of this...
	void RemoveHUDEntity( Entity* i_entity );

	void Render( );
	void OnDeviceReset( );
	void Reset( );

	int			m_maxBoxes;
	int			m_maxVertices;
	int			m_maxIndices;
	int			m_currentVertices;
	bool		m_dirty;


	gHUDQuad*								m_boxVertices;
	unsigned __int16*						m_boxIndices;
	LPDIRECT3DVERTEXBUFFER9					m_pVBuff;
	LPDIRECT3DINDEXBUFFER9					m_pIBuff;
	IDirect3DVertexDeclaration9*			m_vertexDeclaration; // vertex declaration for debug line vertices.


	//we're basically using the same "effect" for all of these, so let's just store the shaders and set the constants manually...
	ResourcePtr<VertexShaderReference>		m_vertexShader;
	ResourcePtr<PixelShaderReference>		m_pixelShader;

	std::vector<GText>						m_text;
	ID3DXSprite*							m_sprite; // one sprite for now?
	std::vector<Entity*>					m_hudEntities;
	ID3DXFont*								m_font;
};


class GText
{
public:
	RECT			m_rectDimensions;
	const char *	m_text;
	D3DCOLOR		m_color;
};

#endif