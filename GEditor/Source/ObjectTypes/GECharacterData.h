#ifndef _GE_CHARACTER_DATA_H_
#define _GE_CHARACTER_DATA_H_

#include "../GETypeData.h"
#include "LuaPlus/LuaState.h"

class GECharacterData : public GETypeData 
{
public:
	const char* m_characterFile;

	GECharacterData( ) : m_characterFile( NULL )
	{
		m_type = GEditorObject::CHARACTER; // wtf can't use initializer...
	}

	virtual ~GECharacterData()
	{
		delete m_characterFile;
	}

	virtual bool LoadFromLua( LuaPlus::LuaObject& i_obj  )
	{
		if( !i_obj["CharacterFile"].IsNil() )
		{
			const char* temp = i_obj["CharacterFile"].GetString();
			int len = strlen( temp ) +1;
			char* tempTwo = new char[len];
			strcpy( tempTwo, temp );
			m_characterFile = (const char*) tempTwo;
			return true;
		}

		return false;
	}

	virtual bool WriteToLua( LuaPlus::LuaObject& o_parent, GEditorObject* i_object )
	{
		LuaPlus::LuaObject& obj = o_parent.CreateTable( "Character" );
		obj.SetString( "CharacterFile", m_characterFile );
		return true;
	}

	void SetCharacterFile( const char* i_file )
	{
		int len = strlen( i_file ) +1;
		char* tempTwo = new char[len];
		strcpy( tempTwo, i_file );

		if( m_characterFile )
			delete m_characterFile;

		m_characterFile = tempTwo;
	}
};

#endif