#ifndef __GOCTREE_H_
#define __GOCTREE_H_

/*
=============================
GOctree - Represents a loose octree for the current assignment.  Makes the assumption that items will not be removed from the 
tree for now...but will be queried.
=============================
*/
#include <vector>
#include "Math/GVector3.h"

/*********************************************************************
	TOP_BACK_LEFT      --------------- TOP_BACK_RIGHT
					  /_|___ /______/| 
					 /  | 	/|     / |
	TOP_FRONT_LEFT  --------------/ TOP_FRONT_RIGHT
					|   |	 |	  |  |
	BOTTOM_BACK_LEFT|   |----|----|--| BOTTOM_BACK_RIGHT
					|  /	 |/   |  /
					| /______/____| /
					|/		/	  |/
BOTTOM_FRONT_LEFT	--------------- BOTTOM_FRONT_RIGHT
***********************************************************************/

template<typename T>
class GOctreeNode;

template<typename T>
class GOctree
{
public:

	enum NodePosition
	{
		BOTTOM_FRONT_LEFT = 0,
		BOTTOM_FRONT_RIGHT,
		BOTTOM_BACK_RIGHT,
		BOTTOM_BACK_LEFT,
		TOP_FRONT_LEFT,
		TOP_FRONT_RIGHT,
		TOP_BACK_RIGHT,
		TOP_BACK_LEFT,
	};

	GOctree( float i_dimension, int i_maxDepth, const GVector3 i_center = GVector3( 0,0,0 ), bool i_init = false ) :
	m_dimension( i_dimension ),
	m_root( NULL ),
	m_maxDepth( i_maxDepth ),
	m_center( i_center ),
	m_itemCount( 0 ),
	// have to initialize these to something....
	r_o_distance( 1000000000.0f ),
	r_o_normal( 0.0f, 1.0f, 0.0f),
	r_i_start( 0.0f, 0.0f, 0.0f ),
	t_out( 0 ),
	r_i_length( 100000000.0f ),
	r_i_direction( 0.0f, 1.0f, 0.0f )
	{
		if( i_init )
		{
			m_root = new GOctreeNode<T>();

			for( int i = 0; i < 8; i++ )
				m_root->m_children[i] = 0;
		}

#ifdef _DEBUG
		m_drawAABBIntersection = false;
#endif
	}

	~GOctree();

	GVector3	GetPosition( const GVector3& i_parentPos, NodePosition type, int i_depth ) const;

	// Is the object doubly contained within the bounds (since this is a LOOSE octree)
	bool			Contains( const GVector3& i_octOrigin, float i_extentLength, const GVector3& i_objOrigin,
						float i_halfWidth, float i_halfLength, float i_halfHeight );

	GOctreeNode<T>*	AddObject( T object );

	// Checks for intersection with a cached ray.
	bool			RayIntersectsAABB( GVector3 i_center, float i_extentLength ) const;
	// Finds an intersection with the closest object.
	T*				RayIntersectsClosest( const GVector3& i_start, const GVector3& i_direction, float length, float& o_distance, GVector3& o_normal );
	bool			RayClosestRecursive( GOctreeNode<T>* i_node, int i_depth, GVector3 i_position );
	
	//Debugging
	void			DrawAABB( const GVector3& i_center, float i_extentLength ) const;

	// Serialization
	void			SerializeNodes( FILE* i_file, GOctreeNode<T>* i_node, int depthId = 0 );
	GOctreeNode<T>*	DeSerializeNodes( FILE* i_file, GOctreeNode<T>* i_node, int depthId  = 0);


	// How large ( m_dimension X m_dimension X m_dimension )
	float						m_dimension;

	GOctreeNode<T>*				m_root;
	int							m_maxDepth;
	int							m_itemCount;
	GVector3				m_center;

	// RAY QUERY CACHE.  Turn into datastructure and pass by reference later.
	float						r_o_distance;
	GVector3				r_o_normal;
	GVector3				r_i_start;
	T*							t_out;
	float						r_i_length;
	GVector3				r_i_direction;

	// Debugging vars
#ifdef _DEBUG
	bool						m_drawAABBIntersection;
	int							m_rayTriangleCount;
	int							m_rayTriangleDepth;
#endif

};

// i really don't care how much memory this is taking up right now.  i just want to get the grade and
template<typename T>
class GOctreeNode
{
public:
	GOctreeNode( )
	{
		for( int i = 0; i < 8; i++ )
			m_children[i] = 0;
	}

	~GOctreeNode( )
	{
		for( int i = 0; i < 8; i++ )
		{
			if( m_children[i] != 0 )
				delete m_children[i];
		}

		m_datas.clear();
	}

	GOctreeNode*		m_children[8];
	std::vector<T>		m_datas;
};


#include "GOctree.inl"
#endif