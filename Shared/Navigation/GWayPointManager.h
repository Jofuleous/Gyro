#ifndef __GWAYPATH_H_
#define __GWAYPATH_H_

/*
	This is a really naive approach to path finding, but it needs to work so I can graduate.
	Plus, I think you should write a system a few times before making a final version of it.
	Plus, this will be swapped to use nav-meshes eventually.
*/

#include <vector>
#include "Memory/GMemoryPool.h"
#include "Math/GVector3.h"
#include "Utility/Singleton.h"
#include "LuaPlus/LuaObject.h"
#include "Memory/GMemoryPool.h"

#define MAX_WAYPOINT_LINKS 4

// 16 bytes.  try to align?
class GWayLink
{
public:
	void	LoadFromLua( LuaPlus::LuaObject& i_obj );
	float	m_NodeLength;	// the linear length from start to end node.
	u16		m_FirstNode;		
	u16		m_SecondNode;		
};


class GWayNode
{
public:
	void		LoadFromLua( LuaPlus::LuaObject& i_obj );
	float		CalculateHeuristic( const GWayNode& i_DestNode );
	GVector3	m_Position;			// 3 * 4 = 12
	u16			m_LinkIndices[4];	// 2 * 4  = 8 the index into the link array. 
	u8			m_LinkCount;	// 1 the number of links in the link array.
};

typedef float (*GHeuristicCallback)( GWayNode* i_current, GWayNode* i_next, void* i_data );

class GWayGraph
{
public:
	void					LoadFromLua( LuaPlus::LuaObject& i_obj );
	u16						FindClosestNode( const GVector3& i_pos );
	std::vector<GWayLink>	m_Links;
	std::vector<GWayNode>	m_Nodes;
};

class GWayPathNode
{
public:
	static GMemoryPool* s_WayPathNodePool;
	inline void*	operator new( size_t size )
	{
		return s_WayPathNodePool->Allocate( sizeof( GWayPathNode ) );
	}
	inline void operator delete( void* addr )
	{
		s_WayPathNodePool->Free( addr );
	}

	float			m_H;
	float			m_G;
	float			m_F; // nope.
	u16				m_PathCount;
	GWayPathNode*	m_Parent;
	u16				m_NodeID;
};

// bleh.  let's get this class over with...
class GWayPath
{
public:
	GWayPath( ) : m_pathFound( false ) {}
	bool				m_pathFound;
	std::vector<u16>	m_PathNodes;
};

// if an object de-allocates, the solver MUST be removed from the manager..
class GWaySolver
{
public:

	u32					m_SolverHandle;
	GWayPath*			m_FinalPath;
	u16					m_StartNode;
	u16					m_EndNode;
	bool				m_Complete;
	GVector3			m_Start;
	GVector3			m_End;
	GHeuristicCallback	m_callback;
	void*				m_callbackData;

	std::vector<GWayPathNode*>	m_openList;	// this should be managed with a binary heap, but it will have to work for now.
	std::vector<GWayPathNode*>	m_closedList;
};

class GWayPointManager
{
public:

	friend class Singleton<GWayPointManager>;

	GWayPointManager( ) : m_HandleCount( 0 )
	{

	}

	bool		Initialize( ) { return true; }
	void		UpdateSolvers( );
	u32			QueuePathFind( GWayPath* finalPath, const GVector3& i_start, const GVector3& i_end, GHeuristicCallback i_callback, void* i_callbackData );
	bool		NodePosition( u16 i_node, GVector3& o_pos );
	void		Solve( GWaySolver& io_solver );

	std::vector<GWaySolver>	m_Solvers;	// this would probably work better with a handle-system for fast external access
										// but it can work for now.
	GWayGraph	m_Graph;	// there can be multiple graphs in a game, but we're only using one for now...
	u32			m_HandleCount;

	static const u16 INVALID_NODE_ID;
	static const u16 INVALID_EDGE_ID;
};


static float GDefaultHeuristic( GWayNode* i_current, GWayNode* i_next, void* i_data )
{
	return (i_current->m_Position - i_next->m_Position).LengthSquared();
}

#define g_WayPointManager Singleton<GWayPointManager>::Get()

#endif