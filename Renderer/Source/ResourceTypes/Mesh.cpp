#include "Mesh.h"
#include "Entity.h"
#include "../RenderDevices.h"
#include "../VertexStructs.h"
#include "Animation/GSkeleton.h"

#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

// bad...but needed for scene meshes...
#include "../RenderManager.h"

//bad...but meh...

MeshLoader* Mesh::DefaultMeshLoader = new MeshLoader();

Mesh::Mesh( ) : m_meshLoader( Mesh::DefaultMeshLoader )
{
	m_parentEntity = NULL;
}

MeshReference* StdMeshLoader::Load( const char* i_filename )
{
	MeshReference* reference = new MeshReference();

	//read vertex count
	reference->rm_Reference.m_vertexCount = m_vertices->size();
	//read triangle count
	reference->rm_Reference.m_triangleCount = m_indices->size() / 3;
	reference->rm_Reference.m_indexCount = m_indices->size();

	unsigned int vertexBufferSize = m_vertices->size() * sizeof( GVertices::s_vertexTNTB );
	unsigned int indexBufferSize = m_indices->size() * sizeof( UINT16 );

	//SETUP VERTEX BUFFER ON CARD/ADAPTER

	// The usage tells Direct3D how this vertex buffer will be used
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
				//cs6963::LogMessage( "Failed to get the device's creation parameters" );
				assert( 0 );
				return false;
			}
		}
	}
	// We will define our own vertex format
	DWORD useSeparateVertexDeclaration = 0;
	// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
	D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
	HANDLE* notUsed = NULL;


	//INITIALIZE VERTEX BUFFER
	HRESULT result = g_RenderDevices.GetDevice()->CreateVertexBuffer( vertexBufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
		&reference->rm_Reference.m_vertexBuffer, notUsed );
	if ( FAILED( result ) )
	{
		//cs6963::LogMessage( "Failed to create the vertex buffer" );
		assert( 0 );
		return false;
	}

	// BEGIN VERTEX READING PROCESS
	GVertices::s_vertexTNTB* vertexData;

	//LOCK VERTEX BUFFER
	{
		unsigned int lockEntireBuffer = 0;
		DWORD useDefaultLockingBehavior = 0;
		HRESULT result = reference->rm_Reference.m_vertexBuffer->Lock( lockEntireBuffer, lockEntireBuffer,
			reinterpret_cast<void**>( &vertexData ), useDefaultLockingBehavior );
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to lock the vertex buffer" );
			assert( 0 );
			return false;
		}
	}

	//FINALLY, FILL VERTEX BUFFER...
	//vertexData = (s_vertexColor*) malloc( vertexCount * sizeof( s_vertexColor ));
	memcpy( vertexData, &((*m_vertices)[0]), vertexBufferSize );

	//UNLOCK VERTEX BUFFER
	{
		HRESULT result = reference->rm_Reference.m_vertexBuffer->Unlock();
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to unlock the vertex buffer" );
			assert( 0 );
			return false;
		}
	}



	//BEGIN INDEX READING PROCESS
	UINT16* indices;

	//INITIALIZE INDEX BUFFER
	D3DFORMAT format = D3DFMT_INDEX16;
	notUsed = NULL;
	result = g_RenderDevices.GetDevice()->CreateIndexBuffer( indexBufferSize, usage, format, useDefaultPool,
		&reference->rm_Reference.m_indexBuffer, notUsed );
	if ( FAILED( result ) )
	{
		// cs6963::LogMessage( "Failed to create the vertex buffer" );
		assert( 0 );
		return false;
	}

	//LOCK INDEX BUFFER
	{
		unsigned int lockEntireBuffer = 0;
		DWORD useDefaultLockingBehavior = 0;

		HRESULT result = reference->rm_Reference.m_indexBuffer->Lock( lockEntireBuffer, lockEntireBuffer,
			reinterpret_cast<void**>( &indices ), useDefaultLockingBehavior );
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to lock the index buffer" );
			assert( 0 );
			return false;
		}
	}

	//FILL INDEX BUFFER
	//indices = (UINT16*) malloc( indexBufferSize );
	memcpy( indices, &((*m_indices)[0]), indexBufferSize );

	//UNLOCK INDEX BUFFER
	{
		HRESULT result = reference->rm_Reference.m_indexBuffer->Unlock();
		if ( FAILED( result ) )
		{
			// cs6963::LogMessage( "Failed to unlock the index buffer" );
			assert( 0 );
			return false;
		}
	}

	return reference;
}


MeshReference* MeshLoader::Load( const char* i_filename )
{
	MeshReference* reference = new MeshReference();

	//OPEN THE MESH FILE
	FILE* file;
	int size;
	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;
	unsigned int boneCount = 0;
	unsigned int bytesRead = 0;

	char filepath[128];
	sprintf( filepath, "data//Meshes//%s", i_filename );
	file = fopen( filepath, "r+b" );
	if( !file )
	{
		return false;
	}

	fseek( file, 0L, SEEK_END );
	size = ftell( file );
	fseek( file, 0L, SEEK_SET);

	//read vertex count
	fread( &reference->rm_Reference.m_vertexCount, sizeof(unsigned), 1, file );
	//read triangle count
	fread( &reference->rm_Reference.m_triangleCount, sizeof( unsigned), 1, file ) ;
	// vertex type
	unsigned vertexType;
	fread(&vertexType, sizeof(unsigned), 1, file);
	// read the bone count
	fread( &boneCount, sizeof( unsigned ), 1, file );
	reference->rm_Reference.m_indexCount = reference->rm_Reference.m_triangleCount * 3;


	unsigned int vertexBufferSize;
	if ( vertexType == 0)
		vertexBufferSize = reference->rm_Reference.m_vertexCount * sizeof(GVertices::s_vertexTNTB);
	else
		vertexBufferSize = reference->rm_Reference.m_vertexCount * sizeof(GVertices::s_SkinnedVertexTNTB);
	unsigned int indexBufferSize = reference->rm_Reference.m_indexCount * sizeof( UINT16 );

	//SETUP VERTEX BUFFER ON CARD/ADAPTER

	// The usage tells Direct3D how this vertex buffer will be used
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
				//cs6963::LogMessage( "Failed to get the device's creation parameters" );
				fclose( file );
				assert( 0 );
				return false;
			}
		}
	}
	// We will define our own vertex format
	DWORD useSeparateVertexDeclaration = 0;
	// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
	D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
	HANDLE* notUsed = NULL;


	//INITIALIZE VERTEX BUFFER
	HRESULT result = g_RenderDevices.GetDevice()->CreateVertexBuffer( vertexBufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
		&reference->rm_Reference.m_vertexBuffer, notUsed );
	if ( FAILED( result ) )
	{
		//cs6963::LogMessage( "Failed to create the vertex buffer" );
		fclose( file );
		assert( 0 );
		return false;
	}



	// BEGIN VERTEX READING PROCESS
	GVertices::s_vertexTNTB* vertexData;
	GVertices::s_SkinnedVertexTNTB* skinnedVertexData;

	//LOCK VERTEX BUFFER
	{
		unsigned int lockEntireBuffer = 0;
		DWORD useDefaultLockingBehavior = 0;
		if (vertexType == 0)
		{
			HRESULT result = reference->rm_Reference.m_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
				reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
		}
		else if (vertexType == 1)
		{
			HRESULT result = reference->rm_Reference.m_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
				reinterpret_cast<void**>(&skinnedVertexData), useDefaultLockingBehavior);
		}
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to lock the vertex buffer" );
			fclose( file );
			assert( 0 );
			return false;
		}
	}

	//FINALLY, FILL VERTEX BUFFER...
	//vertexData = (s_vertexColor*) malloc( vertexCount * sizeof( s_vertexColor ));
	if ( vertexType == 0 )
		fread( vertexData, sizeof( GVertices::s_vertexTNTB ), reference->rm_Reference.m_vertexCount, file );
	else if ( vertexType == 1 )
		fread( skinnedVertexData, sizeof(GVertices::s_SkinnedVertexTNTB), reference->rm_Reference.m_vertexCount, file);

	//UNLOCK VERTEX BUFFER
	{
		HRESULT result = reference->rm_Reference.m_vertexBuffer->Unlock();
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to unlock the vertex buffer" );
			fclose( file );
			assert( 0 );
			return false;
		}
	}



	//BEGIN INDEX READING PROCESS
	UINT16* indices;

	//INITIALIZE INDEX BUFFER
	D3DFORMAT format = D3DFMT_INDEX16;
	notUsed = NULL;
	result = g_RenderDevices.GetDevice()->CreateIndexBuffer( indexBufferSize, usage, format, useDefaultPool,
		&reference->rm_Reference.m_indexBuffer, notUsed );
	if ( FAILED( result ) )
	{
		// cs6963::LogMessage( "Failed to create the vertex buffer" );
		fclose( file );
		assert( 0 );
		return false;
	}

	//LOCK INDEX BUFFER
	{
		unsigned int lockEntireBuffer = 0;
		DWORD useDefaultLockingBehavior = 0;

		HRESULT result = reference->rm_Reference.m_indexBuffer->Lock( lockEntireBuffer, lockEntireBuffer,
			reinterpret_cast<void**>( &indices ), useDefaultLockingBehavior );
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to lock the index buffer" );
			fclose( file );
			assert( 0 );
			return false;
		}
	}

	//FILL INDEX BUFFER
	//indices = (UINT16*) malloc( indexBufferSize );
	fread( indices, sizeof(UINT16), reference->rm_Reference.m_indexCount, file );

	//UNLOCK INDEX BUFFER
	{
		HRESULT result = reference->rm_Reference.m_indexBuffer->Unlock();
		if ( FAILED( result ) )
		{
			// cs6963::LogMessage( "Failed to unlock the index buffer" );
			fclose( file );
			assert( 0 );
			return false;
		}
	}

	// now let's read in the bones, if there are any.
	if (boneCount > 0)
	{
		reference->rm_Reference.m_skeletonInstance = GSkeleton::LoadSkeleton(i_filename, file, boneCount);
	}
	else
		reference->rm_Reference.m_skeletonInstance = NULL;

	fclose( file );

	return reference;
}

void Mesh::Draw( void )
{
	HRESULT result;
	// Bind a specific vertex buffer to the device as a data source
	{
		// There can be multiple streams of data feeding the display adaptor at the same time
		unsigned int streamIndex = 0;
		unsigned int bufferOffset = 0;
		// The "stride" defines how large a single vertex is in the stream of data
		unsigned int bufferStride; // hack hack hack
		if ( m_parentEntity && m_parentEntity->m_material->rm_Reference.m_effect->rm_Reference.m_skinHack )
			bufferStride = sizeof( GVertices::s_SkinnedVertexTNTB );
		else
			bufferStride = sizeof(GVertices::s_vertexTNTB );
		result = g_RenderDevices.GetDevice()->SetStreamSource( streamIndex, m_reference->rm_Reference.m_vertexBuffer, bufferOffset, bufferStride );
		assert( SUCCEEDED( result ) );
	}
	{
		// Set indices
		HRESULT result = g_RenderDevices.GetDevice()->SetIndices( m_reference->rm_Reference.m_indexBuffer );
		assert( SUCCEEDED(result) );
	}
	// Render objects from the current streams
	{
		// We are using triangles as the "primitive" type,
		// and we have defined the vertex buffer as a triangle list
		// (meaning that every triangle is defined by three vertices)
		D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
		// It's possible to start rendering primitives in the middle of the stream
		unsigned int indexOfFirstVertexToRender = 0;
		// We are currently only rendering two triangles to make a rectangle
		result = g_RenderDevices.GetDevice()->DrawIndexedPrimitive( primitiveType, indexOfFirstVertexToRender, 0, m_reference->rm_Reference.m_vertexCount, 0, m_reference->rm_Reference.m_triangleCount );
		assert( SUCCEEDED( result ) );
	}
}

void Mesh::Unload( )
{
}

bool Mesh::Load( const char* i_filename )
{
	char filepath[128];
	sprintf( filepath, "data//Meshes//%s", i_filename );
	m_reference = ResourceManager<MeshReference>::Load( i_filename, m_meshLoader, false );

	return true;
}

bool Mesh::StdLoad( const char* i_name, std::vector<GVertices::s_vertexTNTB>* i_vertices, std::vector<UINT16>* i_indices,
								const char* i_materialName )
{
	StdMeshLoader loader;
	loader.m_indices = i_indices;
	loader.m_vertices = i_vertices;
	m_reference = ResourceManager<MeshReference>::Load( i_name, &loader, false );

	return true;
}

