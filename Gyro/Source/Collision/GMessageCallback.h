#ifndef __GMESSAGECALLBACK_H_
#define __GMESSAGECALLBACK_H_

#include "ICollisionCallback.h"
#include "Core/GMessageManager.h"
#include "CollisionManager.h"

// ditch this to just send a message to both objects saying they collided?

class GMessageCallback : public ICollisionCallback 
{
public:

	struct MessageByType
	{
		GHashedString m_message;
		GHashedString m_type;
	};

	std::vector<MessageByType> m_messagesByType;
	std::vector<GHashedString> m_messages;

	virtual ~GMessageCallback( )
	{
		m_messagesByType.clear();
		m_messages.clear();
	}

	static ICollisionCallback* Load( LuaPlus::LuaObject& i_obj )
	{
		// load the data from the setting...
		GMessageCallback* callback = new GMessageCallback();
		assert( callback );

		// this is bad.  we need to make it to where you don't have to collide with a certain object to send a message as well..
		int size = i_obj.GetTableCount();
		for( LuaPlus::LuaTableIterator iter(i_obj); iter; iter.Next() )
		{
			// hashing still faster for compares?
				//LuaPlus::LuaTableIterator wtf2( i_obj );
			MessageByType data;
			data.m_type = iter.GetValue()["CollidedWith"].GetString();
			data.m_message = iter.GetValue()["Message"].GetString();
			callback->m_messagesByType.push_back( data );
			continue;
		//	else
		//		callback->m_messages.push_back( i_obj[i]["Message"].GetString() );
		}

		return callback;

	}

	virtual void OnCollide( const GContact* i_contact, GColliderData* i_us, GColliderData* i_them )
	{
		assert( i_them->m_owner );
		GActor* actorThem = GActor::FromHandle( i_them->m_owner );
		if( !actorThem )
			return;
		
		for( unsigned i = 0; i < m_messagesByType.size(); i++ )
		{
			if( actorThem->IsA( m_messagesByType[i].m_type ) )
				g_MessageManager::Get().ProcessMessage( m_messagesByType[i].m_message, (void*)i_contact ); 
		}

		for( unsigned i = 0; i < m_messages.size(); i++ )
			g_MessageManager::Get().ProcessMessage( m_messages[i], (void*)i_contact ); 
	}

private:
	GMessageCallback() {}
};

#endif