#include "Scene.h"


#include "cTime.h"
#include "Camera.h"
#include "ResourceTypes/Mesh.h"
#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <fstream>
#include <vector>
#include <malloc.h>
#include <cstdint>
#include <math.h>
#include "RenderManager.h"
#include "MainWindow.h"
#include "VertexDeclarations.h"

#include <assert.h>

// this is all over the place...really need to move this to VertexDeclarations.h
struct s_vertexTNTB //vertex texture, normal, tangent, binormal
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
};

Camera* Scene::m_camera = NULL;
bool Scene::m_postProcessing = false;
Entity* Scene::m_postProcessEntity = NULL;
bool Scene::m_shadowMap = true;

Scene::Scene() : m_currentSceneFile( 0 ), m_loadingScene( false )
{
}

Scene::~Scene()
{
	m_entities.clear();
	//m_entities.pop_back();
	//m_entities._Myfirst = 0;
}

Camera* Scene::GetCamera()
{
	if( m_camera )
		return m_camera;

	assert( m_camera );

	return m_camera;
}

bool Scene::LoadBlankScene( )
{
	m_loadingScene = true;

	// Initialize I guess what will have to be common for every scene now?
	m_pointLight = new PointLight();
	m_pointLight->m_color = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_pointLight->m_intensity = 0.5f;
	m_pointLight->m_lightName = "wut";
	m_pointLight->m_position = D3DXVECTOR3( 1000.0f, 1000.0f, 1000.0f );
	m_pointLight->m_radius = 5.0f;
	m_directionalLightDir.x = 0.1f;
	m_directionalLightDir.y = -0.9f;
	m_directionalLightDir.z = 0.0f;
	m_dirLightColor.x = 0.5f;
	m_dirLightColor.y = 0.5f;
	m_dirLightColor.z = 0.5f;
	m_dirLightIntensity = 1.0f;
	D3DXVec3Normalize( &m_directionalLightDir, &m_directionalLightDir);

	m_ambientLight.x = 0.5f;
	m_ambientLight.y = 0.5f;
	m_ambientLight.z = 0.5f;
	m_ambientLight.w = 1;
	m_postProcessing = false;
	m_shadowMap = false; // default false for now.

	m_camera = new Camera();
	D3DXVECTOR3 cameraPos;
	cameraPos.x = 0.0f;
	cameraPos.y = 0.0f;
	cameraPos.z = 0.0f;
	m_camera->m_position = cameraPos;
	m_camera->LookAtPoint( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );

	m_loadingScene = false;

	return true;
}

bool Scene::LoadBinaryScene( const char* i_filepath )
{

	m_loadingScene = true;

	// Initialize I guess what will have to be common for every scene now?
	m_pointLight = new PointLight();
	m_pointLight->m_color = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_pointLight->m_intensity = 0.5f;
	m_pointLight->m_lightName = "wut";
	m_pointLight->m_position = D3DXVECTOR3( 1000.0f, 1000.0f, 1000.0f );
	m_pointLight->m_radius = 5.0f;
	m_directionalLightDir.x = 0.1f;
	m_directionalLightDir.y = -0.9f;
	m_directionalLightDir.z = 0.0f;
	m_dirLightColor.x = 0.5f;
	m_dirLightColor.y = 0.5f;
	m_dirLightColor.z = 0.5f;
	m_dirLightIntensity = 1.0f;
	D3DXVec3Normalize( &m_directionalLightDir, &m_directionalLightDir);

	m_ambientLight.x = 0.5f;
	m_ambientLight.y = 0.5f;
	m_ambientLight.z = 0.5f;
	m_ambientLight.w = 1;
	m_postProcessing = false;
	m_shadowMap = false; // default false for now.

	m_camera = new Camera();
	D3DXVECTOR3 cameraPos;
	cameraPos.x = 0.0f;
	cameraPos.y = 200.0f;
	cameraPos.z = 0.996f;
	m_camera->m_position = cameraPos;
	m_camera->LookAtPoint( D3DXVECTOR3( 0.0f, 100.0f, 0.0f ) );

	m_currentSceneFile = fopen( i_filepath, "r+b" );

	//get the number of meshes.
	assert( m_currentSceneFile);
	if( !m_currentSceneFile )
		return false;

	int fileSize = 0;
	fseek( m_currentSceneFile, 0L, SEEK_END );
	fileSize = ftell( m_currentSceneFile );
	fseek( m_currentSceneFile, 0L, SEEK_SET);

	u32 numberOfMeshes = 0;
	//read the number of meshes...
	fread( &numberOfMeshes, sizeof(u32), 1, m_currentSceneFile );

	u64 collisionOffset;
	fread( &collisionOffset, sizeof( u64 ), 1, m_currentSceneFile );

	for( int i = 0; i < (int)numberOfMeshes; i++ )
	{
		//Get the entity and material names.
		u32 entityNameLength, materialNameLength;

		fread( &entityNameLength, sizeof( u32 ), 1, m_currentSceneFile  ) ;
		char* entityName = (char*) malloc( entityNameLength );
		fread( entityName, sizeof(char), entityNameLength, m_currentSceneFile );

		fread( &materialNameLength, sizeof( u32 ), 1, m_currentSceneFile  ) ;
		char* materialName = (char*) malloc( materialNameLength );
		fread( materialName, sizeof(char), materialNameLength, m_currentSceneFile );

		// Create entity with this;
		Entity* entity = new Entity();
		entity->m_orientationY = 0.0f; // no rotation for scene meshes...yet.
		entity->m_cameraBound = false;
		entity->m_position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); // no translation on these yet, either...
		entity->m_rotates = false;
		entity->m_scale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ); // no scale for now...
		entity->m_mesh = new Mesh();
		entity->m_entityName = entityName;

		SceneMeshLoader bad; //= new SceneMeshLoader();
		entity->m_mesh->m_meshLoader = &bad;
		entity->m_mesh->Load( entityName );

		MaterialLoader loader;
		entity->m_material = ResourceManager<MaterialReference>::Load( materialName, &loader, false );
		free( materialName );
		g_RenderManager.GenerateSortKey( entity );
		m_entities.push_back( entity );
	}

	fclose( m_currentSceneFile );


	m_loadingScene = false;

	return true;
}

//Replace this later with scene file name...
bool Scene::Load( const char* i_filepath )
{
	m_pointLight = NULL;
	m_camera = new Camera();
	m_ambientLight.x = 0.5f;
	m_ambientLight.y = 0.5f;
	m_ambientLight.z = 0.5f;
	m_ambientLight.w = 1;
	m_postProcessing = false;
	D3DXVECTOR3 cameraPos;
	m_camera->m_position = cameraPos;

	std::ifstream file( i_filepath );

	if( !file )
	{
		std::cerr << "Could not find file: \"" << i_filepath << "\"";
		return false;
	}

	std::string data;
	std::string ignore;

	//get triangle and vertex count..
	file >> data; //"VertexCount" "#"
	if( data.compare( "NAME" ) == 0 )
	{
		file >> data;
		char* name = (char*) malloc( 64 );
		strcpy( name, data.c_str() );
	}
	else
		assert( 0 ); //no scene name specified...

	file >> data;
	if( data.compare("<ENTITIES>") == 0 )
	{
		file >> data;

		while( data.compare("</ENTITIES>") != 0 )
		{
			Entity*			entity;
			std::string		filename;
			D3DXVECTOR3		position;
			float			orientationY;
			D3DXVECTOR3		scale;

			if(data.compare("ENTITY") == 0)
			{
				file >> filename;
				entity = Entity::LoadEntity( filename.c_str() );
				file >> data;

				if( data.compare("POSITION") == 0 )
				{
					file >> position.x >> position.y >> position.z;
					entity->m_position = position;
					
					file >> data;
					if( data.compare("ORIENTATIONY") == 0 )
					{
						file >> orientationY;

						file >> data;
						if( data.compare( "SCALE" ) == 0 )
						{
							file >> scale.x;
							file >> scale.y;
							file >> scale.z;
						}
						else
							assert( 0 );
					}
					else
						assert( 0 );

				}
				else
					assert( 0 );
			}

			entity->m_orientationY = (orientationY * 3.14f)/ 180.0f;
			entity->m_scale = scale;
			m_entities.push_back( entity );
			file >> data;
		}
		file >> data;
	}
	if( data.compare( "<LIGHTS>" ) == 0 )
	{
		file >> data;

		while( data.compare( "</LIGHTS>" ) != 0 )
		{
			if( data.compare("POINTLIGHT") == 0 )
			{
				m_pointLight = new PointLight();
				file >> data;
				char* name = (char*) malloc( 64 );
				strcpy(name, data.c_str());
				m_pointLight->m_lightName = name;
				
				file >> data;
				if( data.compare( "POSITION" ) == 0 )
				{
					file >> m_pointLight->m_position.x >> m_pointLight->m_position.y >> m_pointLight->m_position.z;

					file >> data;
					if( data.compare( "COLOR" ) == 0 )
					{
						file >> m_pointLight->m_color.x >> m_pointLight->m_color.y >> m_pointLight->m_color.z;
						file >> data;
						if( data.compare( "RADIUS" ) == 0 )
						{
							file >> m_pointLight->m_radius;
						}
						else
							assert( 0 );
						file >> data;
						if( data.compare( "INTENSITY" ) == 0 )
						{
							file >> m_pointLight->m_intensity;
						}
						else
							assert( 0 );

					}
					else
						assert( 0 );
				}
				else
					assert( 0 );
			}
			else if( data.compare( "DIRECTIONAL" ) == 0 )
			{
				D3DXVECTOR3 in;
				file >> m_dirEyePos.x >> m_dirEyePos.y >> m_dirEyePos.z >> m_dirLightColor.x >> m_dirLightColor.y >> m_dirLightColor.z >> m_dirLightIntensity;
				D3DXVECTOR3 origin( 0,0, 0);
				in = origin - m_dirEyePos;
				D3DXVec3Normalize( &m_directionalLightDir, &in );
			}
			else if( data.compare( "AMBIENTLIGHT" ) == 0 )
			{
				file >> m_ambientLight.x >> m_ambientLight.y >> m_ambientLight.z;
				m_ambientLight.w = 1.0f;
			}
			file >> data;
		}

		file >> data;
	}
	if( data.compare( "ENVIRONMENTMAP" ) == 0 )
	{
		file >> data;
		EnviroMapLoader loader;
		m_enviroMap = ResourceManager<EnviroMapReference>::Load( data.c_str(), &loader, false );
		file >> data;
	}
	if( data.compare( "CAMERAPOS" ) == 0 )
	{
		file >> cameraPos.x >> cameraPos.y >> cameraPos.z;
		file >> data;
	}
	if( data.compare( "POSTPROCESS") == 0 )
	{
		m_postProcessing = true;
		file >> data;
		m_postProcessEntity = Entity::LoadEntity( data.c_str() );
		m_postProcessEntity->m_orientationY = 0.0f;
		m_postProcessEntity->m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_postProcessEntity->m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}

	/*
	file >> ignore >> triangleCount; //"TriangleCount" "#"
	file >> ignore; // "Vertices"

	std::vector<s_vertexTexture> vertices( vertexCount );

	for( unsigned i = 0; i < vertexCount; ++i )
	{
		file >> vertices[i].x >> vertices[i].y >> vertices[i].z;

		file >> vertices[i].u; //u

		file >> vertices[i].v; //v

		file >> vertices[i].nx >> vertices[i].ny >> vertices[i].nz;
	}

	file >> ignore; //"Indices"
	
	unsigned indexCount = triangleCount * 3;
	std::vector<unsigned short> indices( indexCount );

	for( unsigned i = 0; i < indexCount; i++ )
	{
		file >> indices[i];
	}
	*/

	file.close();

	//LAZY  setup camera here...
	m_camera = new Camera();
	m_camera->m_position = cameraPos;
	//Mesh* cube = new Mesh();
	//cube->Load( "data\\Meshes\\wreckt.bmod", "data\\Textures\\example1.png" );
	//m_meshList.push_back( cube );
	//m_selectedMesh = cube;

	return true;
}

void Scene::Update( float i_dt )
{
	//m_camera->Update( i_dt );

	for( unsigned i = 0; i < m_entities.size(); i++ )
	{
		m_entities[i]->Update( i_dt );
	}
}

void Scene::GetLightViewMatrix( D3DXMATRIX& o_viewMatrix )
{
	D3DXVECTOR3 lookAtPoint( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH( &o_viewMatrix, &m_dirEyePos, &lookAtPoint, &up );
}

void Scene::GetLightViewToProjection( D3DXMATRIX& o_projection )
{
	D3DXMatrixOrthoLH( &o_projection, (float)g_MainWindow.GetWidth()/10, (float)g_MainWindow.GetHeight()/10, m_camera->m_nearPlane, m_camera->m_farPlane);
}



void Scene::Draw( float i_dt )
{
	for( unsigned i = 0; i < m_entities.size(); i++ )
		g_RenderManager.Submit( m_entities[i] );
}

void Scene::Unload()
{
	/*
	for(unsigned i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->Unload();
		delete m_entities[i];
	}
	*/

	// apparently clear calls the destructor?
	if( m_entities.size() > 0 )
		m_entities.clear();

	if( m_camera )
	{
		delete m_camera;
		m_camera = NULL;
	}
	if( m_pointLight )
	{
		delete m_pointLight;
		m_pointLight = NULL;
	}
}


// for scene mesh loading....
MeshReference* Scene::SceneMeshLoader::Load( const char* i_filename )
{
	MeshReference* reference = new MeshReference();
	Scene* currentScene = g_RenderManager.GetCurrentScene();

	assert( currentScene->m_loadingScene );
	if( !currentScene->m_loadingScene )
		return reference;

	//read vertex count
	fread( &reference->rm_Reference.m_vertexCount, sizeof(unsigned int), 1, currentScene->m_currentSceneFile );
	//read triangle count
	fread( &reference->rm_Reference.m_triangleCount, sizeof( unsigned int ), 1, currentScene->m_currentSceneFile ) ;
	reference->rm_Reference.m_indexCount = reference->rm_Reference.m_triangleCount * 3;

	unsigned int vertexBufferSize = reference->rm_Reference.m_vertexCount * sizeof( s_vertexTNTB );
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
				fclose( currentScene->m_currentSceneFile );
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
		fclose( currentScene->m_currentSceneFile );
		assert( 0 );
		return false;
	}



	// BEGIN VERTEX READING PROCESS
	s_vertexTNTB* vertexData;

	//LOCK VERTEX BUFFER
	{
		unsigned int lockEntireBuffer = 0;
		DWORD useDefaultLockingBehavior = 0;
		HRESULT result = reference->rm_Reference.m_vertexBuffer->Lock( lockEntireBuffer, lockEntireBuffer,
			reinterpret_cast<void**>( &vertexData ), useDefaultLockingBehavior );
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to lock the vertex buffer" );
			fclose( currentScene->m_currentSceneFile );
			assert( 0 );
			return false;
		}
	}

	//FINALLY, FILL VERTEX BUFFER...
	//vertexData = (s_vertexColor*) malloc( vertexCount * sizeof( s_vertexColor ));
	fread( vertexData, sizeof( s_vertexTNTB ), reference->rm_Reference.m_vertexCount, currentScene->m_currentSceneFile );

	//UNLOCK VERTEX BUFFER
	{
		HRESULT result = reference->rm_Reference.m_vertexBuffer->Unlock();
		if ( FAILED( result ) )
		{
			//cs6963::LogMessage( "Failed to unlock the vertex buffer" );
			fclose( currentScene->m_currentSceneFile );
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
		fclose( currentScene->m_currentSceneFile );
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
			fclose( currentScene->m_currentSceneFile );
			assert( 0 );
			return false;
		}
	}

	//FILL INDEX BUFFER
	//indices = (UINT16*) malloc( indexBufferSize );
	fread( indices, sizeof(UINT16), reference->rm_Reference.m_indexCount, currentScene->m_currentSceneFile );

	//UNLOCK INDEX BUFFER
	{
		HRESULT result = reference->rm_Reference.m_indexBuffer->Unlock();
		if ( FAILED( result ) )
		{
			// cs6963::LogMessage( "Failed to unlock the index buffer" );
			fclose( currentScene->m_currentSceneFile );
			assert( 0 );
			return false;
		}
	}

	return reference;
}