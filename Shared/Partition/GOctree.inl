#include "GOctree.h"
#include "../../Renderer/Source/ResourceManager.h"
#include "../../Renderer/Source/RenderManager.h"

const float GOCTREE_EPSILON = 0.001f;

template<typename T>
GOctree<T>::~GOctree()
{
	assert( m_root );
	delete m_root;
}


template<typename T>
GOctreeNode<T>* GOctree<T>::AddObject( T object )
{
	GOctreeNode<T>* currentNode = m_root;
	float extentLength = m_dimension;
	GVector3 currentPosition( m_center );
	int depth = 0;
	// 64 = 0
	// 16 = 1 = top size / ( 4 ^ depth ) = 16 
	// 4 = 2 = top size / (  4 ^ depth ) = 4
	// 1 = 3 = top size / ( 4 ^ depth ) = 1
	for( int i = 0; i < 8; i++ )
	{
		GVector3 octOrigin = GetPosition( currentPosition, (NodePosition)i, depth ); 
		if( object.AABBContains( octOrigin, m_dimension / (float)(4 << (depth)) ) )
		{

			if( currentNode->m_children[i] == 0 )
			{
				currentNode->m_children[i] = new GOctreeNode<T>();
				currentNode = currentNode; // since we'll be adding below
			}
			else
				currentNode = currentNode->m_children[i];

			if( depth+1 > m_maxDepth )
				break;

			depth++;
			currentPosition = octOrigin;
			i = -1;
		}
	}

	m_itemCount++;

	currentNode->m_datas.push_back( object );

	return currentNode;
}

template<typename T>
inline bool GOctree<T>::Contains( const GVector3& i_octOrigin, float i_extentLength, const GVector3& i_objOrigin,
	float i_halfWidth, float i_halfLength, float i_halfHeight )
{
	// loosey goosey doesn't work well for rays.
	/*
	if( (i_octOrigin._x - i_extentLength <= i_objOrigin._x) && (i_octOrigin._x + i_extentLength >= i_objOrigin._x ) && (i_halfWidth < i_extentLength ))
		if( (i_octOrigin._y - i_extentLength <= i_objOrigin._y) && (i_octOrigin._y + i_extentLength >= i_objOrigin._y ) && (i_halfHeight < i_extentLength ) )
			if( (i_octOrigin._z - i_extentLength <= i_objOrigin._z) && (i_octOrigin._z + i_extentLength >= i_objOrigin._z ) && (i_halfLength < i_extentLength ))
				return true;
	*/
	if( (i_octOrigin._x - i_extentLength <= i_objOrigin._x - i_halfWidth ) && (i_octOrigin._x + i_extentLength >= i_objOrigin._x + i_halfWidth ))
		if( (i_octOrigin._y - i_extentLength <= i_objOrigin._y - i_halfHeight) && (i_octOrigin._y + i_extentLength >= i_objOrigin._y + i_halfHeight ) )
			if( (i_octOrigin._z - i_extentLength <= i_objOrigin._z - i_halfLength) && (i_octOrigin._z + i_extentLength >= i_objOrigin._z + i_halfLength))
				return true;

	return false;
}

template<typename T>
GVector3 GOctree<T>::GetPosition( const GVector3& i_parentPos, NodePosition type, int i_depth ) const
{
	GVector3 o_position;
	float extentLength = m_dimension / (float)(4 << i_depth);
	switch( type )
	{
		case BOTTOM_FRONT_LEFT:
		{
			o_position._x = i_parentPos._x - extentLength;
			o_position._y = i_parentPos._y - extentLength;
			o_position._z = i_parentPos._z - extentLength; // possibly fucked up my zs...
		}
		break;

		case BOTTOM_FRONT_RIGHT:
		{
			o_position._x = i_parentPos._x + extentLength;
			o_position._y = i_parentPos._y - extentLength;
			o_position._z = i_parentPos._z - extentLength;
		}
		break;

		case BOTTOM_BACK_RIGHT:
		{
			o_position._x = i_parentPos._x + extentLength;
			o_position._y = i_parentPos._y - extentLength;
			o_position._z = i_parentPos._z + extentLength;
		}
		break;

		case BOTTOM_BACK_LEFT:
		{
			o_position._x = i_parentPos._x - extentLength;
			o_position._y = i_parentPos._y - extentLength;
			o_position._z = i_parentPos._z + extentLength;
		}
		break;

		case TOP_FRONT_LEFT:
		{
			o_position._x = i_parentPos._x - extentLength;
			o_position._y = i_parentPos._y + extentLength;
			o_position._z = i_parentPos._z - extentLength; // possibly fucked up my zs...
		}
		break;

		case TOP_FRONT_RIGHT:
		{
			o_position._x = i_parentPos._x + extentLength;
			o_position._y = i_parentPos._y + extentLength;
			o_position._z = i_parentPos._z - extentLength;
		}
		break;

		case TOP_BACK_RIGHT:
		{
			o_position._x = i_parentPos._x + extentLength;
			o_position._y = i_parentPos._y + extentLength;
			o_position._z = i_parentPos._z + extentLength;
		}
		break;

		case TOP_BACK_LEFT:
		{
			o_position._x = i_parentPos._x - extentLength;
			o_position._y = i_parentPos._y + extentLength;
			o_position._z = i_parentPos._z + extentLength;
		}
		break;
	}

	return o_position;
}

template<typename T>
bool GOctree<T>::RayIntersectsAABB( GVector3 i_center, float i_extentLength ) const
{
	i_extentLength += GOCTREE_EPSILON;
	GVector3 p1 = r_i_start + ( r_i_direction * r_i_length );
    GVector3 m = (r_i_start + p1) * 0.5f;	// Segment midpoint
    GVector3 d = p1 - m;												// Segment halflength vector
    m = m - i_center;														// Translate box and segment to origin
    
    // Try world coordinate axes as separating axes
    float adx = fabsf(d._x);
    if (fabsf(m._x) > i_extentLength + adx)
		return 0;
    float ady = fabsf(d._y);
    if (fabsf(m._y) > i_extentLength + ady)
		return 0;
    float adz = fabsf(d._z);
    if (fabsf(m._z) > i_extentLength + adz)
		return 0;

    // Add in an epsilon term to counteract arithmetic errors when segment is
    // (near) parallel to a coordinate axis
    adx += GOCTREE_EPSILON;
    ady += GOCTREE_EPSILON;
    adz += GOCTREE_EPSILON;

    // Try cross products of segment direction vector with coordinate axes
    if (fabsf(m._y * d._z - m._z * d._y) > i_extentLength * adz + i_extentLength * ady)
		return 0;
    if (fabsf(m._z * d._x - m._x * d._z) > i_extentLength * adz + i_extentLength * adx)
		return 0;
    if (fabsf(m._x * d._y - m._y * d._x) > i_extentLength * ady + i_extentLength * adx)
		return 0;


#ifdef _DEBUG
	if( m_drawAABBIntersection )
		DrawAABB( i_center, i_extentLength );
#endif

    return 1;
}


template<typename T>
void GOctree<T>::DrawAABB( const GVector3& i_center, float i_extentLength ) const
{
	// just do lines right now...we need a nice cube drawing setup...
	D3DXCOLOR	color( 1.0f, 0.0f, 0.0f, 1.0f );
	// bottom_front
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x - i_extentLength, i_center._y - i_extentLength, i_center._z - i_extentLength ),
		D3DXVECTOR3( i_center._x + i_extentLength, i_center._y - i_extentLength, i_center._z - i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	// bottom_back
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x - i_extentLength, i_center._y - i_extentLength, i_center._z + i_extentLength ),
		D3DXVECTOR3( i_center._x + i_extentLength, i_center._y - i_extentLength, i_center._z + i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	// bottom_left
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x - i_extentLength, i_center._y - i_extentLength, i_center._z - i_extentLength ),
		D3DXVECTOR3( i_center._x - i_extentLength, i_center._y - i_extentLength, i_center._z + i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	// bottom_right
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x + i_extentLength, i_center._y - i_extentLength, i_center._z - i_extentLength ),
		D3DXVECTOR3( i_center._x + i_extentLength, i_center._y - i_extentLength, i_center._z + i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	// top_front
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x - i_extentLength, i_center._y + i_extentLength, i_center._z - i_extentLength ),
		D3DXVECTOR3( i_center._x + i_extentLength, i_center._y + i_extentLength, i_center._z - i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	// top_back
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x - i_extentLength, i_center._y + i_extentLength, i_center._z + i_extentLength ),
		D3DXVECTOR3( i_center._x + i_extentLength, i_center._y + i_extentLength, i_center._z + i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	// top_left
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x - i_extentLength, i_center._y + i_extentLength, i_center._z - i_extentLength ),
		D3DXVECTOR3( i_center._x - i_extentLength, i_center._y + i_extentLength, i_center._z + i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	// top_right
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x + i_extentLength, i_center._y + i_extentLength, i_center._z - i_extentLength ),
		D3DXVECTOR3( i_center._x + i_extentLength, i_center._y + i_extentLength, i_center._z + i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	//front_left
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x - i_extentLength, i_center._y - i_extentLength, i_center._z + i_extentLength ),
		D3DXVECTOR3( i_center._x - i_extentLength, i_center._y + i_extentLength, i_center._z + i_extentLength ), 0.001f, 0.001f, 1.5f, color );
	//front_right
	g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_center._x + i_extentLength, i_center._y - i_extentLength, i_center._z - i_extentLength ),
		D3DXVECTOR3( i_center._x + i_extentLength, i_center._y + i_extentLength, i_center._z - i_extentLength ), 0.001f, 0.001f, 1.5f, color );
}

template<typename T>
T* GOctree<T>::RayIntersectsClosest( const GVector3& i_start, const GVector3& i_direction, float length, float& o_distance, GVector3& o_normal )
{
	// cache off the ray data.
	// kills multithreading, but passing this all recursively is no bueno...
	// consider making it a reference or a pointer down the line.
	r_o_distance = length;
	r_i_start = i_start;
	r_i_length = length;
	r_i_direction = i_direction;
	t_out = 0;

#ifdef	_DEBUG
		m_rayTriangleCount = 0;
		m_rayTriangleDepth = 0;

	if( m_drawAABBIntersection )
	{
		D3DXCOLOR	color( 1.0f, 0.0f, 0.0f, 1.0f );
		GVector3 endG = i_start + (i_direction * length * 100.0f );
		D3DXVECTOR3 end( endG._x, endG._y, endG._z );
		// bottom_front
		g_RenderManager.m_lines.AddLine( D3DXVECTOR3( i_start._x, i_start._y - 10.0f, i_start._z ),
			end, 0.1f, 0.1f, 1.5f, color );
	}
#endif

	RayClosestRecursive( m_root, 0, m_center );


	o_distance = r_o_distance;
	o_normal = r_o_normal;
	return t_out;
}

template<typename T>
bool GOctree<T>::RayClosestRecursive( GOctreeNode<T>* i_node, int i_depth, GVector3 i_position )
{
	for( int i = 0; i < (int)i_node->m_datas.size(); i++ )
	{

#ifdef	_DEBUG
		m_rayTriangleCount++;
		if( i_depth > m_rayTriangleDepth )
			m_rayTriangleDepth = i_depth;
#endif

		if( i_node->m_datas[i].RayIntersects( r_i_start, r_i_direction, r_i_length, r_o_distance, r_o_normal ) )
		{
			t_out = &i_node->m_datas[i];
		}
	}

	float extentLength = m_dimension / (4 << i_depth ); 
	for( int i = 0; i < 8; i++ )
	{
		if( i_node->m_children[i] == 0 )
			continue;

		GVector3 pos = GetPosition( i_position, (NodePosition) i, i_depth );

		if( RayIntersectsAABB( pos, extentLength ) )
			RayClosestRecursive( i_node->m_children[i], i_depth+1, pos );
	}

	return 0;
}

template<typename T>
void GOctree<T>::SerializeNodes( FILE* i_file, GOctreeNode<T>* i_node, int depthId /* =0 */ )
{
	//write out each child.  this is kind of lame for leaf nodes, but oh well..
	fwrite( &depthId, sizeof( int ), 1, i_file ); // 0 for root
	for( int i = 0; i < 8; i++ )
	{
		if( i_node->m_children[i] != 0 )
			SerializeNodes( i_file, i_node->m_children[i], depthId + 1 );
		else
		{
			int invalid = -1;
			fwrite( &invalid, sizeof( int ), 1, i_file );
		}
	}

	// write out the number of datas in this node.
	int size = i_node->m_datas.size();
	fwrite( &size, sizeof( int ), 1, i_file );
	if( size > 0 )
		fwrite( &i_node->m_datas[0], sizeof( T ), size, i_file );

}

template<typename T>
GOctreeNode<T>* GOctree<T>::DeSerializeNodes( FILE* i_file, GOctreeNode<T>* i_node, int depthId /*=0*/ )
{
	//read in depth-first
	int depth;
	fread( &depth, sizeof( int ), 1, i_file );
	if( depth == -1 )
		return 0;

	GOctreeNode<T>* node = new GOctreeNode<T>();
	for( int i = 0; i < 8; i++ )
	{
		node->m_children[i] = DeSerializeNodes( i_file, node, depth );
	}

	// read in the node data..
	int size;
	fread( &size, sizeof( int ), 1, i_file );
	node->m_datas.resize( size );
	if( size > 0 )
		fread( &node->m_datas[0], sizeof( T ), size, i_file );

	m_itemCount += size;

	return node;
}