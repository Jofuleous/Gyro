#ifndef _GE_TRIGGER_DATA_H_
#define _GE_TRIGGER_DATA_H_

#include "../GETypeData.h"
#include "LuaPlus/LuaState.h"
#include <vector>

// this is going to need a lot more work later.

class GETriggerData : public GETypeData 
{
public:
	std::vector<char*> m_onEnters;
	std::vector<char*> m_onExits;

	GETriggerData()
	{
		m_type = GEditorObject::TRIGGER;
	}

	virtual ~GETriggerData()
	{
		for( int i = 0; i < m_onEnters.size(); i++ )
			delete m_onEnters[i];
		for( int i = 0; i < m_onExits.size(); i++ )
			delete m_onExits[i];
	}

	virtual bool LoadFromLua( LuaPlus::LuaObject& i_obj  )
	{
		if( i_obj["OnEnter"].IsNil() )
			return false;

		for( int i = 1; i <= i_obj["OnEnter"].GetCount(); i++ )
		{
			const char* temp = i_obj["OnEnter"][i].GetString();
			int len = strlen( temp ) +1;
			char* tempTwo = new char[64];
			strcpy( tempTwo, temp );
			m_onEnters.push_back( tempTwo );
		}

		if( i_obj["OnExit"].IsNil() )
			return false;

		for( int i = 1; i <= i_obj["OnExit"].GetCount(); i++ )
		{
			const char* temp = i_obj["OnExit"][i].GetString();
			int len = strlen( temp ) +1;
			char* tempTwo = new char[64];
			strcpy( tempTwo, temp );
			m_onExits.push_back( tempTwo );
		}

		return true;
	}

	// "Trigger" table is the parent.
	virtual bool WriteToLua( LuaPlus::LuaObject& o_parent, GEditorObject* i_object )
	{
		LuaPlus::LuaObject& obj = o_parent.CreateTable( "Trigger" );
		LuaPlus::LuaObject& temp = obj.CreateTable( "OnEnter", m_onEnters.size() );

		for( int i = 1; i <= m_onEnters.size(); i++ )
			temp.SetString( i, m_onEnters[i-1] );

		temp = obj.CreateTable( "OnExit", m_onExits.size() );
		for( int i = 1; i <= m_onExits.size(); i++ )
			temp.SetString( i, m_onExits[i-1] );

		// eventually, the collisions should be more robust...allowing architects to specify collision bits.
		// for now, we're just going to create the actual collision component data...
		obj.SetBoolean( "IsTrigger", true );
		temp = obj.CreateTable( "CollisionBits", 1 );
		temp.SetString( 1, "TRIGGER" ); //
		temp = obj.CreateTable( "CollidesWith", 1 );
		temp.SetString( 1, "PLAYER" ); // just collide with players for now?
		temp = obj.CreateTable( "Callback" );
		temp.CreateTable( "Trigger" );
		temp = obj.CreateTable( "AABB" );
		LuaPlus::LuaObject& tempTwo = temp.CreateTable( "Center", 3 ); // the default trigger has an FBX of 10x10x10
		tempTwo.SetNumber( 1, 0.0f );
		tempTwo.SetNumber( 2, 0.0f );
		tempTwo.SetNumber( 3, 0.0f );
		temp.SetNumber( "HalfX", 10.0f * i_object->m_scale._x ); // scale "hack" for now.
		temp.SetNumber( "HalfY", 10.0f * i_object->m_scale._y );
		temp.SetNumber( "HalfZ", 10.0f * i_object->m_scale._z );

		return true;
	}

	void SetOnEnter( char* onEnter )
	{
		if( m_onEnters.size() >= 1 )
		{
			char* newOnEnter = new char[strlen( onEnter ) + 1];
			delete m_onEnters[0];
			strcpy( newOnEnter, onEnter );
			m_onEnters[0] = newOnEnter;
		}
		else
		{
			char* newOnEnter = new char[strlen( onEnter ) + 1];
			strcpy( newOnEnter, onEnter );
			m_onEnters.push_back( newOnEnter );	
		}
	}

	void SetOnExit( char* onExit )
	{
		if( m_onEnters.size() >= 1 )
		{
			char* newOnExit = new char[strlen( onExit ) + 1];
			delete m_onExits[0];
			strcpy( newOnExit, onExit );
			m_onExits[0] = newOnExit;
		}
		else
		{
			char* newOnExit = new char[strlen( onExit ) + 1];
			strcpy( newOnExit, onExit );
			m_onExits.push_back( newOnExit );	
		}
	}

};

#endif