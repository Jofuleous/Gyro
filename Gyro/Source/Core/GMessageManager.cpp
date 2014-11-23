#include "GMessageManager.h"
#include "Utility/Clock.h"

// this should only ever get called by the singleton class...
GMessageManager::GMessageManager() : m_messageMap( 200 ) // default size of 200...
{

}

GMessageManager::~GMessageManager()
{
	//should have already shut down.
	//Shutdown();

}


void GMessageManager::RegisterHandler( GHashedString i_message, GMessageHandler* i_handler )
{
	u32 hash = i_message.Get();

	GHashNode<u32, std::vector<GMessageHandler*>>* node = m_messageMap[ hash ];
	if( node != NULL )
	{
		GMessageHandler* handler = i_handler;
		node->m_data.push_back( handler );
	}
	else
	{
		std::vector<GMessageHandler*> newVector;
		m_messageMap.Set( hash, newVector );
		m_messageMap[hash]->m_data.push_back( i_handler );
	}
}

void GMessageManager::RemoveHandler( GHashedString i_message, GMessageHandler* i_handler )
{
	u32 hash = i_message.Get();
	GHashNode<u32, std::vector<GMessageHandler*>>* node = m_messageMap[ hash ];
	if( node != NULL )
	{
		for(u32 i = 0; i < node->m_data.size(); i++ )
		{
			if( i_handler == node->m_data[i] )
			{
				GMessageHandler* back = node->m_data.back();
				node->m_data[i] = back;
				node->m_data.pop_back();
			}
		}
	}
}

void GMessageManager::ProcessMessage( GHashedString i_message, void* i_data )
{
	// right now, only do immediate...
	GHashNode<u32, std::vector<GMessageHandler*>>* node = m_messageMap[ i_message.Get() ];
	if( node )
	{
		for( unsigned i = 0 ; i < node->m_data.size(); i++ )
		{
			GMessageHandler* handler = node->m_data[i];
			handler->HandleMessage( i_message, i_data );
		}
	}
}

void GMessageManager::ProcessMessage( GHashedString i_message, void* i_data, float i_dispatchInSeconds )
{
	_DelayedMessage message;
	message.m_data = i_data;
	message.m_timeToDispatch = (float)g_Clock::Get().SecondsSinceStart() + i_dispatchInSeconds;
	message.m_message = i_message;

	m_delayedMessages.push_back( message );
}

void GMessageManager::Update( void )
{
	for( unsigned i = 0; i < m_delayedMessages.size(); i++ )
	{
		if( m_delayedMessages[i].m_timeToDispatch <= g_Clock::Get().SecondsSinceStart() )
		{
			ProcessMessage( m_delayedMessages[i].m_message, m_delayedMessages[i].m_data );

			m_delayedMessages[i] = m_delayedMessages.back();
			m_delayedMessages.pop_back();
		}
	}
}

void GMessageManager::Shutdown( void )
{
	m_messageMap.Clear(); // this deletes all heap hash nodes.
	m_delayedMessages.clear();
}
