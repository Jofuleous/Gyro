#ifndef __GCOLLISIONMESH_H_
#define __GCOLLISIONMESH_H_

#include "Math/GVector3.h"

class GCollisionMesh
{
public:

	GVector3*			m_vertices;
	u16*					m_indices;

	u16						m_indexCount;
	u32						m_vertexCount;
	u16						m_polyCount;

	inline GVector3&	GetVertex( u16 i_index ) { return m_vertices[m_indices[ i_index ]] ; }
	inline u16				GetIndex( u16 i_polyId, u16 i_vertexIndex ) { return i_polyId * 3 + i_vertexIndex; }


	// profile whether this is faster later...
	/*
	class Triangle
	{
	public:
		GVector3 m_zero;
		GVector3 m_one;
		GVector3 m_two;
		GVector3 m_normal;
	};
	*/
	//Triangle*	m_triangles;
};

#endif