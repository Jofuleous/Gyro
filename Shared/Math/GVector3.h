#ifndef _VECTOR3_H_
#define _VECTOR3_H_

class GMemoryPool;

class GVector3
{
public:

	float _x, _y, _z;

	//we may want this to be specific to levels.  keep public for now.
	static GMemoryPool* m_allocator;

	//constructors
							GVector3( void ) {}
	inline					GVector3( float i_x, float i_y, float i_z );
	inline					GVector3(float i_val);
	//copy
	inline					GVector3( const GVector3& i_v );
	inline void				Set( float i_x, float i_y, float i_z );

	//accessors...kind of useless now that i've make the members public.
	inline const float		x( void ) const;
	inline const float		y( void ) const;
	inline const float		z( void ) const;

	//setters
	inline void				x( const float i_x );
	inline void				y( const float i_y);
	inline void				z( const float i_z );

	//assignment operators
	inline const GVector3&	operator=(const GVector3& rhs);
	inline const GVector3&	operator+=(const GVector3& rhs);
	inline const GVector3&	operator-=(const GVector3& rhs);
	inline const GVector3&	operator*=(const GVector3& rhs);
	inline const GVector3&	operator*=(const float rhs);
	inline const GVector3&	operator/=(const float rhs);
	inline void*			operator new( size_t size );
	inline void				operator delete( void* addr );	

	
	//Utility!
	inline GVector3&		Normalize( void );
	inline GVector3			Normal( void ) const;
	inline float			Length( void ) const;
	inline float			LengthXZ( void ) const;
	inline float			LengthXZSquared( void ) const;
	inline float			Dot( const GVector3& rhs ) const;
	inline GVector3			Cross( const GVector3& rhs) const;
	inline float			LengthSquared() const;

	// this is a basic memcpy.  probably isn't that fast.
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