#ifndef __GAABB_H_
#define __GAABB_H_

#include "Math/GVector3.h"
#include "Math/GMatrix4.h"

class GAABB
{
public:
	GAABB( );
	GAABB( const GVector3& i_Max, const GVector3& i_Min );
	GAABB( float i_xMax, float i_yMax, float i_zMax, float i_xMin, float i_yMin, float i_zMin );

	GAABB( const GVector3& i_center, float i_halfX, float i_halfY, float i_halfZ );

	bool RayIntersects( GVector3& i_origin, GVector3& i_dir, float i_length ); 
	bool RayIntersects( GVector3& i_origin, GVector3& i_dir, GMatrix4& i_parent, float i_length );
	bool PointInside( GVector3& i_point );
	bool PointInside( GVector3& i_point, GMatrix4& i_parent );

	// These are both somewhat in-efficient right now.  Optimize later...
	bool RayIntersectsDistance( GVector3& i_origin, GVector3& i_dir, float& o_distance );
	bool RayIntersectsDistance( GVector3& i_origin, GVector3& i_dir, GMatrix4& i_parent, float& o_distance );

	float m_HalfX;
	float m_HalfY;
	float m_HalfZ;

	GVector3 m_Center;
};


#endif