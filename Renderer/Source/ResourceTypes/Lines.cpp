#include "Lines.h"
#include "../RenderDevices.h"
#include "../VertexDeclarations.h"
#include "../RenderManager.h"
#include "../Camera.h"

Lines::Lines( int i_maxLines )
{

	m_maxVertices = i_maxLines * 4; // 8 vertices per line.
	m_bufferSize = sizeof( g3DLineVertex ) * m_maxVertices;
	m_currentVertices = 0;
	m_currentLines = 0;
	m_maxLines = i_maxLines;
	m_lineVertices = (g3DLineVertex*) malloc( m_bufferSize );
	//m_orthoLineVertices = (g3DLineVertex*) malloc( m_bufferSize );
	m_maxIndices = i_maxLines * 6;

}

Lines::~Lines()
{
	Cleanse( );
}

// todo: convert to shared matrix class?
bool Lines::AddLine( D3DXVECTOR3 p0, D3DXVECTOR3 p1, float startRadius, float endRadius, float aspect, D3DXCOLOR i_color )
{
	// Check for overflow.
	if ( m_currentVertices + 4 >= m_maxVertices )
		return false;

	m_lineVertices[m_currentVertices + 0] = g3DLineVertex(p0, p1, startRadius, endRadius, aspect, 1.0f, 0.0f, -1.0f, -1.0f, i_color);
	m_lineVertices[m_currentVertices + 1] = g3DLineVertex(p0, p1, startRadius, endRadius, aspect, 1.0f, 0.0f, -1.0f,  1.0f, i_color);
	m_lineVertices[m_currentVertices + 2] = g3DLineVertex(p0, p1, startRadius, endRadius, aspect, 0.0f, 1.0f,  0.0f, -1.0f, i_color);
	m_lineVertices[m_currentVertices + 3] = g3DLineVertex(p0, p1, startRadius, endRadius, aspect, 0.0f, 1.0f,  0.0f,  1.0f, i_color);

	m_currentVertices += 4;	//8;
	m_dirty = true; // we need to do a new memcpy.

	return true;
}


bool Lines::Render( const D3DXMATRIX& matProj, const D3DXMATRIX& matView, const D3DXMATRIX& matWorld, int bZenable )
{
#ifdef _DEBUG
	D3DPERF_EndEvent( );
	D3DPERF_BeginEvent( D3DCOLOR_XRGB(255, 0, 0 ), L"DRAWING LINES");
#endif

	if( m_currentVertices <= 0 )
		return true;

	if (m_dirty )	// Copy local vertex data into VB if it is new.
	{
		g3DLineVertex  *pVerts = NULL;
		m_pVBuff->Lock(0, 0, (void**)&pVerts, 0);
		memcpy(pVerts, m_lineVertices, m_currentVertices * sizeof(g3DLineVertex));

		m_pVBuff->Unlock();

		m_dirty = false;    //vert data isn't new anymore
	}

	HRESULT hr;


	hr = g_RenderDevices.GetDevice()->SetVertexShader( m_vertexShader->rm_Reference.m_vertexShader );
	//assert( SUCCEEDED( result ) );
	hr = g_RenderDevices.GetDevice()->SetPixelShader( m_pixelShader->rm_Reference.m_pixelShader );
	//assert( SUCCEEDED( result )

	// Draw the lines.
	//hr = g_RenderDevices.GetDevice()->SetIndices(m_pIBuff);
	hr = g_RenderDevices.GetDevice()->SetIndices( m_pIBuff );
	hr = g_RenderDevices.GetDevice()->SetStreamSource(0, m_pVBuff, 0, sizeof(g3DLineVertex));
	hr = g_RenderDevices.GetDevice()->SetVertexDeclaration( m_vertexDeclaration );
	//g_RenderDevices.GetDevice()->SetStreamSource( 0, 0, 0, 0 );
	//g_RenderDevices.GetDevice()->SetVertexDeclaration( 0 );
	

	// we're already in world coords, just pass in the view * projection
	D3DXMATRIX matWorldViewProjection = g_RenderManager.GetCurrentScene()->GetCamera()->ViewTransform() * g_RenderManager.GetCurrentScene()->GetCamera()->ProjectionTransform();
	D3DXHANDLE hTemp = m_vertexShader->rm_Reference.m_vertexShaderConstantTable->GetConstantByName( 0, "gWVP" );
	

	if( hTemp != NULL )
		hr = m_vertexShader->rm_Reference.m_vertexShaderConstantTable->SetMatrix(g_RenderDevices.GetDevice(), hTemp, &matWorldViewProjection);
	else
		assert( 0 );
	
	//hr = g_RenderDevices.GetDevice()->SetRenderState(D3DRS_ZENABLE, false);
	//m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);  //this was just for debugging to make sure the tri's draw correctly

	//Render
	hr = g_RenderDevices.GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_currentVertices, 0, m_currentVertices / 2 );

	//m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	return true;
}

void Lines::Cleanse( )
{
	free( m_lineIndices );
	free( m_lineVertices );
	m_pVBuff->Release();
	m_pIBuff->Release();	
}

bool Lines::Reset( )
{
	// set numbert of verts to zero and mark the buffer as dirty.
	m_currentVertices = 0;
	m_dirty = true;
	return true;
}

bool Lines::OnResetDevice( )
{
	//DebugBreak();
	PixelShaderLoader pLoader;
	m_pixelShader = ResourceManager<PixelShaderReference>::Load( "debugLinesPS.fp", &pLoader, false );

	VertexShaderLoader loader;
	m_vertexShader = ResourceManager<VertexShaderReference>::Load( "debugLinesVS.vp", &loader, false );

	m_lineIndices = (unsigned __int16*) malloc( m_maxIndices * sizeof( unsigned __int16));

	int index = 0;
	for( int i = 0; i < m_maxLines; i++ )
	{
		m_lineIndices[ index++ ] = i * 4;
		m_lineIndices[ index++ ] = i * 4 + 2;
		m_lineIndices[ index++ ] = i * 4 + 3;
		m_lineIndices[ index++ ] = i * 4;
		m_lineIndices[ index++ ] = i * 4 + 3;
		m_lineIndices[ index++ ] = i * 4 + 1;
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
	memcpy( indices, m_lineIndices, m_maxIndices );

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
				return false;
			}
		}
	}

	result = g_RenderDevices.GetDevice()->CreateVertexBuffer( m_maxVertices * sizeof( g3DLineVertex ), usage, 0, D3DPOOL_DEFAULT,
	&m_pVBuff, 0 );
	

	result = g_RenderDevices.GetDevice()->CreateVertexDeclaration( s_gDebugLineVertexDecl, &m_vertexDeclaration );
	if( !SUCCEEDED( result ) )
		assert( 0 );

	return true;
}