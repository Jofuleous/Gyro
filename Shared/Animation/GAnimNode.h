#ifndef __GANIMNODE_H_
#define __GANIMNODE_H_

#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaState.h"
#include "LuaPlus/LuaTableIterator.h"

class GSkeletonInstance;
class GAnimClip;

class GAnimNode
{
	GAnimNode(){}
public:
	static GAnimNode* Create( LuaPlus::LuaObject& i_obj );
	void		Begin();
	void		End();
	bool		Update( GSkeletonInstance* i_skelly, float i_dt );
	GAnimClip*	m_clip;
	bool		m_loop;
	float		m_blendInAmt;
	float		m_trimStart;
	float		m_trimEnd;
	float		m_playRate;
	float		m_currTime;
};

#endif