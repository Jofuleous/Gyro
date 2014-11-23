#include "GEditorObject.h"
#include "GEditorScene.h"
#include "d3dx9.h"
#include "d3d9.h"
#include "FBXProcessor.h"
#include "GETypeData.h"
#include "ObjectTypes/GETriggerData.h"
#include "ObjectTypes/GECharacterData.h"

#define COMPARE(a,b) (_stricmp(a,b)==0)

GEditorObject* GEditorObject::Create( const char* i_name, LuaPlus::LuaObject i_luaObj )
{
	GEditorObject* editorObj = new GEditorObject();
	assert( editorObj );
	editorObj->SetName( (char*)i_name );
	
	// name
	int len = strlen( i_name );

	// translation
	LuaPlus::LuaObject obj = i_luaObj["Translation"];
	editorObj->m_position = GVector3( obj[1].GetFloat(), obj[2].GetFloat(), obj[3].GetFloat() );

	// rotation
	editorObj->m_yOrientation = i_luaObj["RotationY"].GetFloat();

	// scale
	obj = i_luaObj["Scale"];
	editorObj->m_scale = GVector3( obj[1].GetFloat(), obj[2].GetFloat(), obj[3].GetFloat() );

	obj = i_luaObj["Type"];
	if( COMPARE(obj.GetString(), "static" ) )
	{
		editorObj->m_type = STATIC;
		editorObj->m_useFBXGeometry = true;
	}
	else if( COMPARE(obj.GetString(), "trigger" ) )
	{
		editorObj->m_type = TRIGGER;
		editorObj->m_useFBXGeometry = false;
	}
	else if( COMPARE(obj.GetString(), "character" ) )
	{
		editorObj->m_type = CHARACTER;
		editorObj->m_useFBXGeometry = false;
	}
	const char* fbxFileName = i_luaObj["Fbx"].GetString();
	len = strlen( fbxFileName );
	if( len > 2 )
	{
		char* temp = new char[128];
		strcpy( temp, fbxFileName );
		editorObj->m_fbxFileName = temp;
		editorObj->LoadEntitiesFromFBX( editorObj->m_fbxFileName );
	}

	// think of some way of not caring?
	switch( editorObj->m_type )
	{
		case CHARACTER:
		{
			editorObj->m_typeData = new GECharacterData();
			if( !i_luaObj["Character"].IsNil() )
				editorObj->m_typeData->LoadFromLua( i_luaObj["Character"] );
		}
		break;
		case TRIGGER:
		{
			editorObj->m_typeData = new GETriggerData();
			if( !i_luaObj["Trigger"].IsNil() )
				editorObj->m_typeData->LoadFromLua( i_luaObj["Trigger"] );
		}
		break;
		case STATIC:
		{
			editorObj->m_typeData = NULL;
		}
		break;
	}

	editorObj->m_Active = true;

	return editorObj;
}

GEditorObject* GEditorObject::Create( GObjectType i_type, const GVector3& i_position )
{
	GEditorObject* object = new GEditorObject();
	assert( object );

	object->m_yOrientation = 0.0f;
	object->m_type = i_type;
	object->m_useFBXGeometry = false;
	object->SetFbxName( "C:/Users/Jofu/Desktop/SomeGame/Assets/FBXScenes/10x10_cube.fbx" );
	object->m_typeData = NULL;

	// hack: need to make these non-literal so we can delete others.
	switch( i_type )
	{
		case TRIGGER:
			object->SetName( "Trigger" );
		break;
		case STATIC:
			object->SetName( "Static" );
		break;
		case CHARACTER:
			object->SetName( "Character" );
		break;
	}
	object->SetTypeData( i_type );

	object->m_position = i_position;
	object->m_scale = GVector3( 1.0f, 1.0f, 1.0f );
	object->LoadEntitiesFromFBX( object->m_fbxFileName );

	return object;
}

// hack:
// this is a hack.  ideally, we'd want to just have the entities inherit a parent transform which gets
// concatenated with their local transform.  for now, we're having to set each entity's position individually...
void GEditorObject::OrientEntities()
{
	// hack: 
	for( int i = 0; i < m_entities.size(); i++ )
	{
		m_entities[i]->m_orientationY = m_yOrientation;
		m_position.ToD3DX( &m_entities[i]->m_position );
	}

	// hack: 
	for( int i = 0; i < m_entities.size(); i++ )
		m_scale.ToD3DX( &m_entities[i]->m_scale );
}

GEditorObject::~GEditorObject()
{
	for( int i = 0; i < m_entities.size(); i++ )
		delete m_entities[i];

	m_entities.clear();

	// todo:
	// need to kill the character strings here, but some are string literals, so we can't for now...do that after
	// the assignment is finished.
}

void GEditorObject::SetName( char* i_name )
{
	if( m_name )
		delete[] m_name;

	m_name = new char[strlen( i_name ) + 1];
	strcpy( m_name, i_name );
}

void GEditorObject::SetFbxName( char* i_name )
{
	if( m_fbxFileName )
		delete m_fbxFileName;

	m_fbxFileName = new char[strlen( i_name ) + 1];
	strcpy( m_fbxFileName, i_name );
}

bool GEditorObject::Destroy( GEditorObject* i_object )
{
	delete i_object;
	return true;
}

void GEditorObject::SetTypeData( GObjectType i_type )
{
	// hack: need to make these non-literal so we can delete others.
	if( m_typeData )
		delete m_typeData;

	switch( i_type )
	{
		case TRIGGER:
			m_typeData = new GETriggerData();
		break;
		case STATIC:
			m_typeData = NULL;
		break;
		case CHARACTER:
			m_typeData = new GECharacterData();
		break;
	}

	m_type = i_type;
}

bool GEditorObject::LoadEntitiesFromFBX( const char* i_filename )
{
	g_EditorScene::Get().m_renderLock.Lock();
	// kill old entities.
	for( int i = 0; i < m_entities.size(); i++ )
		delete m_entities[i];

	m_entities.clear();

	if( FBXProcessor::g_FbxSdkManager == NULL )
	{
		FBXProcessor::g_FbxSdkManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create( FBXProcessor::g_FbxSdkManager, IOSROOT ); // IOSRoot? wat
		FBXProcessor::g_FbxSdkManager->SetIOSettings(pIOsettings);
	}

	using namespace std;

	FbxImporter* pImporter = FbxImporter::Create( FBXProcessor::g_FbxSdkManager,"The BroShow Importer");
	FbxScene* fbxScene = FbxScene::Create( FBXProcessor::g_FbxSdkManager,"Scene of a Magnificent Bro");

	bool succeeded = pImporter->Initialize( i_filename, -1, FBXProcessor::g_FbxSdkManager->GetIOSettings() );
	if( !succeeded )
	{
		assert( 0 );
		std::cerr << "Could not initialize file " << i_filename << ".  Possibly could not find the file?";
		return false;
	}

	succeeded = pImporter->Import( fbxScene );
	if( !succeeded )
	{
		assert( 0 );
		std::cerr << "Could not import file " << i_filename << ".  Possibly could not find the file?";
		return false;
	}

	pImporter->Destroy();

	FbxNode* rootNode = fbxScene->GetRootNode();


	AABBData aabb;
	aabb.InitLargest();
	GHashMap<u32, BatchedMesh*> meshes;
	FBXProcessor::ProcessGeometryAndAABB( rootNode, meshes, aabb );

	m_AABB = GAABB( aabb.m_maxX, aabb.m_maxY, aabb.m_maxZ, aabb.m_minX, aabb.m_minY, aabb.m_minZ );

	int numberOfMeshes = 0;
	// this is weird, but since materials can be broken up into multiple meshes to get past
	// the maximum primitive count, we need to go through all the broken up material meshes.
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

				m_entities.push_back( gross );
				numberOfMeshes += 1;
				current = current->m_next;
			}
			node = node->m_next;
		}
	}

	g_EditorScene::Get().m_renderLock.Unlock();


	OrientEntities();

	return true;
}

bool GEditorObject::WriteLuaTable( LuaPlus::LuaObject i_parent )
{
	LuaPlus::LuaObject contents = i_parent.CreateTable( m_name );
	LuaPlus::LuaObject prop = contents.CreateTable( "Translation", 3 );

	// position
	prop.SetNumber( 1, m_position._x );
	prop.SetNumber( 2, m_position._y );
	prop.SetNumber( 3, m_position._z );

	// rotation
	contents.SetNumber( "RotationY", 0.0f );

	// scale
	prop = contents.CreateTable( "Scale", 3 );
	prop.SetNumber( 1, m_scale._x );
	prop.SetNumber( 2, m_scale._y );
	prop.SetNumber( 3, m_scale._z );

	// type
	switch( m_type )
	{
		case GObjectType::STATIC:
			contents.SetString( "Type", "static" );
		break;
		case GObjectType::TRIGGER:
			contents.SetString( "Type", "trigger" );
		break;
		case GObjectType::CHARACTER:
			contents.SetString( "Type", "character" );
		break;
	}

	if( m_typeData )
		m_typeData->WriteToLua( contents, this );

	// fbx
	contents.SetString( "Fbx", m_fbxFileName );

	return true;
}

void GEditorObject::UpdatePosition( const GVector3& i_position )
{
	//hack: we need some way of keeping entities relative to the parent object...possibly.
	//this may come naturally when we introduce the idea of models made up of meshes.
	//for now, we depend on the transforms being from on all meshes and those meshes never
	//move independently of each other.
	m_position = i_position;

	for( int i = 0; i < m_entities.size(); i++ )
		m_position.ToD3DX( &m_entities[i]->m_position );
}

void GEditorObject::UpdateScale( const GVector3& i_scale )
{
	//hack: we need some way of keeping entities relative to the parent object...possibly.
	//this may come naturally when we introduce the idea of models made up of meshes.
	//for now, we depend on the transforms being from on all meshes and those meshes never
	//move independently of each other.
	m_scale = i_scale;

	for( int i = 0; i < m_entities.size(); i++ )
		m_scale.ToD3DX( &m_entities[i]->m_scale );
}