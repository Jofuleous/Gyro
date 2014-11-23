#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <malloc.h>
#include <assert.h>
#include "../RenderManager.h"
#include "Entity.h"


void Entity::Update( float i_dt )
{
	if( m_rotates )
		m_orientationY += 6.28f * m_rotationSpeed * i_dt;
}

void Entity::Unload( )
{
	m_mesh->Unload( );
}

Entity* Entity::LoadEntity( const char* i_entityName, const char* i_meshName, std::vector<GVertices::s_vertexTNTB>* i_vertices, std::vector<UINT16>* i_indices,
							const char* i_materialName )
{
	Entity* entity = new Entity();
	assert( entity );

	entity->m_mesh = new Mesh();

	if( !entity->m_mesh->StdLoad( i_meshName, i_vertices, i_indices, i_materialName ) )
	{
		delete entity;
		return 0;
	}

	entity->m_entityName = new char[strlen( i_entityName ) + 1];
	strcpy( entity->m_entityName, i_entityName );

	MaterialLoader loader;
	// hack: something cleaner later
	char name[128];
	sprintf( name, "%s.matb", i_materialName );
	const char* bs = name;
	ResourcePtr<MaterialReference> reference = ResourceManager<MaterialReference>::Load( bs, &loader, false );
	entity->m_material =  reference;

	entity->m_cameraBound = false;
	entity->m_rotates = false;
	entity->m_rotationSpeed =0.0f;
	entity->m_scale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	return entity;

}

Entity* Entity::LoadEntity( const char* i_filename )
{

	//OutputDebugString( L"WE'RE HERE WOOHOO\n" );
	//std::cerr << "WE'RE HERE\n";
	Entity* entity = new Entity();
	char filepath[256];
	sprintf( filepath, "data//Entities//%s", i_filename ); // todo: export

	std::ifstream file( filepath );

	if( !file )
	{
		std::cerr << "Could not find file: \"" << filepath << "\"";
		return false;
	}

	std::string ignore;
	std::string data;

	
	file >> data;
	if( data.compare( "NAME" ) == 0 )
	{
		file >> data;
		char* name = (char*) malloc( data.size() + 1);
		strcpy(name, data.c_str() );
		entity->m_entityName = name;

		file >> data;

		if( data.compare( "MESH" ) == 0 )
		{
			file >> data;
			entity->m_mesh = new Mesh();
			if( entity->m_mesh->Load( data.c_str() ) )
			{

			}
			else
				assert( entity->m_mesh );

			file >> data;

			if( data.compare( "MATERIAL" ) == 0 )
			{
				file >> data;
				MaterialLoader loader;
				ResourcePtr<MaterialReference> reference = ResourceManager<MaterialReference>::Load( data.c_str(), &loader, false );
				entity->m_material =  reference;
			}

			entity->m_cameraBound = false;
			entity->m_rotates = false;
			entity->m_rotationSpeed =0.0f;

			file >> data;
			if( data.compare( "CAMERABOUNDPOSITION") == 0 )
			{
				entity->m_cameraBound = true;
				file >> data;
			}

			if( data.compare( "YROTATE" ) == 0 )
			{
				file >> data;
				entity->m_rotates = true;
				entity->m_rotationSpeed = (float)atof( data.c_str() );
			}
		}
	}

	file.close();

	// todo: fix sort keys...wat
	g_RenderManager.GenerateSortKey( entity );

	return entity;
}

void Entity::Draw( )
{
	// todo ??? not sure if this was ever being used.  don't think so...check assignments.
	//m_material->SetMaterialData( this );
	m_mesh->Draw();
}