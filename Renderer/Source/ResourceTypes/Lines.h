#ifndef __LINES_H_
#define __LINES_H_

#include "VertexShader.h"
#include "PixelShader.h"

#include "Effect.h"

#include <d3d9.h>
#include <D3DX9Shader.h>


class g3DLineVertex
{
public :
	g3DLineVertex()
	{
		m_aspectRatio = 1.77f; // 16:9
		m_startRadius = 1.0f;
		m_endRadius = 1.0f;

		m_weights[0] = 1.0f;
		m_weights[1] = 1.0f;
		m_weights[2] = 1.0f;
		m_weights[3] = 1.0f;
	} 

	g3DLineVertex(D3DXVECTOR3 p0, D3DXVECTOR3 p1, float startRadius, float endRadius, float aspectRatio, float w0, float w1, float w2, float w3 , D3DCOLOR c0) :
		m_p0(p0),
		m_p1(p1),
		m_startRadius(startRadius),
		m_endRadius(endRadius),
		m_aspectRatio(aspectRatio),
		m_color(c0)
	{
		m_weights[0] = w0;
		m_weights[1] = w1;
		m_weights[2] = w2;
		m_weights[3] = w3;
	}
	

	D3DXVECTOR3     m_p0;
	D3DXVECTOR3     m_p1;
	float           m_weights[4];
	float           m_startRadius;
	float           m_endRadius;
	float           m_aspectRatio;
	D3DCOLOR		m_color;
};

class Lines
{
public:
									Lines( int i_maxLines );
									~Lines( );

	bool							AddLine( D3DXVECTOR3 p0, D3DXVECTOR3 p1, float startRadius, float endRadius, float aspect, D3DXCOLOR color0 );
	//bool							AddOrthoLine
	bool							Render( const D3DXMATRIX& matProj, const D3DXMATRIX& matView, const D3DXMATRIX& matWorld, int bZenable );
	bool							Reset();
	bool							OnResetDevice();
	bool							OnLostDevice();
	void							Cleanse( );

	int								m_maxVertices;
	int								m_maxIndices;
	int								m_maxLines;
	int								m_bufferSize;
	int								m_currentVertices;
	int								m_currentLines;
	int								m_bufferOffset;
	g3DLineVertex*					m_lineVertices;
	unsigned __int16*				m_lineIndices;

	LPDIRECT3DVERTEXBUFFER9         m_pVBuff;
	LPDIRECT3DINDEXBUFFER9          m_pIBuff;

	//we're basically using the same "effect" for all of these, so let's just store the shaders and set the constants manually...
	ResourcePtr<VertexShaderReference>		m_vertexShader;
	ResourcePtr<PixelShaderReference>		m_pixelShader;

	IDirect3DVertexDeclaration9*	m_vertexDeclaration; // vertex declaration for debug line vertices.

	bool							m_dirty;

};


#endif