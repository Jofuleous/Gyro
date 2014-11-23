#ifndef __ENTITY_H_
#define __ENTITY_H_

#include "../ResourcePtr.h"
#include <d3dx9math.h> // todo: suddenly needed for D3DXVECTOR3?

#include "Mesh.h"
#include "../VertexStructs.h"
#include "Material.h"

class Entity
{

public:

	// MoVING TO PUBLIC FOR TERRAIN GENERATION HACK.  FIGURE OUT NICER WAY LATER!!!!
	Entity(){}

	static Entity*	LoadEntity( const char* i_filename );
	static Entity*	LoadEntity( const char* i_name, const char* i_meshName,
								std::vector<GVertices::s_vertexTNTB>* i_vertices,
								std::vector<UINT16>* i_indices,
								const char* i_materialName );

	// to initialize to default values.
	void DefaulInit( )
	{
		m_rotationSpeed = 0.0f;
		m_scale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		m_orientationY = 0.0f;
	}

	void			Unload( void );
	void			Update( float i_dt );
	void			Draw( );

	~Entity()
	{
		delete m_mesh;

		if( m_entityName ) // no entity name if coming from FBX...for now.
			free( m_entityName );
	}

	// todo:
	Mesh*								m_mesh;
	ResourcePtr<MaterialReference>		m_material;
	char*								m_entityName;
	bool								m_cameraBound; // hacky, but it will work for now.
	bool								m_rotates; // hacky, but it will work for now.
	float								m_rotationSpeed; // hacky, but it will work for now.



	//these may or may not be used...
	D3DXVECTOR3							m_position;
	float								m_orientationY;
	D3DXVECTOR3							m_scale;

	// todo: make SortKey part of the platform specific
	unsigned __int64					m_sortKey;

private:

};

#endif