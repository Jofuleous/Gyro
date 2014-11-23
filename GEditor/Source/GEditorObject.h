#ifndef __GEDITOR_OBJECT_H_
#define __GEDITOR_OBJECT_H_

/*
	THESE OBJECTS CAN ONLY BE CREATED AND DESTROYED BY GEDITORSCENE.
	DO NOT HOLD A POINTER TO THIS.  IF ANYTHING DOES, THE PARADIGM BREAKS.
	LET'S NOT DO SMART POINTERS OR HANDLES UNTIL WE HAVE TO.
*/

#include "LuaPlus/LuaPlus.h"
#include "ResourceTypes/Entity.h"
#include "Math/GAABB.h"
#include <vector>
#include "Math/GVector3.h"

class GETypeData;

class GEditorObject
{
public:
	friend class GEditorScene;

	enum GObjectType
	{
		STATIC,
		TRIGGER,
		CHARACTER,
		WAYPOINTSYS,
	};

	GVector3				m_position;
	GVector3				m_scale;
	float					m_yOrientation;


	GObjectType				m_type;
	GETypeData*				m_typeData;

	GAABB					m_AABB;

	std::vector<Entity*>	m_entities;

	bool					m_useFBXGeometry;
	bool					m_Active;
	char*					m_fbxFileName;
	char*					m_name;

	void					SetTypeData( GObjectType i_type );
	void					UpdatePosition( const GVector3& i_position );
	void					UpdateScale( const GVector3& i_scale );
	void					SetName( char* i_name );
	void					SetFbxName( char* i_name );

	// in order for entities to show in the correct location, make sure the object transform is set before
	// calling this.  this is a hack for now.
	bool					LoadEntitiesFromFBX( const char* i_fbxName );
	void					OrientEntities();

protected:

	static GEditorObject*	Create( const char* i_name, LuaPlus::LuaObject i_luaObj );
	static GEditorObject*	Create( GObjectType i_type, const GVector3& i_position );
	static bool				Destroy( GEditorObject* i_object );

	bool					WriteLuaTable( LuaPlus::LuaObject i_parent );

	GEditorObject() : m_name( NULL ), m_fbxFileName( NULL )
	{
	}
	~GEditorObject();
};

#endif