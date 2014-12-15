#include "GEAxisMarker.h"
#include "../../Renderer/Source/RenderManager.h"
#include "../../Renderer/Source/Camera.h"
#include "../../Renderer/Source/ResourceTypes/Entity.h"
#include "FBXProcessor.h"
#include "GEditorScene.h"
#include "GEditorObject.h"
#include "Utility/Clock.h"
#include "Math/GMath.h"
#include "Math/Utilities.h"

static const float AXIS_LENGTH = 150.0f;
static const D3DXCOLOR RED = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 0.5f );
static const D3DXCOLOR GREEN = D3DXCOLOR( 0.0f, 1.0f, 0.0f, 0.5f );
static const D3DXCOLOR BLUE = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 0.5f );

GEAxisMarker::~GEAxisMarker()
{
	g_RenderManager.m_hud.RemoveHUDEntity( m_markerEntity );
	delete m_markerEntity;
}

int GEAxisMarker::Select( GVector3& i_origin, GVector3& i_dir )
{
	// eventually, we should do this in screen space...since that's how our lines shader works.
	GMatrix4 parent;
	parent.SetTranslation( m_position );
	float distToImpact = 10000000000.0f;
	float tempDist;
	int axis = 0;
	if( m_xBox.RayIntersectsDistance( i_origin, i_dir, parent, distToImpact ) )
		axis = 1;
	if( m_yBox.RayIntersectsDistance( i_origin, i_dir, parent, tempDist ) )
	{
		if( tempDist < distToImpact )
		{
			axis = 2;
			distToImpact = tempDist;
		}
	}
	if( m_zBox.RayIntersectsDistance( i_origin, i_dir, parent, tempDist ) )
	{
		if( tempDist < distToImpact )
		{
			axis = 3;
			distToImpact = tempDist;
		}
	}
	m_selectedAxis = axis;
	m_parentStart = parent.GetTranslation();

	switch( m_selectedAxis )
	{
	case 1:
		{
			// we need to convert this over to using the object's orientation...
			//we want the plane normal to be the z-axis, but let's check if it's positive or negative z...
			if( GVector3( 0.0f, 0.0f, 1.0f ).Dot( i_dir ) > 0.0f )
				m_planeNormal = GVector3( 0.0f, 0.0f, -1.0f );
			else
				m_planeNormal = GVector3( 0.0f, 0.0f, 1.0f );
			m_axisVector = GVector3( 1.0f, 0.0f, 0.0f ); // need parent transform matrix...
		}
	break;
	case 2:
		{
		// we need to choose between z and x-axis as the plane normals.  get the one we're aligned with most...
			GVector3 axis;
			float z = i_dir.Dot( GVector3(0.0f, 0.0f, 1.0f ) );
			float x = i_dir.Dot( GVector3(1.0f, 0.0f, 0.0f ) );

			if( z > x )
				m_planeNormal = GVector3( 0.0f, 0.0f, GMath::Sign( z ) );
			else
				m_planeNormal = GVector3( GMath::Sign( x ), 0.0f, 0.0f );

			m_axisVector = GVector3( 0.0f, 1.0f, 0.0f ); // need parent transform matrix...
		}
	break;
	case 3:
		{
			if( GVector3( 1.0f, 0.0f, 0.0f ).Dot( i_dir ) > 0.0f )
				m_planeNormal = GVector3( -1.0f, 0.0f, 0.0f );
			else
				m_planeNormal = GVector3( 1.0f, 0.0f, 0.0f );

			m_axisVector = GVector3( 0.0f, 0.0f, 1.0f ); // need parent transform matrix...
		}
	break;
	}

	float distance = GMathUtil::RayPlaneIntersect( i_dir, i_origin, m_planeNormal, m_parentStart );
	GVector3 pointOnPlane = i_origin + (i_dir * distance);
	
	// find the projection onto the axis
	float proj = (pointOnPlane - m_parentStart).Dot( m_axisVector );
	m_axisStart = m_parentStart + m_axisVector * proj;

	return axis;
}

void GEAxisMarker::HandleMouse( int x, int y, int width, int height )
{
	float floatX = (( 2.0f * (float)x ) / (float)width) - 1.0f;
	float floatY = 1.0f - ( 2.0f * (float)y ) / (float)height;

	GVector3 dir;
	g_RenderManager.GetCurrentScene()->GetCamera()->ScreenToWorld( floatX, floatY, dir._x, dir._y, dir._z );
	dir.Normalize();
	GVector3 pos = GVector3::FromD3DX( &g_RenderManager.GetCurrentScene()->GetCamera()->m_position );


	// find the distance to the plane...
	// http://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
	float distance = GMathUtil::RayPlaneIntersect( dir, pos, m_planeNormal, m_parentStart );
	GVector3 pointOnPlane = pos + (dir * distance);
	
	// find the projection onto the axis
	float proj = (pointOnPlane - m_parentStart).Dot( m_axisVector );
	m_position = m_parentStart + (m_parentStart + (m_axisVector * proj)) - m_axisStart;
}

void GEAxisMarker::UnSelect()
{
	m_selectedAxis = 0;
	m_axisVector = GVector3( 0.0f, 0.0f, 0.0f );
}

void GEAxisMarker::Update( )
{

	// if we have more than one object, calculate an average...
	// for now just hack
	if( (g_EditorScene::Get().m_selectedObject || (g_EditorScene::Get().m_path.m_selectedNode != MAX_U16) ) && m_selectedAxis )
	{
		g_EditorScene::Get().MoveSelectedObjects( m_position );
	}

	// update the actual entity.
	if( m_markerEntity )
		m_position.ToD3DX( &m_markerEntity->m_position );
}

void GEAxisMarker::LoadEntity()
{
	
	// figure this out later...

	if( FBXProcessor::g_FbxSdkManager == NULL )
	{
		FBXProcessor::g_FbxSdkManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create( FBXProcessor::g_FbxSdkManager, IOSROOT ); // IOSRoot? wat
		FBXProcessor::g_FbxSdkManager->SetIOSettings(pIOsettings);
	}

	using namespace std;

	FbxImporter* pImporter = FbxImporter::Create( FBXProcessor::g_FbxSdkManager,"AxisMarker importer.");
	FbxScene* fbxScene = FbxScene::Create( FBXProcessor::g_FbxSdkManager,"AxisMarker scene?");

	bool succeeded = pImporter->Initialize( "../../../Assets/Models/AxisMarker.fbx", -1, FBXProcessor::g_FbxSdkManager->GetIOSettings() );
	if( !succeeded )
	{
		assert( 0 );
		//std::cerr << "Could not initialize file Assets/Models/AxisMarker.fbx.  Possibly could not find the file?";
		return;
	}

	succeeded = pImporter->Import( fbxScene );
	if( !succeeded )
	{
		assert( 0 );
		return;
	}

	pImporter->Destroy();

	FbxNode* rootNode = fbxScene->GetRootNode();

	GHashMap<u32, BatchedMesh*> meshes;

	FBXProcessor::ProcessGeometry( rootNode, meshes );

	BatchedMesh* bMesh;
	// get the first mesh.  that's what we want...
	for( int i = 0; i < meshes.Size(); i++ )
	{
		if( meshes.Index( i ) != 0 )
			bMesh = meshes.Index( i )->m_data;
	}

	assert( bMesh );

	m_markerEntity = Entity::LoadEntity( "GEAxisMarker", "GEAxisMarker", &bMesh->m_vertices, &bMesh->m_indices, bMesh->m_materialName );

	g_RenderManager.m_hud.AddHUDEntity( m_markerEntity );
	m_markerEntity->m_position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}