#include "SpawnData.h"
#include "SpawnManager.h"

void SpawnData::Load( LuaPlus::LuaObject& i_luaObject )
{

	m_Time = i_luaObject["Time"].GetNumber();
	m_Frequency = i_luaObject["Frequency"].GetNumber();
	m_Count = i_luaObject["Count"].GetInteger();
	LuaPlus::LuaObject& temp = i_luaObject["Enemies"];
	m_EnemyTypeCount = temp.GetCount();
	for( int i = 0; i < temp.GetCount(); i++ )
	{
		sprintf( &m_EnemyTypes[32 * i], "%s", temp[i+1].GetString() );

		/* moved to the spawn manager.  using "GameProgression.spawn"
		// cache the creation file to avoid load latency later...
		char name[128];
		sprintf( name, "data/Characters/%s.carx", &m_EnemyTypes[32 * i] );
		if( g_SpawnManager::Get().m_luaState->DoFile( name ) )
		{
			if( g_SpawnManager::Get().m_luaState->GetTop() == 1 )
				assert( 0 ); // an error occured.
		}
		*/
	}
}