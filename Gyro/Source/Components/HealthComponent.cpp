#include "HealthComponent.h"
#include "Actor/ComponentManager.h"

const GHashedString HealthComponent::m_typeName = "Health";
u32 HealthComponent::m_typeId =ComponentManager::GetNextTypeId();

IActorComponent* HealthComponent::Create()
{
	return new HealthComponent();
}

void HealthComponent::Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj )
{
	m_MaxHealth = i_obj["MaxHealth"].GetFloat();
	m_KillOnHealthZero = i_obj["KillOnZero"].GetBoolean();
	m_CurrentHealth = m_MaxHealth;
}


// this is purely event driven.
void HealthComponent::DoDamage( int i_ammount, GActorHandle i_owner, GActorHandle i_attacker )
{
	m_CurrentHealth -= i_ammount;
	if( m_CurrentHealth < 0 )
		m_CurrentHealth = 0; // don't go below 0 health.

	if( m_KillOnHealthZero && m_CurrentHealth <= 0 )
	{
		GActor* owner = GActor::FromHandle( i_owner );
		owner->Kill();
	}
}