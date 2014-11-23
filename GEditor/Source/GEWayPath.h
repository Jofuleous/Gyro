// this is a gigantic hack so that i can graduate...

#ifndef __GEWAYPATH_H_
#define __GEWAYPATH_H_

#include "Navigation/GWayPointManager.h"
#include "Math/GAABB.h"
#include "LuaPlus/LuaObject.h"
#include <vector>

class Entity;
class GEWayPath
{
public:

	GEWayPath()
	{
		m_AABB = GAABB( 10.0f, 10.0f, 10.0f, -10.0f, -10.0f, -10.0f );
		m_selectedNode = MAX_U16;
		m_lastSelectedNode = MAX_U16;
	}
				
	void					Update( );
	void					Render( );
	u16						RaySelect( GVector3& i_origin, GVector3& i_direction );
	void					SelectNode( u16 i_node ) { m_lastSelectedNode = m_selectedNode; m_selectedNode = i_node; };
	void					DeleteNode( u16 i_node );
	void					LinkNodes( u16 i_nodeFirst, u16 i_nodeSecond );
	void					WriteLuaTable( LuaPlus::LuaObject& i_obj );
	void					ReadLuaTable( LuaPlus::LuaObject& i_obj );
	bool					NodeSelected() { return m_selectedNode != MAX_U16 ; }
	void					Unload();

	Entity*					LoadNodeEntity( );

	u16						AddNode( const GVector3& i_position );
	GVector3				GetNodePosition( u16 i_node ) { return m_Graph.m_Nodes[i_node].m_Position; }
	void					SetNodePosition( u16 i_node, const GVector3& i_pos );
	void					RecalculateHeuristic( u16 i_node );


	GWayGraph				m_Graph;
	GAABB					m_AABB;

	u16						m_selectedNode;
	u16						m_lastSelectedNode;
	std::vector<Entity*>	m_nodeEntities;
};

#endif