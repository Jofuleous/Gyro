#ifndef __ANIMATION_GANIMNODE_H_
#define __ANIMATION_GANIMNODE_H_

//==================================================================================
// GAnimNode.h
// Author:	Jon Futch
// Created:	December, 2014
//
// Represents the way that a clip should be played.
// TODO: It is possible that this can (and probably should) be instanced.
// Right now, though, it's probably not eating up enough memory to warrant this...
//==================================================================================

#include "LuaPlus/LuaPlus.h"


class GSkeletonInstance;
class GAnimClip;

class GAnimNode
{
	GAnimNode(){}
public:
	static GAnimNode* Create( LuaPlus::LuaObject& i_obj );
	float		CalculateBlendInto();
	void		Begin();
	void		End();
	bool		Update( GSkeletonInstance* i_skelly, float i_dt );
	GAnimClip*	m_clip;
	bool		m_loop;
	float		m_blendInEndFrame;
	float		m_trimStart;
	float		m_trimEnd;
	float		m_playRate;
	float		m_currTime;
};

#endif