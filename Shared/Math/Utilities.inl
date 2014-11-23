#include "Utilities.h"

/*
namespace GMathUtil
{
	bool RayIntersectsTriangle( const GVector3& i_start, const GVector3 i_direction, float distance,
									const GVector3& i_vert_zero, const GVector3& i_vert_one, const GVector3& i_vert_two, 
									float& o_distance, GVector3& o_normal )
	{
		/*
		cyPoint3f e1 = v[f[faceID].v[1]] - v[f[faceID].v[0]];
		cyPoint3f e2 = v[f[faceID].v[2]] - v[f[faceID].v[0]];

		cyPoint3f normal = e1.Cross( e2 );

		cyPoint3f normalized = normal;
		normalized.Normalize();

		float t = normalized.Dot( ray.p - v[f[faceID].v[0]] )/(-normalized.Dot( ray.dir ));

		if( t < GYRO_EPSILON || t > hInfo.z )
			return false;

		cyPoint3f p = ray.p + ray.dir * t;

		cyPoint3f H = p - v[f[faceID].v[0]];

		cyPoint3f a1Vector = e1.Cross( H );
		float a1 = a1Vector.Length() / 2.0f;


		if( a1Vector.Dot( normalized ) < GYRO_EPSILON )
			return false;

		float a0 = normal.Length() / 2.0f;
		if( a0 < GYRO_EPSILON )
			return false;

		float alpha = a1 / a0;

		if( (alpha < - GYRO_EPSILON) || (alpha > 1 + GYRO_EPSILON) )
			return false;

		cyPoint3f a2Vector = H.Cross( e2 );
		float a2 = a2Vector.Length() / 2.0f;

		if( a2Vector.Dot( normalized ) < GYRO_EPSILON )
			return false;

		float beta = a2 /a0;

		if( (beta < -GYRO_EPSILON) || (beta > 1 + GYRO_EPSILON) )
			return false;

		if( alpha + beta > 1 + GYRO_EPSILON )
			return false;

		cyPoint3f lerpedNormal = GetNormal( faceID, cyPoint3f( (1 -(alpha + beta)), beta, alpha ));

		hInfo.N = lerpedNormal;
		hInfo.p = p; // :(
		hInfo.z = t;
		hInfo.uvw = GetTexCoord( faceID, cyPoint3f( (1 -(alpha + beta)), beta, alpha ) );

		// fill out ray differential...
		ray.EllipseAt( hInfo.z, hInfo.N, hInfo.duvw[0], hInfo.duvw[1] );
		hInfo.duvw[0] /= 3.2f ; // multiply by 2...
		hInfo.duvw[1] /= 3.2f ; // multiply by 2...

		// all true for now...
		//if( lerpedNormal.Dot( ray.dir ) < GYRO_EPSILON )
		hInfo.front = true;
		//else
		//	hInfo.front = false; 
		return false;
	}
}
*/