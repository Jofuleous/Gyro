#include "Utilities.h"


const float GYRO_EPSILON = 0.001f;

namespace GMathUtil
{
	// Möller–Trumbore intersection is probably faster, but I'm going to use my algorithm for now....
	bool RayIntersectsTriangle( const GVector3& i_start, const GVector3 i_direction, float distance,
								const GVector3& i_vert_zero, const GVector3& i_vert_one, const GVector3& i_vert_two, 
								float& o_distance, GVector3& o_normal )
	{
		GVector3 e1 = i_vert_two - i_vert_zero; // ac
		GVector3 e2 = i_vert_one - i_vert_zero; // ab
		GVector3 segment = i_start - (i_start + (i_direction * distance )); // qp

		GVector3 normal = e2.Cross( e1 );

		// the ray is moving away from the plane, can't hit.
		float d = normal.Dot( segment );
		if( d <= 0.0f ) return false;

		GVector3 toStart = i_start - i_vert_zero;
		normal.Normalize();
		float t = toStart.Dot( normal );
		if( t < 0.0f ) return false; // I guess return if we hit a back face? :P
		if( t > distance )
			return false;

		// Compute barycentric coordinate components and test if within bounds
		GVector3 e = segment.Cross( toStart );
		float v = e1.Dot( e );
		if( v < 0.0f || v > d ) return 0;

		float w = -e2.Dot( e );
		if( w < 0.0f || v + w > d ) return 0;

		float ood = 1.0f / d;
		//t *= ood;
		// v *= ood;
		// w *= ood;
		//u = 1.0f - v - w;
		o_distance = t;
		o_normal = normal;

		return true;
	}

	bool ColTriangle::RayIntersects( const GVector3& i_start, const GVector3 i_direction, float distance,
								float& o_distance, GVector3& o_normal )
	{

		GVector3 e2 = m_two - m_zero; // ac
		GVector3 e1 = m_one - m_zero; // ab

		GVector3 normal = e1.Cross( e2 );

		GVector3 normalized = normal;
		normalized.Normalize();

		// almost perpendicular?
		if( fabs( normalized.Dot( i_direction ) ) < GYRO_EPSILON )
			return false;

		float t = normalized.Dot( i_start - m_zero )/(-normalized.Dot( i_direction ));

		if( t < GYRO_EPSILON || t > o_distance )
			return false;

		GVector3 p = i_start + i_direction * t;

		GVector3 H = p - m_zero;

		GVector3 a1Vector = e1.Cross( H );
		float a1 = a1Vector.Length() / 2.0f;


		if( a1Vector.Dot( normalized ) < GYRO_EPSILON )
			return false;

		float a0 = normal.Length() / 2.0f;
		if( a0 < GYRO_EPSILON )
			return false;

		float alpha = a1 / a0;

		if( (alpha < - GYRO_EPSILON) || (alpha > 1 + GYRO_EPSILON) )
			return false;

		GVector3 a2Vector = H.Cross( e2 );
		float a2 = a2Vector.Length() / 2.0f;

		if( a2Vector.Dot( normalized ) < GYRO_EPSILON )
			return false;

		float beta = a2 /a0;

		if( (beta < -GYRO_EPSILON) || (beta > 1 + GYRO_EPSILON) )
			return false;

		if( alpha + beta > 1 + GYRO_EPSILON )
			return false;

		o_distance = t;
		o_normal = normalized;
		return true;
	}

	bool ColTriangle::AABBContains( GVector3& i_origin, float i_extentLength ) const
	{
		// check all vertices X's.
		float max = i_origin._x + i_extentLength;
		float min = i_origin._x - i_extentLength;
		if( !(max > m_zero._x && min < m_zero._x && max > m_one._x && min < m_one._x && max > m_two._x && min < m_two._x) )
			return false;

		max = i_origin._y + i_extentLength;
		min = i_origin._y - i_extentLength;
		if( !(max > m_zero._y && min < m_zero._y && max > m_one._y && min < m_one._y && max > m_two._y && min < m_two._y) )
			return false;

		max = i_origin._z + i_extentLength;
		min = i_origin._z - i_extentLength;
		if( !(max > m_zero._z && min < m_zero._z && max > m_one._z && min < m_one._z && max > m_two._z && min < m_two._z) )
			return false;

		return true;
	}
}