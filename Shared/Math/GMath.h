#ifndef __MATH_GMATH_H_
#define __MATH_GMATH_H_

//=============================================
// GMath.h
// Author:	Jon Futch
// Created:	Unknown 2012-2014
//
// Math utility functions.
//=============================================

namespace GMath
{
	static const float Pi =  3.14159265359f;

	// Returns the angle in radians from degrees.
	inline float Deg2Rad( float degrees )
	{
		return ( degrees *  Pi ) / 180.f;
	}

	// Returns the angle in degrees from radians.
	inline float Rad2Deg( float i_radians )
	{
		return ( i_radians * 180.f ) / Pi;
	}

	// Returns the max of two floats.
	inline float Max(float i_one, float i_two)
	{
		if (i_one > i_two)
			return i_one;
		return i_two;
	}

	// Returns the min of two floats.
	inline float Min(float i_one, float i_two)
	{
		if (i_one > i_two)
			return i_two;
		return i_one;
	}

	// Returns the interpolated value between two floats.
	inline float Lerp( float i_source, float i_dest, float i_destPercent )
	{
		return i_source * (1.0f - i_destPercent) + i_dest * i_destPercent;
	}

	// Returns the smooth step between 2 floats.
	inline float SmoothStep(float i_min, float i_max, float i_value )
	{
		float val = Max(0.f, Min(1.f, (i_value - i_min) / (i_max - i_min)));
		return val * val * (3.f - 2.f * val);
	}

	// Returns whether the float is positive or negative.
	inline float Sign( float i_input )
	{
		return i_input / fabs( i_input );
	}

	// Move val to the target val at a given rate.
	inline void MoveToValue( float& io_val, float i_target, float i_rate )
	{
		if( i_target < io_val )
		{
			io_val -= i_rate;
			if( i_target > io_val )
				io_val = i_target;
		}
		else if( i_target > io_val )
		{
			io_val += i_rate;
			if( i_target < io_val )
				io_val = i_target;
		}
	}

	// Clamp val to the min or max.
	inline void Clamp( float* val, float i_min, float i_max )
	{
		if( *val > i_max )
			*val = i_max;
		else if( *val < i_min )
			*val = i_min;
	}

	// Return val clamped to the min or max.
	inline float Clamp(float val, float i_min, float i_max)
	{
		if (val > i_max)
			val = i_max;
		else if (val < i_min)
			val = i_min;

		return val;
	}

	// Reflect a vector about a normal.
	inline void Reflect( GVector3& o_result, const GVector3& i_normal, const GVector3& i_vector )
	{
		// This hasn't been tested.
		//o_result = 2.0f * i_normal * ( i_vector.Dot( i_normal ) ) - i_vector;
	}

	inline bool		IsNAN( float i_val );
	inline bool		AlmostEqual( float A, float B, int maxUlps );
	inline bool		GoodEnough( float A, float B );
	inline bool		IsExactlyZero( float i_val );

#ifdef _DEBUG
	inline bool		AlmostEqual_UnitTest( void );
#endif

}

#include "GMath.inl"

#endif