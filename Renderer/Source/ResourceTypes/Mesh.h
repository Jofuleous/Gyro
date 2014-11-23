#ifndef __NEW_MESH_H_
#define __NEW_MESH_H_

//#include "Texture.h"
#include "../ResourceManager.h"
#include "../ResourceMaster.h"
#include "../ResourceLoader.h"
#include "../ResourcePtr.h"
#include "../VertexStructs.h"
//#include "Material.h"
#include <d3d9.h>
#include <vector>

class Entity;

class MeshData
{
public:

	~MeshData()
	{
		m_vertexBuffer->Release();
		m_indexBuffer->Release();
	}

	unsigned int				m_vertexCount;
	unsigned int				m_indexCount;
	unsigned int				m_triangleCount;

	IDirect3DVertexBuffer9*		m_vertexBuffer;
	IDirect3DIndexBuffer9*		m_indexBuffer;
};

class MeshReference : public ResourceMaster<MeshData>
{
	public:
	~MeshReference()
	{

	}
};

class MeshLoader : public ResourceLoader<MeshReference>
{
public:
	virtual MeshReference* Load( const char* i_filename );
};


class StdMeshLoader : public ResourceLoader<MeshReference>
{
public:
	virtual MeshReference* Load( const char* i_filename );

	std::vector<GVertices::s_vertexTNTB>*	m_vertices;
	std::vector<UINT16>*					m_indices;
};


class Mesh
{

public:
								Mesh();
								~Mesh()
								{

								}

	bool						Load( const char* i_filename );

	bool						StdLoad( const char* i_name,
								std::vector<GVertices::s_vertexTNTB>* i_vertices,
								std::vector<UINT16>* i_indices,
								const char* i_materialName );


	void						Unload( );
	void						SetParent( Entity* i_entity ) { m_parentEntity = i_entity; }

	void							Draw( void );

	ResourcePtr<MeshReference>		m_reference;
	MeshLoader*						m_meshLoader;
	Entity*							m_parentEntity;

	static MeshLoader*				DefaultMeshLoader;
};



#endif