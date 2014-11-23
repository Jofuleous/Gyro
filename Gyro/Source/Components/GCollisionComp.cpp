#include "GCollisionComp.h"
#include "Actor/ComponentManager.h"

/*
GCollisionComp
*/

const GHashedString GCollisionComp::m_typeName = "Collision";
u32 GCollisionComp::m_typeId =ComponentManager::GetNextTypeId();

//This will be reading from a config file eventually...for now, just expose a way of assigning a renderer object...
void GCollisionComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	GActor* actor = GActor::FromHandle( i_actor );

	//m_RigidBody = new GRigidBody( actor, 0.0f );
	LuaPlus::LuaObject temp = i_obj;
	if( !i_obj["RigidBody"].IsNil() )
	{
		temp = i_obj["RigidBody"];
		m_RigidBody = new GRigidBody(); // ideally, we'd want to allocate this from a pool...
		m_RigidBody->m_mass = temp["Mass"].GetFloat();
		m_RigidBody->m_lastPosition = actor->m_position;

		if( !temp["Gravity"].IsNil() )
		{
			m_RigidBody->m_gravity._x = temp["Gravity"][1].GetFloat();
			m_RigidBody->m_gravity._y = temp["Gravity"][2].GetFloat();
			m_RigidBody->m_gravity._z = temp["Gravity"][3].GetFloat();
		}

		if( !temp["Resolves"].IsNil() )
			m_RigidBody->m_resolves = temp["Resolves"].GetBoolean();
		else
			m_RigidBody->m_resolves = false;
		// else, the gravity is 0.0f
	}
	else
		m_RigidBody = NULL;

	m_Collider = new GColliderData();
	m_Collider->m_owner = i_actor;
	GColliderData::CreateFromConfig( *m_Collider, i_obj );
	m_Collider->m_rigidBody = m_RigidBody;
	g_Collision::Get().AddCollider( m_Collider );

	if( m_RigidBody )
		g_Physics::Get().AddRigidBody( m_RigidBody );
}

IActorComponent* GCollisionComp::Create()
{
	return new GCollisionComp();
}

void GCollisionComp::Update( GActorHandle i_actor )
{
}

GCollisionComp::~GCollisionComp( void )
{
	/*
	if( m_RigidBody != NULL )
	{
		g_Physics::Get().RemoveRigidBody( m_RigidBody );
		delete m_RigidBody;
	}

	if( m_Collider != NULL )
	{
		g_Collision::Get().RemoveCollider( m_Collider );
		delete m_Collider;
	}
	*/
	
}

void GCollisionComp::EndUpdate( GActorHandle i_actor)
{
	// we may want to change this later.  as it stands, anything that has a rigid body has to do this...
	if( m_RigidBody )
	{
		GActor* actor = GActor::FromHandle( i_actor );
		actor->m_position = m_RigidBody->m_lastPosition;
	}
}