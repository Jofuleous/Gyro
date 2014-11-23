#include "GPhysicsComponent.h"
#include "Actor/ComponentManager.h"

/*
GPhysicsComponent - Represents a physics-driven actor.  As such, we update the position of the actor based on the physics simulation.
*/

const GHashedString GPhysicsComponent::m_typeName = "Physics";
u32 GPhysicsComponent::m_typeId = Gyro::ComponentManager::GetNextTypeId();

//This will be reading from a config file eventually...for now, just expose a way of assigning a renderer object...
void GPhysicsComponent::Initialize( Gyro::SharedPtr<Gyro::Actor> i_actor, LuaPlus::LuaObject& i_obj )
{
	m_RigidBody = new GRigidBody( i_actor, 0.0f );

	libconfig::Setting& attributes = i_setting["Attributes"];

	if( attributes.exists( "RigidBody" ) )
	{
		attributes[ "RigidBody" ].lookupValue( "Mass", m_RigidBody->m_mass );
		m_RigidBody->m_lastPosition		= i_actor->m_position;

		if( attributes[ "RigidBody"].exists("IgnoresGravity") )
			attributes[ "RigidBody"].lookupValue( "IgnoresGravity", m_RigidBody->m_ignoresGravity );
		else
			m_RigidBody->m_ignoresGravity = false;
	}
	else
		m_RigidBody = NULL;

	/// god fucking damnit, i'm not in the mood for how shittily i designed this.
	libconfig::Setting* colliderData;
	//add the collider data, if any.
	if( attributes.exists( "Collider" ) )
		colliderData = &attributes["Collider"];
	else if( m_RigidBody && attributes["RigidBody"].exists( "Collider" ) )
		colliderData = &attributes["RigidBody"]["Collider"];

	if( colliderData )
	{
		// i think you should always delete from where you create, but in this case, we're going to delete this in 
		// the collision manager upon detecting that there is a dead object.  this needs to be cleaned up, and that will happen once i finish 
		// all the homework..
		m_Collider = new GColliderData();
		m_Collider->m_owner = i_actor;
		GColliderData::CreateFromConfig( *m_Collider, *colliderData );
		m_Collider->m_rigidBody = m_RigidBody;
		g_Collision::Get().AddCollider( m_Collider );
	}
	else
		m_Collider = NULL;

	if( m_RigidBody )
		g_Physics::Get().AddRigidBody( m_RigidBody );
}

Gyro::IActorComponent* GPhysicsComponent::Create()
{
	return new GPhysicsComponent();
}

void GPhysicsComponent::Update( Gyro::SharedPtr<Gyro::Actor> i_actor )
{
}

GPhysicsComponent::~GPhysicsComponent( void )
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

void GPhysicsComponent::EndUpdate( Gyro::SharedPtr<Gyro::Actor> i_actor)
{
	// we may want to change this later.  as it stands, anything that has a rigid body has to do this...
	if( m_RigidBody )
		i_actor->m_position = m_RigidBody->m_lastPosition;
}