#ifndef __GTRIGGERCALLBACK_H_
#define __GTRIGGERCALLBACK_H_

#include "ICollisionCallback.h"
#include "Components/GTriggerComponent.h"

class GTriggerCallback : public ICollisionCallback
{
public:

	virtual ~GTriggerCallback()
	{
	}

	static ICollisionCallback* Load( LuaPlus::LuaObject& i_data )
	{
		return new GTriggerCallback();
	}

	virtual void OnCollide( const GContact* i_contact, GColliderData* i_us, GColliderData* i_them )
	{
		assert( i_us->m_owner );

		GTriggerComponent* trigger = GetComponent<GTriggerComponent>( i_us->m_owner );
		if( trigger )
		{
			if( i_contact->m_first == i_us )
				trigger->OnCollide( i_us->m_owner, i_them->m_owner, i_contact->m_firstNormal );
			else
				trigger->OnCollide( i_us->m_owner, i_them->m_owner, -i_contact->m_firstNormal );
		}
	}
};

#endif