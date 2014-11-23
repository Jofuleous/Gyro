#ifndef _GE_TYPEDATA_H_
#define _GE_TYPEDATA_H_

#include "LuaPlus/LuaPlus.h"
#include "GEditorObject.h"
/*
Eventually, make this all data driven from some kind of lua template...this all needs to be
working, though.  Get the grades.
*/
class GETypeData
{
public:
	GEditorObject::GObjectType m_type;
	virtual bool LoadFromLua( LuaPlus::LuaObject& i_obj ) = 0;
	virtual bool WriteToLua( LuaPlus::LuaObject& o_parent, GEditorObject* i_obj ) = 0;
};

#endif