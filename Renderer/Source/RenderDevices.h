#ifndef __RENDERDEVICES_H_
#define __RENDERDEVICES_H_

#include <d3d9.h>
#include <D3DX9Shader.h>

#include <Windows.h>

class RenderDevices
{
public:

	//I think this function actually gets compiled out in optimized?  If not, kill the class...
	static inline RenderDevices& GetSingleton()
	{
		static RenderDevices singleton;
		return singleton;
	}

		
									~RenderDevices();
	bool							Initialize( HWND i_windowId, int i_backBufferWidth, int i_backBufferHeight );
	bool							InitializeVertexDeclarataions();
	bool							ResetDevice( );
	bool							Shutdown();

	IDirect3D9*						GetD3DInterface() { return m_direct3dInterface; }
	IDirect3DDevice9*				GetDevice() { return m_direct3dDevice; }


	//todo: come up with better way of setting vertex declarations
	IDirect3DVertexDeclaration9*	GetDefaultVertexDeclaration() { return m_vertexDeclaration; }

private:

	IDirect3D9*						m_direct3dInterface;
	IDirect3DDevice9*				m_direct3dDevice;
	D3DPRESENT_PARAMETERS			m_presentationParameters;

	// Ideally a game would have many different vertex declarations,
	// but we will only use a single one for our class
	IDirect3DVertexDeclaration9*	m_vertexDeclaration;

	bool InitializeVertexDeclaration();
};

#define g_RenderDevices RenderDevices::GetSingleton()

#endif