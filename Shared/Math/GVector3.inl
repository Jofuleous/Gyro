#include <math.h>
#include <string.h>
#include <assert.h>
#include "Memory/GMemoryPool.h"
#include "GVector3.h"
#include "GMath.h"


GVector3::GVector3(float i_x, float i_y, float i_z):
	_x( i_x ),
	_y( i_y ),
	_z( i_z )

{
	assert( !GMath::IsNAN(i_x) );
	assert( !GMath::IsNAN(i_y) );
	assert( !GMath::IsNAN(i_z) );
}

GVector3::GVector3( float i_val )
:	_x( i_val ),
	_y( i_val ),
	_z( i_val )
{
	assert(!GMath::IsNAN(i_val));
}

GVector3::GVector3(const GVector3& i_v):
	_x( i_v._x ),
	_y( i_v._y ),
	_z( i_v._z )
{
	assert( !GMath::IsNAN(i_v._x) );
	assert( !GMath::IsNAN(i_v._y) );
	assert( !GMath::IsNAN(i_v._z) );
}

void GVector3::Set( float i_x, float i_y, float i_z )
{
	_x = i_x;
	_y = i_y;
	_z = i_z;
}

const float GVector3::x( void ) const
{
	return _x;
}

const float GVector3::y( void ) const
{
	return _y;
}

const float GVector3::z( void ) const
{
	return _z;
}


void GVector3::x( const float i_x )
{
	assert( !GMath::IsNAN(i_x) );

	_x = i_x;
}

void GVector3::y( const float i_y )
{
	assert( !GMath::IsNAN(i_y) );

	_y = i_y;
}

void GVector3::z( const float i_z )
{
	assert( !GMath::IsNAN(i_z) );

	_z = i_z;
}

const GVector3& GVector3::operator=( const GVector3& rhs )
{
	assert( !GMath::IsNAN(rhs._x) );
	assert( !GMath::IsNAN(rhs._y) );
	assert( !GMath::IsNAN(rhs._z) );

	_x = rhs._x;
	_y = rhs._y;
	_z = rhs._z;

	return *this;
}

const GVector3& GVector3::operator-=( const GVector3& rhs )
{
	_x -= rhs._x;
	_y -= rhs._y;
	_z -= rhs._z;

	return *this;
}

const GVector3& GVector3::operator+=( const GVector3& rhs )
{
	_x += rhs._x;
	_y += rhs._y;
	_z += rhs._z;

	return *this;
}


const GVector3& GVector3::operator*=( const GVector3& rhs )
{
	_x *= rhs._x;
	_y *= rhs._y;
	_z *= rhs._z;

	return *this;
}

const GVector3& GVector3::operator*=( const float rhs )
{
	_x *= rhs;
	_y *= rhs;
	_z *= rhs;

	return *this;
}

const GVector3& GVector3::operator/=( const float value )
{
	assert( !GMath::IsExactlyZero( value ) );
	_x /= value;
	_y /= value;
	_z /= value;

	return *this;
}

void* GVector3::operator new( size_t size )
{	
	//make sure the dynamic initializer didn't fail on creation.
	assert( m_allocator );
	return m_allocator->Allocate( size );
}

void GVector3::operator delete( void* addr )
{
	GVector3::m_allocator->Free( addr );
}

GVector3& GVector3::Normalize( void )
{
	float mag = Length();
	assert( !GMath::IsExactlyZero( mag ) );

	_x = _x / mag;
	_y = _y / mag;
	_z = _z / mag;

	return *this;
}

void GVector3::ToD3DX( void* o_vector ) const
{
	memcpy( o_vector, this, sizeof( float ) * 3 );
}

GVector3 GVector3::Normal( void ) const
{
	float mag = Length();
	assert( !GMath::IsExactlyZero( mag ) );

	return GVector3( _x/mag, _y/mag, _z/mag );
}

float GVector3::Dot( const GVector3& i_v ) const
{
	return _x * i_v.x() + _y * i_v.y() + _z * i_v.z();
}

// (y*pt.z-z*pt.y, z*pt.x-x*pt.z, x*pt.y-y*pt.x); just in case?
GVector3 GVector3::Cross( const GVector3& b) const
{
	return GVector3( _y*b.z() - _z*b.y(), _z*b.x() - _x*b.z(), _x*b.y() - _y*b.x() );
}

float GVector3::LengthSquared( void ) const
{
	return _x*_x + _y*_y + _z*_z;
}

float GVector3::Length( void ) const
{
	return sqrt( _x * _x + _y * _y + _z * _z );
}

float GVector3::LengthXZ( void ) const
{
	return sqrt( _x * _x + _z * _z );
}

float GVector3::LengthXZSquared( void ) const
{
	return _x * _x + _z * _z;
}

/*
void Reflect( GVector3& o_result, GVector3& i_normal, GVector3& i_reflecting )
{
	o_result = i_normal;
}
*/

inline const GVector3 operator+( const GVector3& lhs, const GVector3& rhs )
{
	return GVector3(lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z() );
}

inline const GVector3 operator-( const GVector3& lhs, const GVector3& rhs )
{
	return GVector3( lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z() );
}

inline const GVector3 operator-( const GVector3& rhs )
{
	return GVector3( -rhs.x(),-rhs.y(),-rhs.z() );
}

inline const GVector3 operator*( const GVector3& lhs, const GVector3& rhs )
{
	return GVector3( lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z() );
}

inline const GVector3 operator*( const GVector3& lhs, const float value)
{
	return GVector3( lhs.x() * value, lhs.y() * value, lhs.z() * value );
}

inline const GVector3 operator*( const float value, const GVector3& rhs)
{
	return GVector3( rhs.x() * value, rhs.y() * value, rhs.z() * value );
}

inline const GVector3 operator/( const GVector3& lhs, const float value)
{
	assert( !GMath::IsExactlyZero( value ) );
	return GVector3( lhs.x() / value, lhs.y() / value, lhs.z() / value );
}

//This could be useful in some situations haha...
inline bool operator==( const GVector3& lhs, const GVector3& rhs )
{
	if( GMath::AlmostEqual(lhs.x(), rhs.x()) && GMath::AlmostEqual(lhs.y(), rhs.y()) && GMath::AlmostEqual(lhs.z(), rhs.z()) )
		return true;
	return false;
}

inline GVector3 GVector3::SafeReciprocal( const GVector3& i_vector )
{
	assert( i_vector._x != 0.0f && i_vector._y != 0.0f && i_vector._z != 0.0f );
	return GVector3(1.0f / i_vector._x, 1.0f / i_vector._y, 1.0f / i_vector._z );
}