#include "CollisionManager.h"
#include "CollisionUtil.h"
#include "Math/GAABB.h"
#include "Utility/Clock.h"
#include "GMessageCallback.h"
#include "GTriggerCallback.h"
#include "Actor/ComponentManager.h"
#include "../../../Renderer/Source/RenderManager.h"
#include "Misc/DebugConsole.h"
#include "Profiling/GProfiling.h"

#include "GCollisionMesh.h"

// set the callback creator table size initially to be 10.  i don't see this causing performance problems...
static const u32 CALLBACK_TABLE_SIZE = 10;

// set the size of the callback map to be 20 for now
CollisionManager::CollisionManager() : m_callbackMap( CALLBACK_TABLE_SIZE ), m_needsResolution( false ), m_staticTree( NULL )
{

}

void CollisionManager::Initialize( void )
{
	// register the engine specific callbacks.  this probably isn't a good place for this, but meh...it can change later.
	// another option would be to register and enforce using a templated Registration function, I think...
	RegisterCallback( "Messages", GMessageCallback::Load );
	RegisterCallback( "Trigger", GTriggerCallback::Load );
	RegisterComponent<GTriggerComponent>(); ///falfklajlgjlafladgljasd
}

// don't need, but have to declare.
void ToggleOctreeCallback( const char* i_name, bool* i_value ){}

void CollisionManager::BeginFrame( void )
{
	m_currentSeconds = 0.0f;
}

void CollisionManager::EndFrame( void )
{
	 m_needsResolution = false; 
	 m_recentContact.m_time = 2.0f;
}

void CollisionManager::Cleanse()
{
	std::vector< GColliderData* >::iterator iter = m_colliderDatas.begin();

	while( iter != m_colliderDatas.end() )
	{	
		delete (*iter);
		iter++;
	}

	m_colliderDatas.clear();

	// destroy the octree
	delete m_staticTree;
	m_staticTree = NULL;
}

void CollisionManager::Shutdown()
{
	Cleanse();
	// this is all the game specific shit for now...
	m_callbackMap.Clear(); // this deletes all heap hash nodes.
}

void CollisionManager::RemoveDeadActors()
{
	for( u32 i = 0; i < m_colliderDatas.size();  )
	{
		GActor* actor = GActor::FromHandle( m_colliderDatas[i]->m_owner );
		if( !actor )
		{
			delete m_colliderDatas[i];
			m_colliderDatas[i] = m_colliderDatas.back();
			m_colliderDatas.pop_back();
			continue;
		}
		i++;
	}
}

u32 CollisionManager::GetCollisionMask( const char* i_maskName )
{
	return m_maskBitSet.GetBitMask( i_maskName );
}


// seriously, there needs to be a better way of doing this using a pointer hash map or ids...
void CollisionManager::RemoveCollider( GColliderData* i_collider )
{
	for( u32 i = 0; i < m_colliderDatas.size(); i++ )
	{
		if( i_collider == m_colliderDatas[i] )
		{
			m_colliderDatas[i] = m_colliderDatas.back();
			m_colliderDatas.pop_back();
		}
	}
}

void CollisionManager::FindContacts( float i_frameTime, float i_timeAdvanced )
{

	PROFILE_SCOPE_BEGIN( "Collision/Physics - Find Contacts" )
	m_currentSeconds += i_timeAdvanced;

	for( u32 i = 0 ; i < m_colliderDatas.size(); i++ )
	{

		for( u32 j = i ; j < m_colliderDatas.size(); j++ )
		{
			if( i == j )
				continue;

			bool iCollidesWith = (m_colliderDatas[i]->m_colWithMask & m_colliderDatas[j]->m_colIdMask) > 0; //we've had a false positive here...check later.
			bool jCollidesWith = (m_colliderDatas[j]->m_colWithMask & m_colliderDatas[i]->m_colIdMask) > 0;

			//for now, let's say they both have to be registered to collide with each other to register as a collision

			if( iCollidesWith || jCollidesWith )
			{
				// let's a make pre-emptive contact..
				// not everything needs to be a swept collision, but for now, that's what's happening...
				GContact contact;
				contact.m_time = 2.0f;


				GVector3 iCurrentPosition;
				GVector3 jCurrentPosition;
				GVector3 iCurrentVelocity;
				GVector3 jCurrentVelocity;

				// should never be null unless something really bad happened...
				GActor* actori = GActor::FromHandle( m_colliderDatas[i]->m_owner );
				assert( actori );
				GActor* actorj = GActor::FromHandle( m_colliderDatas[j]->m_owner );
				assert( actorj );
				int hash = actori->m_name.Get();
				int hashj = actorj->m_name.Get();

				if( m_colliderDatas[i]->m_rigidBody != NULL )
				{
					iCurrentPosition = m_colliderDatas[i]->m_rigidBody->m_lastPosition;
					iCurrentVelocity = m_colliderDatas[i]->m_rigidBody->m_velocity;
				}
				else if( m_colliderDatas[j]->m_owner != INVALID_ACTOR_HANDLE )
				{
					iCurrentPosition = actori->m_position;
					iCurrentVelocity = GVector3::Zero;
				}
				else
				{
					jCurrentPosition = GVector3::Zero;
					jCurrentVelocity = GVector3::Zero;
				}

				if( m_colliderDatas[j]->m_rigidBody != NULL )
				{
					jCurrentPosition = m_colliderDatas[j]->m_rigidBody->m_lastPosition;
					jCurrentVelocity = m_colliderDatas[j]->m_rigidBody->m_velocity;
				}
				else if( m_colliderDatas[j]->m_owner != INVALID_ACTOR_HANDLE )
				{
					jCurrentPosition = actorj->m_position;
					jCurrentVelocity = GVector3::Zero;
				}
				else
				{
					jCurrentPosition = GVector3::Zero;
					jCurrentVelocity = GVector3::Zero;
				}

				// we may be able to do something like this later when we don't sweep for resting objects, but do bounds checks...
				//if( iCurrentVelocity.LengthSquared() <= 0.0f && jCurrentVelocity.LengthSquared() <= 0.0f )
				//	continue;


				bool collided = false;
				// find out which collision function we'll want to use.  It may actually be better to use inheritance here.
				// A VFT is probably faster...profile later, I guess.  void pointers are cool, though :(

				GMatrix4 jTransform;
				GMatrix4 iTransform;

				switch( m_colliderDatas[i]->m_type )
				{
				case GColliderData::OBB:
					switch( m_colliderDatas[j]->m_type )
					{
					case GColliderData::OBB:
						iTransform = actori->m_rotation;
						iTransform.PokeTranslation( iCurrentPosition );

						jTransform = actorj->m_rotation;
						jTransform.PokeTranslation( jCurrentPosition );

						collided = GCollision::SATOBBCheck( *((const GAABB*)m_colliderDatas[i]->m_collisionObject), iTransform, iCurrentVelocity,
							*((const GAABB*)m_colliderDatas[j]->m_collisionObject), jTransform, jCurrentVelocity, i_frameTime, contact );
						break;
					case GColliderData::AABB:
						iTransform = actori->m_rotation;
						iTransform.PokeTranslation( iCurrentPosition );
						jTransform.Identify();
						jTransform.PokeTranslation( jCurrentPosition );

						collided = GCollision::SATOBBCheck( *((const GAABB*)m_colliderDatas[i]->m_collisionObject), iTransform, iCurrentVelocity, 
							*((const GAABB*)m_colliderDatas[j]->m_collisionObject), jTransform, jCurrentVelocity, i_frameTime, contact );
						break;
					}
					break;
				case GColliderData::AABB:
					switch( m_colliderDatas[j]->m_type )
					{
					case GColliderData::AABB:
						collided = GCollision::SATAABB( *((const GAABB*)m_colliderDatas[i]->m_collisionObject), iCurrentPosition, iCurrentVelocity, 
							*((const GAABB*)m_colliderDatas[j]->m_collisionObject), jCurrentPosition, jCurrentVelocity, i_frameTime, contact );
						break;
					case GColliderData::OBB:
						jTransform = actorj->m_rotation;
						jTransform.PokeTranslation( jCurrentPosition );
						iTransform.Identify();
						iTransform.PokeTranslation( iCurrentPosition );

						collided = GCollision::SATOBBCheck( *((const GAABB*)m_colliderDatas[i]->m_collisionObject), iTransform, iCurrentVelocity, 
							*((const GAABB*)m_colliderDatas[j]->m_collisionObject), jTransform, jCurrentVelocity, i_frameTime, contact );
						break;
					}
					break;
				}

				if( collided )
				{

					contact.m_timeInFrame = m_currentSeconds + (i_frameTime * contact.m_time);
					contact.m_first = m_colliderDatas[i];
					contact.m_second = m_colliderDatas[j];

					// if this is a trigger, we don't want to do any kind of resolution.  just call the callback and move on.w
					if( contact.m_time < 0.0f || contact.m_first->m_isTrigger || contact.m_second->m_isTrigger )
					{
						// these are already intersecting this frame.  we're not going to resolve with physics, but call their collision callbacks...
						if( contact.m_first->m_callback )
							contact.m_first->m_callback->OnCollide( &contact, contact.m_first, contact.m_second );
						if( contact.m_second->m_callback )
							contact.m_second->m_callback->OnCollide( &contact, contact.m_second, contact.m_first );
						continue;
						
						continue;
					}

					if( (contact.m_time < m_recentContact.m_time) &&
						((contact.m_first->m_rigidBody && contact.m_first->m_rigidBody->m_resolves ) || 
						(contact.m_first->m_rigidBody && contact.m_first->m_rigidBody->m_resolves ) ) )
					{
						m_recentContact = contact;
						m_needsResolution = true;
					}
				}
			}
		}
	}

	PROFILE_SCOPE_END( )
}

void CollisionManager::RegisterCallback( const GHashedString& i_hashedString, ICollisionCallback* (*i_creationFunc) ( LuaPlus::LuaObject& i_obj ) )
{
	m_callbackMap.Set( i_hashedString.Get(), i_creationFunc );
}

CollisionCreatorFunc CollisionManager::GetCallback( const GHashedString& i_hashedString )
{
	GHashNode<u32, CollisionCreatorFunc>* node = m_callbackMap[ i_hashedString.Get() ];
	if( node )
		return node->m_data;

	assert( 0 ); // a collision callback by this name was not registered.

	return NULL;
}

void CollisionManager::HandleCallbacks( void )
{
	if( m_recentContact.m_first->m_callback )
		m_recentContact.m_first->m_callback->OnCollide( &m_recentContact, m_recentContact.m_first, m_recentContact.m_second );
	if( m_recentContact.m_second->m_callback )
		m_recentContact.m_second->m_callback->OnCollide( &m_recentContact, m_recentContact.m_second, m_recentContact.m_first );
}

bool CollisionManager::RayQuery( GRayQuery& i_query ) const
{
	bool hitSomething = false;
	float distanceTo = FLT_MAX;
	i_query.m_hitDistance = i_query.m_distance;
	GVector3 normal;

	GMathUtil::ColTriangle* tri = m_staticTree->RayIntersectsClosest( i_query.m_start, i_query.m_direction, i_query.m_distance,
																		i_query.m_hitDistance, i_query.m_normal );

	if( tri )
		return true;

	// turrible for now...
	/*
	for( int i = 0; i < m_colliderDatas.size(); i++ )
	{
		GColliderData*	collider = m_colliderDatas[i];
		if( collider->m_type == GColliderData::Mesh )
		{
			GCollisionMesh* mesh = (GCollisionMesh*) collider->m_collisionObject ;
			if( mesh )
			{
				for( int p = 0; p < mesh->m_polyCount; p++ )
				{
					// test
#define TEST_VERTS
#ifdef TEST_VERTS
					/*
					GVector3 vertZero = mesh->GetVertex( mesh->GetIndex( p, 0 ));
					GVector3 vertOne = mesh->GetVertex( mesh->GetIndex( p, 1 ));
					GVector3 vertTwo = mesh->GetVertex( mesh->GetIndex( p, 2 ));
					D3DXCOLOR	color( 1.0f, 0.0f, 0.0f, 1.0f );
					RenderManager::m_RenderManager->m_lines.AddLine( D3DXVECTOR3( vertZero._x, vertZero._y, vertZero._z ), D3DXVECTOR3( vertOne._x, vertOne._y, vertOne._z ), 0.001f, 0.001f, 1.5f, color );
					RenderManager::m_RenderManager->m_lines.AddLine( D3DXVECTOR3( vertOne._x, vertOne._y, vertOne._z ), D3DXVECTOR3( vertTwo._x, vertTwo._y, vertTwo._z ), 0.001f, 0.001f, 1.5f, color );
					RenderManager::m_RenderManager->m_lines.AddLine( D3DXVECTOR3( vertTwo._x, vertTwo._y, vertTwo._z ), D3DXVECTOR3( vertZero._x, vertZero._y, vertZero._z ), 0.001f, 0.001f, 1.5f, color );
					*/
/*
#endif TEST_VERTS
					if( GMathUtil::RayIntersectsTriangle( i_query.m_start, i_query.m_direction, i_query.m_hitDistance, mesh->GetVertex( mesh->GetIndex( p, 0 ) ),
						mesh->GetVertex( mesh->GetIndex( p, 1 ) ), mesh->GetVertex( mesh->GetIndex( p, 2 ) ), i_query.m_hitDistance, i_query.m_normal ) )
					{
						hitSomething = true;
					}
				}
			}
		}
	}
	*/



	return false;
}

bool CollisionManager::LoadBinarySceneCollision( const char* i_filename )
{
	FILE* sceneFile;
	sceneFile = fopen( i_filename, "r+b" );
	assert( sceneFile );

	fseek( sceneFile, 4, SEEK_SET );

	u64 collisionStartPos;
	fread( &collisionStartPos, sizeof( u64 ), 1, sceneFile );
	fseek( sceneFile, (long)collisionStartPos, SEEK_SET );

	GVector3	center;
	float			dimension;
	int				maxDepth;
	fread( &center, sizeof( GVector3 ), 1, sceneFile );
	fread( &dimension, sizeof( float ), 1, sceneFile );
	fread( &maxDepth, sizeof( int ), 1, sceneFile );

	m_staticTree = new GOctree<GMathUtil::ColTriangle>( dimension, maxDepth, center );
	// Register a checkbox that allows us to toggle the intersecting AABBs on and off.

#ifdef _DEBUG
	DebugConsole::AddCheckBox( "Collision OctreeIntersections", &m_staticTree->m_drawAABBIntersection, ToggleOctreeCallback );
#endif

	// here we go...
	m_staticTree->m_root = m_staticTree->DeSerializeNodes(  sceneFile, m_staticTree->m_root, 0 );


	/*
	u32 numberOfMeshes;
	fread( &numberOfMeshes, sizeof( u32 ), 1, sceneFile );

	//i should probably be pooling these datas....BLAH
	for( u32 m = 0; m < numberOfMeshes; m++ )
	{
		GCollisionMesh* colMesh = new GCollisionMesh();
		fread( &colMesh->m_vertexCount, sizeof( u32 ), 1, sceneFile );
		fread( &colMesh->m_polyCount, sizeof( u32 ), 1, sceneFile );
		colMesh->m_vertices = new GVector3[ colMesh->m_vertexCount ];
		colMesh->m_indices = new u16[ colMesh->m_polyCount * 3 ];

		fread( colMesh->m_vertices, sizeof( GVector3 ), colMesh->m_vertexCount, sceneFile );
		fread( colMesh->m_indices, sizeof( u16 ), colMesh->m_polyCount * 3, sceneFile );

		GColliderData* data = new GColliderData();
		data->m_callback = NULL;
		data->m_colIdMask = GetCollisionMask( "TERRAIN" );
		data->m_collisionObject = colMesh;
		data->m_colWithMask = GetCollisionMask( "PLAYER" );
		data->m_isTrigger = false;
		data->m_rigidBody = NULL;
		data->m_type = GColliderData::Mesh;

		m_colliderDatas.push_back( data );
	}
	*/

	fclose( sceneFile );

	return true;
}

GColliderData::~GColliderData()
{
	switch( m_type )
	{
	case AABB:
	case OBB:
		GAABB* aabb = (GAABB*) m_collisionObject;
		delete aabb;
		break;
	}

	if( m_callback )
		delete m_callback;

}

void GColliderData::CreateFromConfig( GColliderData& o_data, LuaPlus::LuaObject& i_obj )
{
	LuaPlus::LuaObject& temp = i_obj["CollisionBits"];
	o_data.m_colIdMask = 0;

	for( int i = 1; i <= temp.GetCount(); i++ )
		o_data.m_colIdMask |= g_Collision::Get().GetCollisionMask( temp[i].GetString() );
	
	temp = i_obj[ "CollidesWith" ];
	o_data.m_colWithMask = 0;
	for( int i = 1; i <= temp.GetCount(); i++ )
		o_data.m_colWithMask |= g_Collision::Get().GetCollisionMask( temp[i].GetString() );

	if( !i_obj["IsTrigger"].IsNil() )
		o_data.m_isTrigger = i_obj["IsTrigger"].GetBoolean();
	else
		o_data.m_isTrigger = false;

	if( !i_obj["AABB"].IsNil() || !i_obj["OBB"].IsNil() )
	{
		const char* colStr;
		GColliderType type;
		if( !i_obj["AABB"].IsNil() )
		{
			temp = i_obj["AABB"];
			o_data.m_type = AABB;
		}
		else
		{
			temp = i_obj["OBB"];
			o_data.m_type = OBB;
		}

		if( !i_obj["Callback"].IsNil() )
		{
			for ( LuaPlus::LuaTableIterator it( i_obj["Callback"] ); it; it.Next() )
			{
				const char* key = it.GetKey().GetString();
				CollisionCreatorFunc func = g_Collision::Get().GetCallback( key );
				o_data.m_callback = func( it.GetValue() );
			}
		}

		float x, y, z, halfX, halfY, halfZ;
		x = temp["Center"][1].GetFloat();
		y = temp["Center"][2].GetFloat();
		z = temp["Center"][3].GetFloat();
		halfX = temp["HalfX"].GetFloat();
		halfY = temp["HalfY"].GetFloat();
		halfZ = temp["HalfZ"].GetFloat();

		if( o_data.m_type == OBB )
		{
			//for now, let's say the rotation of the obb drives the rotation of the object.  need a more clever way of syncing later.
			if( !temp["RotationZ"].IsNil() )
			{
				float zRot = temp["RotationZ"].GetFloat();
				GMatrix4 zRotation;
				zRotation.SetRotZ( GMath::Deg2Rad( zRot ) );
				GActor* actor = GActor::FromHandle( o_data.m_owner );
				assert( actor );
				actor->m_rotation = zRotation;
			}
		}

		GVector3 center( x, y, z );
		GAABB* aabb = new GAABB( center, halfX, halfY, halfZ );
		o_data.m_collisionObject = aabb;
	}
}