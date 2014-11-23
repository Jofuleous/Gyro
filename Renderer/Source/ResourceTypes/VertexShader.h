#ifndef __VERTEXSHADER_H_
#define __VERTEXSHADER_H_

#include "../ResourceLoader.h"
#include "../ResourceManager.h"
#include "../ResourceMaster.h"
#include "../RenderDevices.h"
#include "../RenderDevices.h"

#include <assert.h>
#include <d3d9.h>
#include <D3DX9Shader.h>

class VertexShaderData
{
public:

	void Unload( )
	{
		m_vertexShader->Release();
		m_vertexShaderConstantTable->Release();
	}

	IDirect3DVertexShader9*				m_vertexShader;
	ID3DXConstantTable*					m_vertexShaderConstantTable;
};

class VertexShaderReference : public ResourceMaster<VertexShaderData>
{

};

class VertexShaderLoader : public ResourceLoader<VertexShaderReference>
{
	VertexShaderReference* Load( const char* i_filename )
	{
		//DebugBreak();

		VertexShaderReference* reference = new VertexShaderReference();

		// LOAD AND STORE THE COMPILED VERTEX SHADER IN VRAM
		void* compiledVertexShader;
		{
			// Open the file
			char fileName[128];
			sprintf( fileName, "data//VertexShaders//%s", i_filename );

			HANDLE fileHandle;
			{
				DWORD desiredAccess = GENERIC_READ;
				DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
				SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
				DWORD onlySucceedIfFileExists = OPEN_EXISTING;
				DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
				HANDLE dontUseTemplateFile = NULL;
				fileHandle = CreateFileA( fileName, desiredAccess, otherProgramsCanStillReadTheFile,
					useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile );
				if ( fileHandle == INVALID_HANDLE_VALUE )
				{
					//cs6963::LogMessage( std::string( "Failed to open vertex shader " ) + fileName );
					return false;
				}
			}
			// Get the file's size
			DWORD fileSize;
			{
				LARGE_INTEGER fileSize_integer;
				if ( GetFileSizeEx( fileHandle, &fileSize_integer ) == TRUE )
				{
					// This is unsafe, because the file's size could be bigger than a DWORD
					fileSize = static_cast<DWORD>( fileSize_integer.QuadPart );
				}
				else
				{
					//cs6963::LogMessage( std::string( "Couldn't get the size of vertex shader " ) + fileName );
					CloseHandle( fileHandle );
					return false;
				}
			}
			// Read the file's contents into temporary memory
			compiledVertexShader = malloc( fileSize );
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				if ( ReadFile( fileHandle, compiledVertexShader, fileSize,
					&bytesReadCount, readSynchronously ) == TRUE )
				{
					CloseHandle( fileHandle );
				}
				else
				{
					//cs6963::LogMessage( std::string( "Couldn't read the contents of vertex shader " ) + fileName );
					free( compiledVertexShader );
					CloseHandle( fileHandle );
					return false;
				}
			}
		}
		// Create the vertex shader object
		{
			HRESULT result = g_RenderDevices.GetDevice()->CreateVertexShader( reinterpret_cast<DWORD*>( compiledVertexShader ), &reference->rm_Reference.m_vertexShader );
			if ( SUCCEEDED( result ) )
			{
				// Get the constant table from the vertex shader
				result = D3DXGetShaderConstantTable( reinterpret_cast<DWORD*>( compiledVertexShader ), &reference->rm_Reference.m_vertexShaderConstantTable );
				if ( SUCCEEDED( result ) )
				{
					// Set the default values
					result = reference->rm_Reference.m_vertexShaderConstantTable->SetDefaults( g_RenderDevices.GetDevice() );
					if ( FAILED( result ) )
					{
						assert( 0 );
						//cs6963::LogMessage( "Couldn't set the vertex shader constant defaults" );
					}
				}
				free( compiledVertexShader );
				if ( FAILED( result ) )
				{
					assert( 0 );
					//cs6963::LogMessage( "Couldn't get the vertex shader constant table" );
					return false;
				}
			}
			else
			{
				assert( 0 );
				//cs6963::LogMessage( "Couldn't create the vertex shader object" );
				free( compiledVertexShader );
				return false;
			}
		}

		return reference;	
	}
};

#endif