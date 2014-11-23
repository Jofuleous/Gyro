#include "GWayPointManager.h"
#include "Utility/Clock.h"
#include "LuaPlus/LuaTableIterator.h"

const u16 GWayPointManager::INVALID_EDGE_ID = MAX_U16;
const u16 GWayPointManager::INVALID_NODE_ID = MAX_U16;

GMemoryPool* GWayPathNode::s_WayPathNodePool = GMemoryPool::Create( sizeof( GWayPathNode ), /*was 200...*/4000 );


bool GWayPointManager::NodePosition( u16 i_node, GVector3& o_pos  )
{
	o_pos = m_Graph.m_Nodes[i_node].m_Position;
	return true;
}

u16 GWayGraph::FindClosestNode( const GVector3& i_pos )
{
	// lol....
	u16 best = 0;
	float closestDist = FLT_MAX;
	for( int i = 0; i < m_Nodes.size(); i++ )
	{
		float dist = (m_Nodes[i].m_Position - i_pos).LengthSquared();
		if( dist < closestDist )
		{
			best = i;
			closestDist = dist;
		}
	}

	return best;
}

float GWayNode::CalculateHeuristic( const GWayNode& i_destNode )
{
	return (m_Position - i_destNode.m_Position).LengthSquared();
}

// potentially queue up the finding the start and end node too...
u32 GWayPointManager::QueuePathFind( GWayPath* finalPath, const GVector3& i_start, const GVector3& i_end, GHeuristicCallback i_callback, void* i_callbackData )
{
	m_Solvers.push_back( GWaySolver( ) );
	GWaySolver& solver = m_Solvers[m_Solvers.size() - 1];
	solver.m_Complete = false;
	solver.m_Start = i_start;
	solver.m_End = i_end;
	solver.m_FinalPath = finalPath;
	solver.m_SolverHandle = m_HandleCount++;
	solver.m_StartNode = m_Graph.FindClosestNode( i_start );
	solver.m_EndNode = m_Graph.FindClosestNode( i_end );
	solver.m_callback = i_callback;
	solver.m_callbackData = i_callbackData;

	GWayPathNode* pathNode = new GWayPathNode();
	solver.m_openList.push_back( pathNode );
	pathNode->m_H = solver.m_callback( &m_Graph.m_Nodes[solver.m_StartNode] , &m_Graph.m_Nodes[solver.m_EndNode] , solver.m_callbackData );
	pathNode->m_G = 0.0f;
	pathNode->m_F = pathNode->m_H;
	pathNode->m_NodeID = solver.m_StartNode;
	pathNode->m_Parent = NULL;
	pathNode->m_PathCount = 1; // path size of 1 for now :)

	return solver.m_SolverHandle;
}

void GWayPointManager::UpdateSolvers( )
{
	float startTime = g_Clock::Get().ActualMillisecondsSinceStart();

	// we allow this to run for one millisecond.  not sure how performance intensive it is to query...potentially profile...
	// don't worry about this for now, but it's possible for a solver to be solving forever due to lots of solvers being queued.
	for( int i = 0; i < m_Solvers.size() /*&& (startTime - g_Clock::Get().ActualMillisecondsSinceStart() < 1.0f )*/; i++ )
	{
		if( !m_Solvers[i].m_Complete )
		{
			Solve( m_Solvers[i] );
			m_Solvers[i].m_FinalPath->m_pathFound = true;
			m_Solvers[i] = m_Solvers[m_Solvers.size()-1];
			m_Solvers.pop_back();
		}
	}
}

// this is really bad.  since it is so bad, consider doing this over some number of frames as well..
void GWayPointManager::Solve( GWaySolver& io_solver )
{
	std::vector<GWayPathNode*>& openList = io_solver.m_openList;
	std::vector<GWayPathNode*>& closedList = io_solver.m_closedList;
	GWayPathNode* currentNode = openList[0];
	GWayNode* wayNode = &m_Graph.m_Nodes[currentNode->m_NodeID];
	openList.pop_back();

	GWayNode endNode = m_Graph.m_Nodes[io_solver.m_EndNode];

	while( currentNode->m_NodeID != io_solver.m_EndNode )
	{
		u16 linkCount = wayNode->m_LinkCount;
		for( int i = 0; i < linkCount; i++ )
		{
			u16 linkIndex = wayNode->m_LinkIndices[i];
			u16 nextNode;
			if( m_Graph.m_Links[linkIndex].m_FirstNode != currentNode->m_NodeID )
				nextNode = m_Graph.m_Links[linkIndex].m_FirstNode;
			else
				nextNode = m_Graph.m_Links[linkIndex].m_SecondNode;

			bool inAList = false;

			// holy shit...go through the entire free list and see if this is a shorter path.
			for( int j = 0; j < openList.size(); j++ )
			{
				if( openList[j]->m_NodeID == nextNode )
				{
					if( openList[j]->m_G > currentNode->m_G + m_Graph.m_Links[linkIndex].m_NodeLength )
					{
						openList[j]->m_G = currentNode->m_G + m_Graph.m_Links[linkIndex].m_NodeLength;
						openList[j]->m_F = openList[j]->m_G + openList[j]->m_H;
						openList[j]->m_Parent = currentNode;
						openList[j]->m_PathCount = currentNode->m_PathCount + 1;
					}

					inAList = true;
					break;
				}
			}

			// double holy...potentially get rid of?
			if( !inAList )
			{
				for( int j = 0; j < io_solver.m_closedList.size(); j++ )
				{
					if( closedList[j]->m_NodeID == nextNode )
					{
						if( closedList[j]->m_G > currentNode->m_G + m_Graph.m_Links[linkIndex].m_NodeLength )
						{
							closedList[j]->m_G = currentNode->m_G + m_Graph.m_Links[linkIndex].m_NodeLength;
							closedList[j]->m_F = closedList[j]->m_G + closedList[j]->m_H;
							closedList[j]->m_Parent = currentNode;
							closedList[j]->m_PathCount = currentNode->m_PathCount + 1;
							openList.push_back( closedList[j] );
							closedList[j] = closedList[closedList.size() - 1 ];
							closedList.pop_back();
						}

						inAList = true;
						break;
					}
				}
			}

			if( !inAList )
			{
				GWayPathNode* node = new GWayPathNode();
				openList.push_back( node );
				node->m_Parent = currentNode;
				node->m_NodeID = nextNode;
				node->m_G = currentNode->m_G + m_Graph.m_Links[linkIndex].m_NodeLength;
				node->m_H = io_solver.m_callback( &m_Graph.m_Nodes[nextNode] , &endNode , io_solver.m_callbackData );
				//node->m_H = m_Graph.m_Nodes[nextNode].CalculateHeuristic( endNode );
				node->m_F = node->m_G + node->m_H;
				node->m_PathCount = currentNode->m_PathCount + 1;
			}
		}

		// find the node with the lowest ranking F value.
		u16 openListIndex;
		float bestF = FLT_MAX;
		for( int i = 0 ; i < openList.size(); i++ )
		{
			if( openList[i]->m_F < bestF )
			{
				bestF = openList[i]->m_F;
				openListIndex = i;
			}
		}

		closedList.push_back( currentNode );

		// THE NEW NODE
		currentNode = openList[openListIndex];
		wayNode = &m_Graph.m_Nodes[currentNode->m_NodeID];
		// THE NEW NODE

		openList[openListIndex] = openList[openList.size() - 1];
		openList.pop_back();
	}

	int count = currentNode->m_PathCount;
	io_solver.m_FinalPath->m_PathNodes.resize( count );
	count--;

	while( currentNode != NULL )
	{
		io_solver.m_FinalPath->m_PathNodes[count] = currentNode->m_NodeID;
		GVector3 location = m_Graph.m_Nodes[ currentNode->m_NodeID ].m_Position;
		//DEBUG_PRINT( "path: %d Vector x: %f y: %f z: %f", (int)currentNode->m_NodeID, location._x, location._y, location._z );
		currentNode = currentNode->m_Parent;
		count--;
	}

	io_solver.m_Complete = true;
	
	for( int i = 0; i < io_solver.m_closedList.size(); i++ )
		delete io_solver.m_closedList[i];

	for( int i = 0; i < io_solver.m_openList.size(); i++ )
		delete io_solver.m_openList[i];

	io_solver.m_closedList.clear();
	io_solver.m_openList.clear();
}

void GWayNode::LoadFromLua( LuaPlus::LuaObject& i_obj )
{
	m_Position.x( i_obj["Position"][1].GetFloat() );
	m_Position.y( i_obj["Position"][2].GetFloat() );
	m_Position.z( i_obj["Position"][3].GetFloat() );

	int q = 0;
	for( LuaPlus::LuaTableIterator it2( i_obj["Links"] ); it2; it2++ )
	{
		m_LinkIndices[q] = it2.GetValue().GetInteger();
		q++;
	}
	m_LinkCount = q;
}

void GWayGraph::LoadFromLua( LuaPlus::LuaObject& i_obj )
{
	LuaPlus::LuaObject& nodes = i_obj["Nodes"];
	if( nodes.IsNil() )
		return;

	for( LuaPlus::LuaTableIterator it( nodes ); it; it++ )
	{
		m_Nodes.push_back( GWayNode() );
		GWayNode& node = m_Nodes[ m_Nodes.size() - 1 ];
		LuaPlus::LuaObject& obj = it.GetValue();
		node.LoadFromLua( obj );
	}

	LuaPlus::LuaObject& links = i_obj["Links"];
	for( LuaPlus::LuaTableIterator it( links ); it; it++ )
	{
		m_Links.push_back( GWayLink() );
		GWayLink& link = m_Links[m_Links.size() -1 ];
		link.LoadFromLua( it.GetValue() );
	}
}

void GWayLink::LoadFromLua( LuaPlus::LuaObject& i_obj )
{
	m_FirstNode = (u16) i_obj["First"].GetInteger();
	m_SecondNode = (u16) i_obj["Second"].GetInteger();
	m_NodeLength = i_obj["H"].GetFloat();
}