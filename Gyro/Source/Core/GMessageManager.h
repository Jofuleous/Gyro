#ifndef __GMESSAGEMANAGER_H_
#define __GMESSAGEMANAGER_H_

#include "Utility/Singleton.h"
#include "Containers/GHashMap.h"
#include "Utility/GHashedString.h"
#include <vector>

/*
todo:  ideally, we will want the ability for specific actors to register for messages.  this way, not all actors will receive
a message that only applies to one specific actor.  example: when an actor gets a "kill" message, only the actor that is getting killed cares about that.
*/
class GMessageHandler
{
public:
	virtual void	HandleMessage( GHashedString& i_message, void* i_data ) = 0;
	virtual			~GMessageHandler() {}
	// what i like about having an owner is that ANY object can register anonymously for a message and have a reference back to iteself.  the callback knows how to handle that reference.
	// we do this at Avalanche, and i like it a lot more.  the only problem is that there's a pretty large memory overhead.  i don't really care, though,
	// i like the flexibility that this provides.  i'll profile it later.
	void*			m_messageOwner;
};

struct _DelayedMessage
{
	float			m_timeToDispatch;
	GHashedString	m_message;
	void*			m_data;
};



class GMessageManager
{
public:
	friend Singleton<GMessageManager>;

	void		RegisterHandler( GHashedString i_message, GMessageHandler* i_handler );
	void		RemoveHandler( GHashedString i_message, GMessageHandler* i_handler );

	//I had to call it "ProcessMessage" for some really messed up compiler collision error...talk with Joe...
	void		ProcessMessage( GHashedString i_message, void* i_data );
	void		ProcessMessage( GHashedString i_message, void* i_data, float i_dispatchInSeconds );
	void		Update( void );
	void		Shutdown( void );

private:
	GMessageManager();
	~GMessageManager();

	// u32 = the hashed string.  we don't actually need to store the string :)
	GHashMap<u32, std::vector<GMessageHandler*>>	m_messageMap;

	std::vector< _DelayedMessage >					m_delayedMessages;
};


typedef Singleton<GMessageManager> g_MessageManager; 

#endif