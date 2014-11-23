#ifndef __COLLISION_MANAGER_H_
#define __COLLISION_MANAGER_H_

#include <vector>
#include "Utility/GSharedPtr.h"
#include "Actor/GActor.h"
#include "Math/GVector3.h"
#include "Utility/NamedBitSet.h"
#include "Physics/GPhysics.h"
#include "Containers/GHashMap.h"
#include "Utility/GHashedString.h"
#include "LuaPlus/LuaPlus.h"
#include "ICollisionCallback.h"
#include "Partition/GOctree.h"
#include "Math/Utilities.h"

class GColliderData
{
public:

	enum GColliderType
	{
		AABB = 0,
		OBB,
		Sphere,
		Cylinder,
		Mesh,
	};

	GColliderData() : m_rigidBody( NULL ), m_callback( NULL ){}
	~GColliderData();

	GActorHandle	m_owner;
	GRigidBody*						m_rigidBody; // this is null unless the collider has a rigid body.
	u32								m_colIdMask;
	u32								m_colWithMask;
	ICollisionCallback*				m_callback;
	bool							m_isTrigger;

	GColliderType					m_type;
	void*							m_collisionObject; // void pointer to the actual collision data for this collision object.
	static void						CreateFromConfig( GColliderData& o_data, LuaPlus::LuaObject& i_obj );
};

class GContact
{
public:
	float				m_time;  // used by the physics and collision system.
	float				m_timeInFrame; // the absolute time (in seconds) during this frame that the collision occured.
	GVector3		m_firstNormal; // corresponds to the normal that m_first hit.

	GColliderData*		m_first;
	GColliderData*		m_second;
};

// Ray query class that will return query results...
class GRayQuery
{
public:
	GRayQuery( const GVector3& i_start, const GVector3& i_direction, u32 i_mask, float i_distance ) :
	m_start( i_start ), m_direction( i_direction ), m_colMask( i_mask ), m_distance( i_distance ) {}

	const GVector3&	m_start;
	const GVector3&	m_direction;
	u32						m_colMask;
	float					m_distance;


	float			m_hitDistance;
	GVector3	m_normal;
	GColliderData*	m_collisionObject;
};

class CollisionManager
{
	friend Singleton<CollisionManager>;

public:

	GContact							m_recentContact;
	bool								m_needsResolution;
	float								m_currentSeconds; // how far (in seconds) the collision system has advanced this frame.
	std::vector<GContact>				m_contactList; // this is used for sending messages and is cleaned up on reset...
	std::vector<GColliderData*>			m_colliderDatas;
	GNamedBitSet<u32>					m_maskBitSet;
	GOctree<GMathUtil::ColTriangle>*	m_staticTree;

	// hashed string u32 to collision callback creator function.
	GHashMap<u32, CollisionCreatorFunc> m_callbackMap;

	void								Initialize( void );
	void								BeginFrame( void );
	void								EndFrame( void );
	void								Shutdown( void );
	bool								LoadBinarySceneCollision( const char* i_filename );
	void								Cleanse();
	void								Clear( ) { m_needsResolution = false;  m_recentContact.m_time = 2.0f; }

	void								RegisterCallback( const GHashedString& i_hashedString, CollisionCreatorFunc i_creationFunc );
	CollisionCreatorFunc				GetCallback( const GHashedString& i_hashedString );
	void								HandleCallbacks( void );

	void								AddCollider( GColliderData* i_collider ) { m_colliderDatas.push_back( i_collider ); } // this should be faster and use a mapping later on.

	// A Ray query that returns the closest object with collision information.  Returns FALSE if it hits nothing.
	bool								RayQuery( GRayQuery& i_query ) const;
	void								RemoveDeadActors( void );
	void								RemoveCollider( GColliderData* i_collider );
	u32									GetCollisionMask( const char* i_maskName );

	
	void							FindContacts( float i_frameTime, float i_timeAdvanced );
	GContact						GetMostRecentContact() { return m_recentContact; }


private:
 	CollisionManager();
	~CollisionManager() {}

};


// this can be used with a ring buffer to store data on a collision...
#ifdef _DEBUG
class TestData
{
public:
	int count;
	float				m_time;
	GVector3		m_firstNormal; // corresponds to the normal of m_first. 
	GVector3		m_secondNormal; // corresponds to the normal of m_second.

	GVector3				rBodyOne_m_velocity;
	GVector3				rBodyOne_m_acceleration;
	GVector3				rBodyOne_m_lastPosition; //we'll probably want to save last velocity as well.

	GVector3				rBodyTwo_m_velocity;
	GVector3				rBodyTwo_m_acceleration;
	GVector3				rBodyTwo_m_lastPosition; //we'll probably want to save last velocity as well.
};
#endif

typedef Singleton< CollisionManager > g_Collision;

#endif


