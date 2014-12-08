#include "RenderDevices.h"
#include "VertexDeclarations.h"
#include "MainWindow.h"
#include <assert.h>



RenderDevices::~RenderDevices()
{
	Shutdown();
}

bool RenderDevices::Initialize( HWND i_windowId, int i_backBufferWidth, int i_backBufferHeight )
{
	// D3D_SDK_VERSION is #defined by the Direct3D header files,
	// and is just a way to make sure that everything is up-to-date
	m_direct3dInterface = Direct3DCreate9( D3D_SDK_VERSION );
	if ( m_direct3dInterface )
	{
		// Created a Direct3D9 interface
	}
	else
	{
		// Failed to create a Direct3D9 interface
		assert( 0 );
	}

	// Create an interface to a Direct3D device
	{
		UINT useDefaultDevice = D3DADAPTER_DEFAULT;
		D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
		DWORD useHardwareVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		m_presentationParameters;
		{
			m_presentationParameters.BackBufferWidth = i_backBufferWidth;
			m_presentationParameters.BackBufferHeight = i_backBufferHeight;
			m_presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
			m_presentationParameters.BackBufferCount = 1;
			m_presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
			m_presentationParameters.SwapEffect = D3DSWAPEFFECT_COPY;
			m_presentationParameters.hDeviceWindow = i_windowId;
			m_presentationParameters.Windowed = TRUE; // true for now... we have this set in UserSettings.
			m_presentationParameters.EnableAutoDepthStencil = TRUE;
			m_presentationParameters.AutoDepthStencilFormat = D3DFMT_D16;
			m_presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}
		HRESULT result = m_direct3dInterface->CreateDevice( useDefaultDevice, useHardwareRendering,
			i_windowId, useHardwareVertexProcessing, &m_presentationParameters, &m_direct3dDevice );

		if ( !SUCCEEDED( result ) )
		{
			//LogMessage( "Failed to create a Direct3D device" );
			return false;
		}
	}

	if( !InitializeVertexDeclarataions() )
		return false;

	return true;
}

bool RenderDevices::ResetDevice()
{
	HRESULT result;
	if( m_direct3dInterface )
		m_direct3dInterface->Release();

	if( m_direct3dDevice )
		m_direct3dDevice->Reset( &m_presentationParameters );

	return true;
}

bool RenderDevices::InitializeVertexDeclarataions()
{
	//Renderer::s_vertexColorElements

	// CLEAN UP
	//set cap
	D3DCAPS9 caps;
	m_direct3dDevice->GetDeviceCaps( &caps );
	//caps.MaxPrimitiveCount = 150000;
	//caps.MaxVertexIndex = 400000000;

	HRESULT result = m_direct3dDevice->CreateVertexDeclaration( s_vertexTextureNormalTangentBinormalElements, &m_vertexDeclaration );
	if ( !SUCCEEDED( result ) )
	{
		// Failed to create the vertex declaration
		assert( 0 );
		return false;
	}

	result = m_direct3dDevice->CreateVertexDeclaration(s_SkinnedVertexTextureNormalTangentBinormalElements, &m_SkinnedVertexDeclaration);
	if (!SUCCEEDED(result))
	{
		// Failed to create the vertex declaration
		assert(0);
		return false;
	}

	result = m_direct3dDevice->SetVertexDeclaration( m_vertexDeclaration );
	if ( SUCCEEDED( result ) )
		return true;



	return false;
}


bool RenderDevices::Shutdown()
{
	bool wereThereAnyErrors = false;

	if ( m_vertexDeclaration )
	{
		m_vertexDeclaration->Release();
		m_vertexDeclaration = NULL;
		//LogMessage( "Released the vertex declaration" );
	}

	if ( m_direct3dDevice )
	{
		m_direct3dDevice->Release();
		m_direct3dDevice = NULL;
		//LogMessage( "Released the Direct3D device" );
	}

	if ( m_direct3dInterface )
	{
		m_direct3dInterface->Release();
		m_direct3dInterface = NULL;
		//LogMessage( "Released the Direct3D9 interface" );
	}

	return !wereThereAnyErrors;
}


