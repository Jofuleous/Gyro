#include "GTriggerComponent.h"
#include "Actor/ComponentManager.h"
#include "GTriggerFunctions.h"
#include "Utility/Clock.h"

u32 GTriggerComponent::m_typeId =ComponentManager::GetNextTypeId();
const GHashedString GTriggerComponent::m_typeName = "Trigger";

IActorComponent* GTriggerComponent::Create()
{
	return new GTriggerComponent();
}

void GTriggerComponent::Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj )
{
	//LuaPlus::LuaObject triggers = i_obj[ "Triggers" ];
	if( !i_obj["OnEnter"].IsNil( ) )
	{
		for( int i = 1; i <= i_obj["OnEnter"].GetCount(); i++ )
		{
			const char* name = i_obj["OnEnter"][i].GetString();
			m_onEnters.push_back( g_TriggerFuncs::Get().GetTriggerFunction( name ) );
		}
	}
	/*
	if( !triggers["OnPersist"].IsNil() )
	{
		for( int i = 1; i <= triggers["OnPersist"].GetCount(); i++ )
		{
			const char* name = triggers["OnPersist"][i].GetString();
			m_onPersists.push_back( g_TriggerFuncs::Get().GetTriggerFunction( name ) );
		}
	}
	*/
	if( !i_obj["OnExit"].IsNil() )
	{
		for( int i = 1; i <= i_obj["OnExit"].GetCount(); i++ )
		{
			const char* name = i_obj["OnExit"][i].GetString();
			m_onExits.push_back( g_TriggerFuncs::Get().GetTriggerFunction( name ) );
		}
	}
}

void GTriggerComponent::Update( GActorHandle i_actor )
{

}

void GTriggerComponent::EndUpdate( GActorHandle i_actor )
{
	// check to see if actors entered the trigger this frame.
	// also, clean up the ones who did not and call their on exit...
	for( unsigned i = 0 ; i < m_currentData.size(); i++ )
	{
		if( !m_currentData[i].m_enteredThisFrame )
		{
			for( unsigned j = 0 ; j < m_onExits.size(); j++ )
			{
				m_onExits[j]( i_actor, m_currentData[i].m_actor, GVector3::Zero );
			}
			//DEBUG_PRINT( "Exiting frame %f \n", (float)g_Clock::Get().SecondsSinceStart() );

			//GActorHandle actorIn = m_currentData[i].m_actor;
			m_currentData[i] = m_currentData.back();
			m_currentData.pop_back();
		}
		else
			m_currentData[i].m_enteredThisFrame = false;
	}
}

void GTriggerComponent::OnCollide( GActorHandle i_us, GActorHandle i_them, GVector3 i_collisionNormal )
{
	// loop through the triggers of this actor and set their collided date.
	bool exists = false;
	for( unsigned i = 0; i < m_currentData.size(); i++ )
	{
		if( m_currentData[i].m_actor == i_them )
		{
			m_currentData[i].m_enteredThisFrame = true;
			exists = true;
		}
	}

	if( !exists )
	{
		TriggerData d( i_them, true );

		// call the onenters.
		for( unsigned i = 0 ; i < m_onEnters.size(); i++ )
		{
			m_onEnters[i]( i_us, i_them, GVector3::Zero );
		}

		m_currentData.push_back( d );
	}
}
