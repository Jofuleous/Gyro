#include "GEditorFrame.h"
#include "GEditorScene.h"
#include "GEditorObject.h"
#include "RenderManager.h"
#include "Camera.h"
#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaState.h"
#include "LuaPlus/LuaTableIterator.h"
#include "Math/GMatrix4.h"
#include "Utility/Clock.h"

/*
Todo:
1) Add support for reading in character templates for types (like "basic trigger" character) and to also allow to send multiple messages instead of just one.
2) Add support for maintaining character-specific lua tables.
3) Figure out how to compile the lua and slam it into the binary level file...
4) Come up with a solution that just writes plain text to the level file for now...
5) Figure out a good way of displaying character and type lua data in the UI.
6) Get rid of vector<Entity*> for GEditorObjects and come up with a better solution render-side to manage multiple meshes per-object.
*/

static const float CREATE_OBJECT_DISTANCE = 100.0f;

bool GEditorScene::Initialize( GEditorFrame* i_frame )
{

	LoadBlankScene();
	m_axisMarker.LoadEntity();
	m_editorFrame = i_frame;

	return true;
}

bool GEditorScene::LoadWorldFBX( const char* i_filename )
{
	// load the fbx, extract the static render data and send that off to the static scene.
	//
	return false;
}

bool GEditorScene::LoadBlankScene( )
{
	g_RenderManager.GetCurrentScene()->LoadBlankScene();

	return true;
}


bool GEditorScene::LoadLuaScene( const char* i_filepath )
{
	LuaPlus::LuaState* theLuaState = LuaPlus::LuaState::Create( true );
	// save off the scene file path.
	{
		int pathLength = strlen( i_filepath );
		m_filePath = new char[pathLength + 1];
		strcpy( m_filePath, i_filepath );

		// get the name of this file
		const char* i_filename = strrchr( i_filepath, '\\' ) + 1;
		int nameLength = strrchr( i_filepath, '.' ) - i_filename;
		assert( nameLength < 63 );
		m_sceneName = new char[64];
		memcpy( m_sceneName, i_filename, nameLength );
		m_sceneName[nameLength] = '\0'; // null terminator.

		if( theLuaState->DoFile( i_filepath ) )
		{
			if( theLuaState->GetTop() == 1 )
			{
				assert( 0 ); // an error occured.
			}
		}

		LuaPlus::LuaObject& root = theLuaState->GetGlobal( m_sceneName );
		LuaPlus::LuaObject& objects = root["Objects"];
		LuaPlus::LuaTableIterator it( objects );
		for ( ; it; it.Next() )
		{
			const char* key = it.GetKey().GetString();
			GEditorObject* obj = GEditorObject::Create( key, it.GetValue() );
			AddObject( obj );
		}
		if( !root["Navigation"].IsNil() )
		{
			LuaPlus::LuaObject& navigation = root["Navigation"];
			m_path.ReadLuaTable( navigation );
		}
	}

	LuaPlus::LuaState::Destroy( theLuaState );

	return true;

}

bool GEditorScene::QuickExport()
{
	if( m_filePath )
		return ExportMapFiles( m_filePath );

	return false;
}

bool GEditorScene::QuickSave()
{
	if( m_filePath )
		return SaveMapFile( m_filePath );

	return false;
}

// this needs to be worked on.
bool GEditorScene::SaveMapFile( const char* i_filePath )
{
	// get the name of this file
	const char* i_filename = strrchr( i_filePath, '\\' ) + 1;
	int nameLength = strrchr( i_filePath, '.' ) - i_filename;
	assert( nameLength < 63 );
	char nameBuffer[64];
	memcpy( nameBuffer, i_filename, nameLength );
	nameBuffer[nameLength] = '\0'; // null terminator.

	LuaPlus::LuaState* theLuaState = LuaPlus::LuaState::Create( true );
	{
		LuaPlus::LuaObject& root = theLuaState->GetGlobals().CreateTable( nameBuffer );
		LuaPlus::LuaObject& objects = root.CreateTable( "Objects" );
		for( int i = 0; i < m_allObjects.size(); i++ )
			m_allObjects[i]->WriteLuaTable( objects );

		LuaPlus::LuaObject& nav = root.CreateTable( "Navigation" );

		if( !nav.IsNil() )
			m_path.WriteLuaTable( nav );

		// write out the file...
		//FILE* file = fopen( i_filePath, "w+b" );
		if( theLuaState->DumpObject(i_filePath, nameBuffer, root ) )
			return true;
	}
	LuaPlus::LuaState::Destroy( theLuaState );

	assert( 0 );
	return false;
}

// this needs to be worked on.
bool GEditorScene::ExportMapFiles( const char* i_filePath )
{
	LuaPlus::LuaState* theLuaState = LuaPlus::LuaState::Create( true );

	// get the name of this file
	{
	const char* i_filename = strrchr( i_filePath, '\\' ) + 1;
	int nameLength = strrchr( i_filePath, '.' ) - i_filename;
	assert( nameLength < 63 );
	char nameBuffer[64];
	memcpy( nameBuffer, i_filename, nameLength );
	nameBuffer[nameLength] = '\0'; // null terminator.

	LuaPlus::LuaObject& root = theLuaState->GetGlobals().CreateTable( nameBuffer );
	LuaPlus::LuaObject& objects = root.CreateTable( "Objects" );
	for( int i = 0; i < m_allObjects.size(); i++ )
	{
		if( m_allObjects[i]->m_type != GEditorObject::STATIC )
			m_allObjects[i]->WriteLuaTable( objects );
	}

	LuaPlus::LuaObject& nav = root.CreateTable( "Navigation" );
	if( !nav.IsNil() )
		m_path.WriteLuaTable( nav );


	// write out the file...
	//FILE* file = fopen( i_filePath, "w+b" );
	theLuaState->DumpObject(i_filePath, nameBuffer, root );

	char binaryFileName[ 256 ];
	memcpy( binaryFileName, i_filePath, strlen( i_filePath ) + 1 );
	char* extStart = strrchr( binaryFileName, '.' );
	sprintf( extStart, ".scx\0" );
	if( ExportStaticMapData( binaryFileName ) )
		return true;
	}

	LuaPlus::LuaState::Destroy( theLuaState );

	assert( 0 );
	return false;
}

bool GEditorScene::ExportStaticMapData( const char* i_filePath )
{
	SceneExportData data( 20000.0f, 200, GVector3( 1000.0f, 1000.0f, 1000.0f ) );

	for( int i = 0; i < m_allObjects.size(); i++ )
	{
		if( m_allObjects[i]->m_type != GEditorObject::STATIC )
			continue;

		if( FBXProcessor::g_FbxSdkManager == NULL )
		{
			FBXProcessor::g_FbxSdkManager = FbxManager::Create();
			FbxIOSettings* pIOsettings = FbxIOSettings::Create( FBXProcessor::g_FbxSdkManager, IOSROOT ); // IOSRoot? wat
			FBXProcessor::g_FbxSdkManager->SetIOSettings(pIOsettings);
		}

		FbxImporter* pImporter = FbxImporter::Create( FBXProcessor::g_FbxSdkManager,"The BroShow Importer");
		FbxScene* fbxScene = FbxScene::Create( FBXProcessor::g_FbxSdkManager,"Scene of a Magnificent Bro");
		bool succeeded = pImporter->Initialize( m_allObjects[i]->m_fbxFileName, -1, FBXProcessor::g_FbxSdkManager->GetIOSettings() );
		if( !succeeded )
			return false;
		succeeded = pImporter->Import( fbxScene );
		if( !succeeded )
			return false;

		pImporter->Destroy();

		FbxNode* rootNode = fbxScene->GetRootNode();
		
		FBXProcessor::ProcessGeometryAndCollision( rootNode, data.m_batchedMeshes, data.m_colTree );
	}

	WriteStaticMapData( i_filePath, data );

	return true;
}

bool GEditorScene::WriteStaticMapData( const char* i_filename, SceneExportData& i_data )
{
	FILE* targetFile = fopen( i_filename, "w+b" );
	// write out the number of meshes...
	int numberOfMeshes = 0;
	// this is weird, but since materials can be broken up into multiple meshes to get past
	// the maximum primitive count, we need to go through all the broken up material meshes.
	for( u32 i = 0; i < i_data.m_batchedMeshes.Size(); i++ )
	{
		GHashNode< u32, BatchedMesh* >* node = i_data.m_batchedMeshes.Index( i );
		if( !node )
			continue;

		while( node )
		{
			BatchedMesh* current = node->m_data;

			while( current != NULL )
			{
				numberOfMeshes += 1;
				current = current->m_next;
			}
			node = node->m_next;
		}
	}

	fwrite( &numberOfMeshes, sizeof( u32 ), 1, targetFile );
	u64 bleh = 0;
	fwrite( &bleh, sizeof( u64 ), 1, targetFile );	// we're going to come back and overwrite this...


	u64 dataOffsetCheck = 0;

	for( int i = 0; i < (int)i_data.m_batchedMeshes.Size(); i++ )
	{
		GHashNode< u32, BatchedMesh* >* node = i_data.m_batchedMeshes.Index( i );
		if( !node )
			continue;

		while( node )
		{
			BatchedMesh* mesh = node->m_data;
			while( mesh != NULL )
			{
				// add .matb to the end of the material name..
				u32 eNameLength = strlen( mesh->m_meshName )+1;
				u32 mNameLength = strlen( mesh->m_materialName )+ 1 + 5;

				char* materialName =(char*) malloc( mNameLength );
				sprintf( materialName, "%s.matb",  mesh->m_materialName );

				fwrite( &eNameLength, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite( mesh->m_meshName, sizeof( char ), eNameLength, targetFile );
				dataOffsetCheck += eNameLength;

				fwrite( &mNameLength, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite( materialName, sizeof( char ), mNameLength, targetFile );
				dataOffsetCheck += mNameLength;

				// later, if we wind up going by entity, we'll want to write out matrix transformation data...
				u32 polyCount = mesh->m_indices.size() / 3;
				u32 vertexCount = mesh->m_vertices.size();

				//wow
				//http://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array-c
				fwrite( &vertexCount, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite( &polyCount, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite(&mesh->m_vertices[0], sizeof( GVertices::s_vertexTNTB ), mesh->m_vertices.size(), targetFile );
				dataOffsetCheck += mesh->m_vertices.size() * sizeof( GVertices::s_vertexTNTB );

				fwrite(&mesh->m_indices[0], sizeof( UINT16 ), mesh->m_indices.size(), targetFile );
				dataOffsetCheck += mesh->m_indices.size() * sizeof( u16 );

				mesh = mesh->m_next;
			}

			node = node->m_next;
		}
	}

	u32 collisionPos = ftell( targetFile );

	fseek( targetFile, 4, SEEK_SET );
	u64 test = ftell( targetFile );
	fwrite( &collisionPos, sizeof( u64 ), 1, targetFile );
	fseek( targetFile, collisionPos, SEEK_SET );
	test = ftell( targetFile );

	// write out the octree data.
	fwrite( &i_data.m_colTree.m_center, sizeof( GVector3 ), 1, targetFile );
	fwrite( &i_data.m_colTree.m_dimension, sizeof( float ), 1, targetFile );
	fwrite( &i_data.m_colTree.m_maxDepth, sizeof( int ), 1, targetFile );

	i_data.m_colTree.SerializeNodes( targetFile, i_data.m_colTree.m_root );

	fclose( targetFile );

	return true;
}

GEditorObject* GEditorScene::CreateTrigger()
{
	// hack: lots of things need to be done here.
	GVector3 camPos, lookAtPos;
	memcpy( &camPos, &g_RenderManager.GetCurrentScene()->GetCamera()->m_position, 3 * sizeof( float ) );
	memcpy( &lookAtPos, &g_RenderManager.GetCurrentScene()->GetCamera()->m_lookAtPoint, 3 * sizeof( float ) );
	GVector3 dir = (lookAtPos - camPos) * CREATE_OBJECT_DISTANCE;

	GEditorObject* obj = GEditorObject::Create( GEditorObject::TRIGGER, camPos + dir );

	AddObject( obj );

	SelectObject( obj, false );

	return obj;
}

GEditorObject* GEditorScene::CreateStatic()
{
	// hack: lots of things need to be done here.
	GVector3 camPos, lookAtPos;
	memcpy( &camPos, &g_RenderManager.GetCurrentScene()->GetCamera()->m_position, 3 * sizeof( float ) );
	memcpy( &lookAtPos, &g_RenderManager.GetCurrentScene()->GetCamera()->m_lookAtPoint, 3 * sizeof( float ) );
	GVector3 dir = (lookAtPos - camPos) * CREATE_OBJECT_DISTANCE;

	GEditorObject* obj = GEditorObject::Create( GEditorObject::STATIC, camPos + dir );
	AddObject( obj );

	SelectObject( obj, false );

	return obj;
}

u16 GEditorScene::CreateWaypoint()
{
	GVector3 camPos, lookAtPos;
	memcpy( &camPos, &g_RenderManager.GetCurrentScene()->GetCamera()->m_position, 3 * sizeof( float ) );
	memcpy( &lookAtPos, &g_RenderManager.GetCurrentScene()->GetCamera()->m_lookAtPoint, 3 * sizeof( float ) );
	GVector3 dir = (lookAtPos - camPos) * CREATE_OBJECT_DISTANCE;

	u16 theNode = m_path.AddNode( camPos + dir );
	m_editorFrame->DisplayWayNode( theNode );

	return theNode;
}

void GEditorScene::SelectObject( GEditorObject* i_object, bool i_multiple )
{
	if( !i_multiple )
		m_selectedObjects.clear();

	m_selectedObjects.push_back( i_object );

	m_selectedObject = i_object;
	m_axisMarker.m_position = i_object->m_position;
}

void GEditorScene::UnSelectObject( GEditorObject* i_object )
{
	for( int i = 0; i < m_selectedObjects.size(); i++ )
	{
		if( i_object == m_selectedObjects[i] )
		{
			m_selectedObjects[i] = m_selectedObjects.back();
			m_selectedObjects.pop_back();
		}
	}

	if( m_selectedObject == i_object )
		m_selectedObject = NULL;
}

void GEditorScene::UnSelectAll( )
{
	m_selectedObjects.clear();
	m_selectedObject = NULL;
}

bool GEditorScene::UnloadScene( )
{
	UnSelectAll();
	for( int i = 0; i < m_allObjects.size(); i++ )
		delete m_allObjects[i];

	m_allObjects.clear();
	m_path.Unload();

	delete m_sceneName;

	return true;
}

void GEditorScene::Update( )
{
	m_axisMarker.Update();
}

void GEditorScene::MoveSelectedObjects( GVector3& i_position )
{

	// hack for now...until we get a command processor.
	//if( m_selectedObjects.size() > 0 && i_delta.LengthSquared() > 0.0f )

	for( int i =0; i < m_selectedObjects.size(); i++ )
		m_selectedObjects[i]->UpdatePosition( i_position );

	if( m_path.m_selectedNode != MAX_U16 )
	{
		m_path.SetNodePosition( m_path.m_selectedNode, i_position );
		m_editorFrame->DisplayWayNode( m_path.m_selectedNode );
	}

	if( m_selectedObject )
		m_editorFrame->DisplayEditorObject( m_selectedObject );
}

void GEditorScene::Render()
{

	// draw all nav nodes

	m_path.Render( );
	// Draw all editor objects.
	for( int i = 0; i < m_allObjects.size(); i++ )
	{
		for( int j = 0; j < m_allObjects[i]->m_entities.size(); j++ )
		{
			g_RenderManager.Submit( m_allObjects[i]->m_entities[j] );
		}
	}

	// Draw selection ray.
	if( m_drawDebug )
	{
		D3DXCOLOR color( 1.0f, 0.0f, 0.0f, 1.0f );
		g_RenderManager.m_lines.AddLine( m_start, m_end, 0.001f, 0.001f, g_RenderManager.GetCurrentScene()->m_camera->m_aspectRatio, color );
	}
}

void GEditorScene::AttemptRaySelect( GVector3& i_rayDirection, bool i_shiftDown )
{
	if( g_RenderManager.GetCurrentScene() )
	{
		// hack: getting camera position from D3D....not cross platform.
		D3DXVECTOR3 position = g_RenderManager.GetCurrentScene()->GetCamera()->m_position;
		GVector3 origin( position.x, position.y, position.z );
		GVector3 end = origin + (i_rayDirection * 2000.0f);

		// draw lines...
		//D3DXVECTOR3 zero( origin._x, origin._y, origin._z );
		//m_start = zero;
		//D3DXVECTOR3 top( end._x, end._y, end._z );
		//m_end = top;
		m_drawDebug = false;

		u16 navNodeSelected = MAX_U16;
		navNodeSelected = m_path.RaySelect( origin, i_rayDirection );
		if( navNodeSelected != MAX_U16 )
		{
			m_path.SelectNode( navNodeSelected );
			m_editorFrame->DisplayWayNode( navNodeSelected );
			m_selectedObject = NULL;
			m_selectedObjects.clear();
			m_axisMarker.m_position = m_path.GetNodePosition( navNodeSelected );

			// test for intersection with the axis marker.  this should probably be done differently, since it requires for us to 
			// click inside the actual object, which the marker could extend outside of.
			GVector3 v( position.x, position.y, position.z);
			m_axisMarker.Select( v, i_rayDirection );

			return;
		}
		
		float closestDistance = 10000000000000000.0f;
		GEditorObject* currentSelection = NULL;
		for( int i = 0; i < m_allObjects.size(); i++ )
		{
			// transform the ray into AABB space...we need a transformation matrix...
			GMatrix4 parent;
			parent.SetScale( m_allObjects[i]->m_scale );
			parent.PokeTranslation( m_allObjects[i]->m_position );
			if( m_allObjects[i]->m_AABB.RayIntersects( origin, i_rayDirection, parent, 100000.0f ) && !m_allObjects[i]->m_AABB.PointInside( origin) )
			{
				float dist = (origin - m_allObjects[i]->m_position).Length();

				if( dist < closestDistance )
				{
					closestDistance = dist;
					currentSelection = m_allObjects[i];
				}
			}
		}

		if( currentSelection != NULL )
		{
			m_editorFrame->DisplayEditorObject( currentSelection );
			SelectObject( currentSelection, i_shiftDown );

			// test for intersection with the axis marker.  this should probably be done differently, since it requires for us to 
			// click inside the actual object, which the marker could extend outside of.
			GVector3 v( position.x, position.y, position.z);
			m_axisMarker.Select( v, i_rayDirection );
		}
	}

	return;
}

void GEditorScene::EndUpdate( )
{
}

void GEditorScene::AddObject( GEditorObject* i_object )
{
	m_allObjects.push_back( i_object );
}

void GEditorScene::RemoveObject( GEditorObject* i_object )
{
	UnSelectObject( i_object );
	for( int i = 0; i < m_allObjects.size(); i++ )
	{
		if( m_allObjects[i] == i_object )
		{
			m_allObjects[i] = m_allObjects.back();
			m_allObjects.pop_back();
		}
	}

	GEditorObject::Destroy( i_object );
}

GEditorObject* GEditorScene::GetObjectByName( const char* i_objectName ) const
{
	return 0;
}

