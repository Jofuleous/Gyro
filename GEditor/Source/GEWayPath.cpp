#include "GEWayPath.h"
#include "Math/GMatrix4.h"
#include "../../Renderer/Source/ResourceTypes/Entity.h"
#include "../../Renderer/Source/RenderManager.h"
#include "../../Renderer/Source/Camera.h"
#include "FBXProcessor.h"
#include "LuaPlus/LuaTableIterator.h"
#include "assert.h"

void GEWayPath::Update( )
{

}


// holy shit this operation is rough in this...wow.  just don't do it.
void GEWayPath::DeleteNode( u16 i_node )
{
	// destroy the links created by this node.
}

u16 GEWayPath::AddNode( const GVector3& i_position )
{
	m_Graph.m_Nodes.push_back( GWayNode() );
	m_Graph.m_Nodes[m_Graph.m_Nodes.size() - 1].m_Position = i_position;
	m_Graph.m_Nodes[m_Graph.m_Nodes.size() - 1].m_LinkCount = 0;

	Entity* e = LoadNodeEntity( );
	i_position.ToD3DX( &e->m_position );

	m_nodeEntities.push_back( e );
	assert( m_nodeEntities.size() == m_Graph.m_Nodes.size() );

	// if node already selected, link this one with it...
	if( m_selectedNode != MAX_U16 )
		LinkNodes( m_Graph.m_Nodes.size() - 1, m_selectedNode );

	m_selectedNode = m_Graph.m_Nodes.size() - 1;

	return m_selectedNode;
}

void GEWayPath::LinkNodes( u16 i_nodeFirst, u16 i_nodeSecond )
{
	// ensure link doesn't already exist or that we have exceded the max number of links...
	GWayNode& node = m_Graph.m_Nodes[i_nodeFirst];
	if( m_Graph.m_Nodes.size() <= i_nodeFirst || m_Graph.m_Nodes.size() <= i_nodeSecond )
		return;

	if( node.m_LinkCount >= 4 || m_Graph.m_Nodes[i_nodeFirst].m_LinkCount >= 4 ) // MAX_WAYPOINT_LINKS.
		return;

	for( int i = 0; i < node.m_LinkCount; i++ )
	{
		if( m_Graph.m_Links[node.m_LinkIndices[i]].m_FirstNode == i_nodeSecond ||
			m_Graph.m_Links[node.m_LinkIndices[i]].m_SecondNode == i_nodeSecond)
		{
			return;
		}
	}

	m_Graph.m_Links.push_back( GWayLink() );
	u16 linkIndex = m_Graph.m_Links.size() - 1;
	GWayLink& link = m_Graph.m_Links[linkIndex];
	link.m_FirstNode = i_nodeFirst;
	link.m_SecondNode = i_nodeSecond;
	link.m_NodeLength = (m_Graph.m_Nodes[link.m_FirstNode].m_Position - m_Graph.m_Nodes[link.m_SecondNode].m_Position).LengthSquared();
	m_Graph.m_Nodes[i_nodeFirst].m_LinkIndices[ m_Graph.m_Nodes[i_nodeFirst].m_LinkCount ] = linkIndex;
	m_Graph.m_Nodes[i_nodeSecond].m_LinkIndices[ m_Graph.m_Nodes[i_nodeSecond].m_LinkCount ] = linkIndex;
	m_Graph.m_Nodes[i_nodeFirst].m_LinkCount++;
	m_Graph.m_Nodes[i_nodeSecond].m_LinkCount++;

}

void GEWayPath::WriteLuaTable( LuaPlus::LuaObject& obj )
{
	LuaPlus::LuaObject& nodes = obj.CreateTable( "Nodes" );
	for( int i = 1; i <= m_Graph.m_Nodes.size(); i++ )
	{
		LuaPlus::LuaObject& node = nodes.CreateTable( i );
		LuaPlus::LuaObject& temp = node.CreateTable( "Position", 3 );
		temp.SetNumber( 1, m_Graph.m_Nodes[i-1].m_Position.x() );
		temp.SetNumber( 2, m_Graph.m_Nodes[i-1].m_Position.y() );
		temp.SetNumber( 3, m_Graph.m_Nodes[i-1].m_Position.z() );

		temp = node.CreateTable( "Links", m_Graph.m_Nodes[i-1].m_LinkCount );
		for( int j = 0; j < m_Graph.m_Nodes[i-1].m_LinkCount; j++ )
			temp.SetInteger( j + 1,  m_Graph.m_Nodes[i-1].m_LinkIndices[j] );
		
	}

	LuaPlus::LuaObject& links = obj.CreateTable( "Links" );
	for( int i = 1; i <= m_Graph.m_Links.size(); i++ )
	{
		LuaPlus::LuaObject& link = links.CreateTable( i ) ;
		link.SetInteger( "First", m_Graph.m_Links[i-1].m_FirstNode );
		link.SetInteger( "Second", m_Graph.m_Links[i-1].m_SecondNode );
		link.SetNumber( "H", m_Graph.m_Links[i-1].m_NodeLength );
	}
}

void GEWayPath::ReadLuaTable( LuaPlus::LuaObject& obj )
{
	if( obj.IsNil() )
		return;

	LuaPlus::LuaObject& nodes = obj["Nodes"];
	if( nodes.IsNil() )
		return;

	for( LuaPlus::LuaTableIterator it( nodes ); it; it++ )
	{
		m_Graph.m_Nodes.push_back( GWayNode() );
		GWayNode& node = m_Graph.m_Nodes[ m_Graph.m_Nodes.size() - 1];
	
		LuaPlus::LuaObject& obj = it.GetValue();
		node.m_Position.x( obj["Position"][1].GetFloat() );
		node.m_Position.y( obj["Position"][2].GetFloat() );
		node.m_Position.z( obj["Position"][3].GetFloat() );

		int q = 0;
		for( LuaPlus::LuaTableIterator it2( obj["Links"] ); it2; it2++ )
		{
			node.m_LinkIndices[q] = it2.GetValue().GetInteger();
			q++;
		}
		node.m_LinkCount = q;

		Entity* entity = LoadNodeEntity();
		node.m_Position.ToD3DX( entity->m_position );
		m_nodeEntities.push_back( entity ); // bad, but works for now...
	}

	LuaPlus::LuaObject& links = obj["Links"];
	for( LuaPlus::LuaTableIterator it( links ); it; it++ )
	{
		m_Graph.m_Links.push_back( GWayLink() );
		GWayLink& link = m_Graph.m_Links[m_Graph.m_Links.size() -1 ];
		link.m_FirstNode = (u16) it.GetValue()["First"].GetInteger();
		link.m_SecondNode = (u16) it.GetValue()["Second"].GetInteger();
		link.m_NodeLength = it.GetValue()["H"].GetFloat();
	}
}


void GEWayPath::SetNodePosition( u16 i_node, const GVector3& i_pos )
{
	m_Graph.m_Nodes[i_node].m_Position = i_pos;
	i_pos.ToD3DX( &m_nodeEntities[i_node]->m_position );
	RecalculateHeuristic( i_node );
}

void GEWayPath::RecalculateHeuristic( u16 i_node )
{
	int linkCount = m_Graph.m_Nodes[i_node].m_LinkCount;

	for( int i = 0; i < linkCount; i++ )
	{
		GWayLink& link = m_Graph.m_Links[ m_Graph.m_Nodes[i_node].m_LinkIndices[i] ];
		link.m_NodeLength = (m_Graph.m_Nodes[link.m_FirstNode].m_Position - m_Graph.m_Nodes[link.m_SecondNode].m_Position).LengthSquared();
	}
}

u16 GEWayPath::RaySelect( GVector3& i_origin, GVector3& i_direction )
{
	float closestDistance = 10000000000000000.0f;
	u16 hitNode = MAX_U16;
	for( int i = 0; i < m_Graph.m_Nodes.size(); i++ )
	{
		// transform the ray into AABB space...we need a transformation matrix...
		GMatrix4 parent;
		parent.Identify();
		parent.PokeTranslation( m_Graph.m_Nodes[i].m_Position );
		if( m_AABB.RayIntersects( i_origin, i_direction, parent, 100000.0f ) )
		{
			float dist = (i_origin - m_Graph.m_Nodes[i].m_Position).LengthSquared();
			if( dist < closestDistance )
			{
				closestDistance = dist;
				hitNode = i;
			}
		}
	}

	return hitNode;
}


void GEWayPath::Render( )
{
	D3DXCOLOR red( 1.0f, 0.0f, 0.0f, 1.0f );
	for( int i = 0; i < m_Graph.m_Links.size(); i++ )
	{
		D3DXVECTOR3 first, second;
		m_Graph.m_Nodes[m_Graph.m_Links[i].m_FirstNode].m_Position.ToD3DX( &first );
		m_Graph.m_Nodes[m_Graph.m_Links[i].m_SecondNode].m_Position.ToD3DX( &second );
		g_RenderManager.m_lines.AddLine( first, second,  0.001f, 0.001f, g_RenderManager.GetCurrentScene()->m_camera->m_aspectRatio, red ); 
	}

	for( int i = 0; i < m_nodeEntities.size(); i++ )
	{
		g_RenderManager.Submit( m_nodeEntities[i] );
	}
}

Entity* GEWayPath::LoadNodeEntity( )
{
	if( FBXProcessor::g_FbxSdkManager == NULL )
	{
		FBXProcessor::g_FbxSdkManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create( FBXProcessor::g_FbxSdkManager, IOSROOT ); // IOSRoot? wat
		FBXProcessor::g_FbxSdkManager->SetIOSettings(pIOsettings);
	}

	using namespace std;

	FbxImporter* pImporter = FbxImporter::Create( FBXProcessor::g_FbxSdkManager,"The BroShow Importer");
	FbxScene* fbxScene = FbxScene::Create( FBXProcessor::g_FbxSdkManager,"Scene of a Magnificent Bro");

	bool succeeded = pImporter->Initialize( "../Assets/FBXScenes/10x10_cube.fbx", -1, FBXProcessor::g_FbxSdkManager->GetIOSettings() );
	if( !succeeded )
	{
		assert( 0 );
		//std::cerr << "Could not initialize file " << i_filename << ".  Possibly could not find the file?";
		return false;
	}

	succeeded = pImporter->Import( fbxScene );
	if( !succeeded )
	{
		assert( 0 );
		//std::cerr << "Could not import file " << i_filename << ".  Possibly could not find the file?";
		return false;
	}

	pImporter->Destroy();

	FbxNode* rootNode = fbxScene->GetRootNode();
	GHashMap<u32, BatchedMesh*> meshes;
	FBXProcessor::ProcessGeometry( rootNode, meshes );

		for( u32 i = 0; i < meshes.Size(); i++ )
	{
		GHashNode< u32, BatchedMesh* >* node = meshes.Index( i );
		if( !node )
			continue;

		while( node )
		{
			BatchedMesh* current = node->m_data;

			while( current != NULL )
			{
				const char* lol = current->m_meshName;
				const char* lol2 = current->m_materialName;
				Entity* gross = Entity::LoadEntity( "Temp", lol, &current->m_vertices, &current->m_indices, lol2 );
				assert( gross );

				return gross;
			}
			node = node->m_next;
		}
	}

	assert( 0 );
}

void GEWayPath::Unload( )
{
	for( int i = 0; i < m_nodeEntities.size(); i++ )
		delete m_nodeEntities[i];

	m_nodeEntities.clear();
	m_Graph.m_Links.clear();
	m_Graph.m_Nodes.clear();
	m_selectedNode = MAX_U16;
}