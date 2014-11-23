#ifndef __PIXELSHADER_H_
#define __PIXELSHADER_H_

#include "../ResourceLoader.h"
#include "../ResourceManager.h"
#include "../ResourceMaster.h"
#include "../RenderDevices.h"

#include <d3d9.h>
#include <D3DX9Shader.h>

#include <assert.h>

class PixelShaderData
{
public:

	IDirect3DPixelShader9*				m_pixelShader;
	ID3DXConstantTable*					m_pixelShaderConstantTable;
};

class PixelShaderReference : public ResourceMaster<PixelShaderData>
{
public:
	PixelShaderReference()
	{

		int stop = 0;
	}
};

class PixelShaderLoader : public ResourceLoader<PixelShaderReference>
{
	PixelShaderReference*	Load( const char* i_filename )
	{
		//DebugBreak();
		//void* somethn = malloc( sizeof( PixelShaderReference ) );

		PixelShaderReference* reference = new PixelShaderReference();

		void* compiledFragmentShader;
		{
			// Open the file
			char fileName[128];
			sprintf( fileName, "data//FragmentShaders//%s", i_filename );
			reference->rm_Filename = fileName;

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
					assert( 0 );
					//cs6963::LogMessage( std::string( "Failed to open fragment shader " ) + fileName );
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
					assert( 0 );
					//cs6963::LogMessage( std::string( "Couldn't get the size of fragment shader " ) + fileName );
					CloseHandle( fileHandle );
					return false;
				}
			}
			// Read the file's contents into temporary memory
			compiledFragmentShader = malloc( fileSize );
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				if ( ReadFile( fileHandle, compiledFragmentShader, fileSize,
					&bytesReadCount, readSynchronously ) == TRUE )
				{
					CloseHandle( fileHandle );
				}
				else
				{
					assert( 0 );
					//cs6963::LogMessage( std::string( "Couldn't read the contents of fragment shader " ) + fileName );
					free( compiledFragmentShader );
					CloseHandle( fileHandle );
					return false;
				}
			}
		}
		// Create the fragment shader object
		{
			HRESULT result = g_RenderDevices.GetDevice()->CreatePixelShader( reinterpret_cast<DWORD*>( compiledFragmentShader ), &reference->rm_Reference.m_pixelShader );
			if ( SUCCEEDED( result ) )
			{
				// Get the constant table from the fragment shader
				result = D3DXGetShaderConstantTable( reinterpret_cast<DWORD*>( compiledFragmentShader ), &reference->rm_Reference.m_pixelShaderConstantTable );
				if ( SUCCEEDED( result ) )
				{
					// Set the default values
					result = reference->rm_Reference.m_pixelShaderConstantTable->SetDefaults( g_RenderDevices.GetDevice() );
					if ( FAILED( result ) )
					{
						assert( 0 );
						//cs6963::LogMessage( "Couldn't set the fragment shader constant defaults" );
					}
				}
				free( compiledFragmentShader );
				if ( FAILED( result ) )
				{
					assert( 0 );
					//cs6963::LogMessage( "Couldn't get the fragment shader constant table" );
					return false;
				}
			}
			else
			{
				assert( 0 );
				//cs6963::LogMessage( "Couldn't create the fragment shader object" );
				free( compiledFragmentShader );
				return false;
			}
		}

		return reference;
	}
};


#endif