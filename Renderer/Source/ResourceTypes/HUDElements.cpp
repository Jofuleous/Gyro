#include "HUDElements.h"

#include "../MainWindow.h"
#include "../VertexDeclarations.h"
#include "D3d9types.h"
#include "../RenderManager.h"
#include "Entity.h"

/*
For now, let's just make it dirty to where boxes have to register every frame and the vertex buffer gets re-created.
*/

static const unsigned int		s_FontNameWCHARLen = 128;
static const char*					s_FontName = "Arial";

HudElements::HudElements( int i_maxBoxes )
{
	m_maxBoxes = i_maxBoxes;
	m_maxVertices = i_maxBoxes * 4;
	m_maxIndices = i_maxBoxes * 6;
	m_boxVertices = (gHUDQuad*) malloc( sizeof( gHUDQuad ) * m_maxVertices );
	m_currentVertices = 0;
	m_dirty = false;

	m_sprite = NULL;
	m_font = NULL;
}

HudElements::~HudElements()
{
	free( m_boxIndices );
	free( m_boxVertices );
	m_pVBuff->Release();
	m_pIBuff->Release();
	m_sprite->Release();
	m_font->Release();

	m_text.clear();
}

void HudElements::AddHUDText( long i_left, long i_top, long i_right, long i_bottom, D3DXCOLOR i_color, const char* i_text )
{
	// for now, let's just keep a list of 

	GText text;
	RECT dims = { i_left, i_top, i_right, i_bottom };
	text.m_rectDimensions = dims;
	text.m_text = i_text;

	D3DCOLOR trans = D3DCOLOR_ARGB( (int) (i_color.a * 255.0f), (int) (i_color.r * 255.0f), (int) (i_color.g * 255.0f), (int) (i_color.b * 255.0f) );

	text.m_color = trans;

	m_text.push_back( text );

}

void HudElements::AddHUDBox( float i_topLeftXP, float i_topLeftYP, float i_bottomRightXP, float i_bottomRightYP, D3DXCOLOR i_color )
{
	if( m_currentVertices + 4 > m_maxVertices )
	{
		assert( 0 );
		return;
	}

	// this puts the coords in screen space percentages.
	float left = (i_topLeftXP * 2.0f) - 1.0f;
	float top = (i_topLeftYP * -2.0f) + 1.0f;
	float bottom = (i_bottomRightYP * -2.0f) + 1.0f;
	float right = (i_bottomRightXP * 2.0f) - 1.0f;

	m_boxVertices[m_currentVertices + 0] = gHUDQuad( D3DXVECTOR2( left, bottom ), (D3DXVECTOR4) i_color );
	m_boxVertices[m_currentVertices + 1] = gHUDQuad( D3DXVECTOR2( right, bottom ), (D3DXVECTOR4) i_color );
	m_boxVertices[m_currentVertices + 2] = gHUDQuad( D3DXVECTOR2( left, top ), (D3DXVECTOR4) i_color );
	m_boxVertices[m_currentVertices + 3] = gHUDQuad( D3DXVECTOR2( right, top ), (D3DXVECTOR4) i_color );

	m_currentVertices += 4;
	m_dirty = true;

}

void HudElements::AddHUDEntity( Entity* i_entity )
{
	m_hudEntities.push_back( i_entity );
}

void HudElements::RemoveHUDEntity( Entity* i_entity )
{
	for( unsigned i = 0; i < m_hudEntities.size(); i++ )
	{
		if( m_hudEntities[i] == i_entity )
		{
			m_hudEntities[i] = m_hudEntities.back();
			m_hudEntities.pop_back();
		}
	}
}

void HudElements::Render()
{
#ifdef _DEBUG
	D3DPERF_EndEvent( );
	D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"DRAWING HUD BOXES");
#endif

	HRESULT hr;
	hr = g_RenderDevices.GetDevice()->SetRenderState(D3DRS_ZENABLE, false);

	// big ol hack...
	if( m_hudEntities.size() > 0 )
	{
		//g_RenderManager.SetSceneOrthoConstants();

		for( int i = 0; i < m_hudEntities.size(); i++ )
		{
			g_RenderManager.SetUniformData( m_hudEntities[i], m_hudEntities[i]->m_material->rm_Reference.m_effect, m_hudEntities[i]->m_material, false, false, 1 );
			m_hudEntities[i]->Draw();
		}

		//g_RenderManager.SetSceneConstants();
	}

	if( m_currentVertices > 0 )
	{
		//return ;

		if (m_dirty )	// Copy local vertex data into VB if it is new.
		{
			gHUDQuad  *pVerts = NULL;
			m_pVBuff->Lock(0, 0, (void**)&pVerts, 0);
			memcpy(pVerts, m_boxVertices, m_currentVertices * sizeof(gHUDQuad));

			m_pVBuff->Unlock();

			m_dirty = false;    //vert data isn't new anymore
		}


		hr = g_RenderDevices.GetDevice()->SetVertexShader( m_vertexShader->rm_Reference.m_vertexShader );
		//assert( SUCCEEDED( result ) );
		hr = g_RenderDevices.GetDevice()->SetPixelShader( m_pixelShader->rm_Reference.m_pixelShader );
		//assert( SUCCEEDED( result )

		// Draw the lines.
		//hr = g_RenderDevices.GetDevice()->SetIndices(m_pIBuff);
		hr = g_RenderDevices.GetDevice()->SetIndices( m_pIBuff );
		hr = g_RenderDevices.GetDevice()->SetStreamSource(0, m_pVBuff, 0, sizeof(gHUDQuad));
		hr = g_RenderDevices.GetDevice()->SetVertexDeclaration( m_vertexDeclaration );
		//g_RenderDevices.GetDevice()->SetStreamSource( 0, 0, 0, 0 );
		//g_RenderDevices.GetDevice()->SetVertexDeclaration( 0 );

		//Render
		hr = g_RenderDevices.GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_currentVertices, 0, m_currentVertices / 2 );
	}



#ifdef _DEBUG
	D3DPERF_EndEvent( );
	D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"DRAWING HUD TEXT");
#endif

	HRESULT result = m_sprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
	assert( result == D3D_OK );

	// Render the text
	for( unsigned i = 0; i < m_text.size(); i++)
	{
		HRESULT result = m_font->DrawTextA( m_sprite, m_text[i].m_text, -1, &m_text[i].m_rectDimensions, DT_LEFT | DT_NOCLIP | DT_VCENTER | DT_SINGLELINE, m_text[i].m_color );
		assert( result != 0 );
	}



	result = m_sprite->End();
	assert( result == D3D_OK );
}

void HudElements::Reset()
{
	// clear previously drawn text.  they need to re-register.
	m_text.clear();

	m_currentVertices = 0;
	m_dirty = true;
}

void HudElements::OnDeviceReset()
{
	//DebugBreak();
	PixelShaderLoader pLoader;
	m_pixelShader = ResourceManager<PixelShaderReference>::Load( "hudBoxPS.fp", &pLoader, false );

	VertexShaderLoader loader;
	m_vertexShader = ResourceManager<VertexShaderReference>::Load( "hudVS.vp", &loader, false );

	m_boxIndices = (unsigned __int16*) malloc( m_maxIndices * sizeof( unsigned __int16));

	int index = 0;
	for( int i = 0; i < m_maxBoxes; i++ )
	{
		m_boxIndices[ index++ ] = i * 4;
		m_boxIndices[ index++ ] = i * 4 + 2;
		m_boxIndices[ index++ ] = i * 4 + 3;
		m_boxIndices[ index++ ] = i * 4;
		m_boxIndices[ index++ ] = i * 4 + 3;
		m_boxIndices[ index++ ] = i * 4 + 1;
	}

	//INITIALIZE INDEX BUFFER
	D3DFORMAT format = D3DFMT_INDEX16;
	HRESULT result = g_RenderDevices.GetDevice()->CreateIndexBuffer( m_maxIndices * sizeof( unsigned __int16), 0, format, D3DPOOL_DEFAULT,
		&m_pIBuff, 0 );


	//BEGIN INDEX READING PROCESS
	UINT16* indices;

	//LOCK INDEX BUFFER
	unsigned int lockEntireBuffer = 0;
	DWORD useDefaultLockingBehavior = 0;

	result = m_pIBuff->Lock( lockEntireBuffer, lockEntireBuffer,
		reinterpret_cast<void**>( &indices ), useDefaultLockingBehavior );
	if ( FAILED( result ) )
	{
		//cs6963::LogMessage( "Failed to lock the index buffer" );
		assert( 0 );
	}

	//FILL INDEX BUFFER
	//indices = (UINT16*) malloc( indexBufferSize );
	memcpy( indices, m_boxIndices, m_maxIndices );

	//UNLOCK INDEX BUFFER
	result =  m_pIBuff->Unlock();
	if ( FAILED( result ) )
	{
		// cs6963::LogMessage( "Failed to unlock the index buffer" );
		assert( 0 );
	}


	DWORD usage = 0;
	{
		// Our code will only ever write to the buffer
		usage |= D3DUSAGE_WRITEONLY;
		// The type of vertex processing should match what was specified when the device interface was created
		{
			D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
			HRESULT result = g_RenderDevices.GetDevice()->GetCreationParameters( &deviceCreationParameters );
			if ( SUCCEEDED( result ) )
			{
				DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
					( D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING );
				usage |= ( vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING ) ?
					0 : D3DUSAGE_SOFTWAREPROCESSING;
			}
			else
			{
				assert( 0 );
				return;
			}
		}
	}

	result = g_RenderDevices.GetDevice()->CreateVertexBuffer( m_maxVertices * sizeof( gHUDQuad ), usage, 0, D3DPOOL_DEFAULT,
	&m_pVBuff, 0 );
	

	result = g_RenderDevices.GetDevice()->CreateVertexDeclaration( s_gDebugHUDVertexDecl, &m_vertexDeclaration );
	if( !SUCCEEDED( result ) )
		assert( 0 );

	// handle sprite creation?
	if( m_font == NULL )
	{
		HRESULT result = D3DXCreateFont( g_RenderDevices.GetDevice(), 15, 0, FW_THIN, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, s_FontName, &m_font );
		assert( result == D3D_OK );
	}

	if( m_sprite == NULL )
	{
		HRESULT result = D3DXCreateSprite( g_RenderDevices.GetDevice(), &m_sprite );
		assert( result == D3D_OK );
	}

	return;
}