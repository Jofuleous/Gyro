#ifndef __GEDITOR_IO_H_
#define __GEDITOR_IO_H_

#include <fbxsdk.h>
#include <vector>
#include "VertexStructs.h"
#include "Containers/GHashMap.h"
#include "Math/GAABB.h"
#include "Partition/GOctree.h"
#include "Math/Utilities.h"

/*
	Class just used for de-cluttering the editor scene class...
*/

// A batched mesh represents an entity whose meshes with the same materials have been combined.
// This is not ideal for a rigged entity or entity that is large enough to be culled significantly.
class BatchedMesh
{
public:
	BatchedMesh() : m_next( NULL ) { }
	~BatchedMesh()
	{
		if( m_next )
			delete m_next;
	}

	char									m_materialName[64];
	char									m_meshName[64]; // name it material name for now, maybe.
	std::vector<GVertices::s_vertexTNTB>	m_vertices;
	std::vector<UINT16>						m_indices;
	BatchedMesh*							m_next; // When a batch becomes too large for a draw call, "next" is the next mesh with the same material.
};

class SceneExportData
{
public:
	SceneExportData( float i_dimension, int i_depth, GVector3& i_center ) :
	  m_colTree( i_dimension, i_depth, i_center, true )
	{
	}

	GHashMap<u32, BatchedMesh*>			m_batchedMeshes; // a map of material name to corresponding meshes.
	GOctree< GMathUtil::ColTriangle >	m_colTree;
};


struct AABBData
{
	void InitLargest()
	{
		m_minX = FLT_MAX;
		m_maxX = FLT_MIN;
		m_minY = FLT_MAX;
		m_maxY = FLT_MIN;
		m_minZ = FLT_MAX;
		m_maxZ = FLT_MIN;
	}

	float m_minX;
	float m_maxX;
	float m_minY;
	float m_maxY;
	float m_minZ;
	float m_maxZ;
};

class GEditorScene;

namespace FBXProcessor
{
	static FbxManager* g_FbxSdkManager;

	void ProcessCollisionNodes( FbxNode* i_node, GOctree< GMathUtil::ColTriangle >& o_octree );

	bool ProcessGeometry(FbxNode* i_node, GHashMap<u32, BatchedMesh*>& i_meshes );

	bool ProcessGeometryAndAABB( FbxNode* i_node, GHashMap<u32, BatchedMesh*>& i_meshes, AABBData& o_aabb );

	bool ProcessGeometryAndCollision( FbxNode* i_node, GHashMap<u32, BatchedMesh*>& o_meshes, GOctree< GMathUtil::ColTriangle >& o_octree );

	void ProcessCharacterNodes( FbxNode* i_node, GEditorScene* i_mainScene );

	bool WriteSceneData( const char* i_filepath, SceneExportData&  i_data );
}

#endif