#ifndef __SCENEDATA_H_
#define __SCENEDATA_H_

#include <vector>
#include "Containers/GHashMap.h"
#include "Utility/GHashedString.h"
#include "../../Gyro/Source/Collision/GCollisionMesh.h"
#include "Partition/GOctree.h"
#include "Math/Utilities.h"

// for now, our scene consists of this type of vertex :P
struct s_vertexTNTB //vertex texture, normal, tangent, binormal
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
};

class BatchedMesh
{
public:
	BatchedMesh() : m_next( NULL ) { }
	~BatchedMesh()
	{
		if( m_next ) delete m_next;
		delete m_meshName; // since we've created a new ones.
	}

	const char*					m_materialName;
	char*						m_meshName; // name it material name for now, maybe.
	std::vector<s_vertexTNTB>	m_vertices;
	std::vector<UINT16>			m_indices;
	BatchedMesh*				m_next;
};


// very similar to GCollisionMesh, but uses an std::vector for indices.
class ColMesh
{
public:
	GVector3*		m_vertices;
	std::vector< u16 >	m_indices;

	u32					m_vertexCount;
};

class SceneData
{
public:
	SceneData() : m_colTree( 20000.0f, 200, GVector3( 1000.0f, 1000.0f, 1000.0f ), true ) {}
	GHashMap<u32, BatchedMesh*>			m_batchedMeshes;
	std::vector< ColMesh >				m_collisionMeshes;
	GOctree< GMathUtil::ColTriangle >	m_colTree;
};


#endif

