#ifndef __MATH_VECTOR3_H_
#define __MATH_VECTOR3_H_

//========================================================================
// GVector3.h
// Author: Jon Futch
// Created: Fall 2012
//
// A basic vector3 class.
//========================================================================

class GVector3
{
public:

	float _x, _y, _z;

							GVector3( void ) {}
	inline					GVector3( float i_x, float i_y, float i_z );
	inline					GVector3(float i_val);
	inline					GVector3( const GVector3& i_v );

	// Accessors

	inline const float		x( void ) const;
	inline const float		y( void ) const;
	inline const float		z( void ) const;

	// Setters

	inline void				Set( float i_x, float i_y, float i_z );
	inline void				x( const float i_x );
	inline void				y( const float i_y);
	inline void				z( const float i_z );

	// Assignment operators

	inline const GVector3&	operator=(const GVector3& rhs);
	inline const GVector3&	operator+=(const GVector3& rhs);
	inline const GVector3&	operator-=(const GVector3& rhs);
	inline const GVector3&	operator*=(const GVector3& rhs);
	inline const GVector3&	operator*=(const float rhs);
	inline const GVector3&	operator/=(const float rhs);
	
	//Utility!

	inline GVector3&		Normalize( void );
	inline GVector3			Normal( void ) const;
	inline float			Length( void ) const;
	inline float			LengthXZ( void ) const;
	inline float			LengthXZSquared( void ) const;
	inline float			Dot( const GVector3& rhs ) const;
	inline GVector3			Cross( const GVector3& rhs) const;
	inline float			LengthSquared() const;


	inline void				ToD3DX( void* o_vector ) const;

	static GVector3			FromD3DX( const void* i_vector );
	static GVector3			Lerp( const GVector3& i_source, const GVector3& i_dest, float i_destPercent );
	static const GVector3	Zero;
	static const GVector3	One;
	static const GVector3	Up;
	static const GVector3	Down;
	static const GVector3	Right;
	static const GVector3	Forward;
	static GVector3			SafeReciprocal( const GVector3& i_vector );
	static void				Vector3_UnitTest();
};

#include "GVector3.inl"

#endif