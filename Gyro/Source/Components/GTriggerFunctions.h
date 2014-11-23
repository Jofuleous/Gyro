/*
	This could probably be used as an engine-level initialization class.  Basically, we need some way of registering functions and components that are
	being read in via config files.
*/

#ifndef __GTRIGGERFUNCTIONS_H_
#define __GTRIGGERFUNCTIONS_H_

#include "Containers/GHashMap.h"
#include "Components/GTriggerComponent.h"
#include "Utility/Singleton.h"

class GTriggerFunctions
{
public:
	friend Singleton<GTriggerFunctions>;


	GHashMap< u32, GTriggerComponent::OnTriggerEvent > m_triggerLoadMap;

	GTriggerComponent::OnTriggerEvent GetTriggerFunction( GHashedString i_funcName )
	{
		GHashNode<u32, GTriggerComponent::OnTriggerEvent>* node = m_triggerLoadMap[ i_funcName.Get() ];
		assert( node ); // the function name was not registered.
		return node->m_data;
	}

	void RegisterTriggerFunction( GHashedString i_funcName, GTriggerComponent::OnTriggerEvent i_triggerEvent )
	{
		m_triggerLoadMap.Set( i_funcName.Get(), i_triggerEvent );
	}

	~GTriggerFunctions()
	{
		m_triggerLoadMap.Clear();
	}

	//worry about shutdown later...

private:
	GTriggerFunctions() : m_triggerLoadMap( 20 ) {}
};

typedef Singleton<GTriggerFunctions> g_TriggerFuncs;

#endif