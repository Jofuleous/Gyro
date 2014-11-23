#include "TriggerComponent.h"
#include "Actor/ComponentManager.h"

u32 TriggerComponent::m_typeId = Gyro::ComponentManager::GetNextTypeId();
const char* const TriggerComponent::m_typeName = "TriggerVolume";

Gyro::IActorComponent* TriggerComponent::Create()
{
	return new TriggerComponent();
}

void TriggerComponent::Initialize( Gyro::SharedPtr<Gyro::Actor> actor, libconfig::Setting& i_setting )
{
	
}

void TriggerComponent::Update( Gyro::SharedPtr<Gyro::Actor> i_actor )
{
	//call the updates of the volumes...
}

void TriggerComponent::EndUpdate( Gyro::SharedPtr<Gyro::Actor> i_actor )
{
	// end frame for each

}

void TriggerComponent::OnCollide( const GContact& i_contact, GColliderData* i_thisObject )
{
	// loop through the triggers of this actor and set their collided date.
	/*for( int i = 0; i < m_volumes.size(); i++ )
	{
	}
	*/
}
