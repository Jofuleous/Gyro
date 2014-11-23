#ifndef __MATH_UTILITIES_H_
#define __MATH_UTILITIES_H_

#include "GVector3.h"

namespace GMathUtil
{
	bool RayIntersectsTriangle( const GVector3& i_start, const GVector3 i_direction, float distance,
									const GVector3& i_vert_zero, const GVector3& i_vert_one, const GVector3& i_vert_two, 
									float& o_distance, GVector3& o_normal );

	// Returns the distance to a plane.  If the return value is negative, the plane is never hit...so it's behind you.
	// http://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
	inline float RayPlaneIntersect( const GVector3& i_rayDir, const GVector3 i_rayOrigin, const GVector3& i_planeNormal, const GVector3& i_pointOnPlane )
	{
		return (i_pointOnPlane - i_rayOrigin).Dot( i_planeNormal ) / i_rayDir.Dot( i_planeNormal );
	}

	class ColTriangle
	{
	public:
		GVector3 m_zero;
		GVector3 m_one;
		GVector3 m_two;

		// required by GOctree...
		bool RayIntersects( const GVector3& i_start, const GVector3 i_direction, float distance, float& o_distance, GVector3& o_normal );
		bool AABBContains( GVector3& i_origin, float i_extentLength ) const;
	};
}

#endif