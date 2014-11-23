#ifndef __COLLISION_UTIL_H_
#define __COLLISION_UTIL_H_

#include "Math/GVector3.h"
#include "Math/GMatrix4.h"
#include "CollisionManager.h" // not sure if i like this.  possibly clean up later..
#include "Math/GAABB.h"

namespace GCollision
{
	bool SATAABB( const GAABB& i_boxOne, const GVector3& i_boxOnePos, const GVector3& i_boxOneVel,
		const GAABB& i_boxTwo, const GVector3& i_boxTwoPos, const GVector3& i_boxTwoVel, float i_time, GContact& o_contact);

	bool SATOBBCheck( const GAABB& i_boxA, const GMatrix4& i_boxATransform, const GVector3& i_boxAVel,
		const GAABB& i_boxB, const GMatrix4& i_boxBTransform, const GVector3& i_boxBVel, float i_time, GContact& o_contact );

	bool SAT_OBB_AABB_Check( const GAABB& i_boxA, const GMatrix4& i_boxATransform, const GVector3& i_boxAVel,
		const GAABB& i_boxB, const GVector3 i_boxBPos, const GVector3& i_boxBVel, float i_time, GContact& o_contact );

	bool SATAABBUnitTests( );
}

#include "CollisionUtil.inl"

#endif