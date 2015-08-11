#ifndef __MATH_GQUAT_H_
#define __MATH_GQUAT_H_

//====================================================================
// GQuat.h
// Author: Irrlicht Bros.  I just copied it from their engine.  I am not a quat ninja yet.
// Created: December 2014 
//
// A quaternion class based on the Irrlict Engine's implementation
//====================================================================

#include "GVector3.h"
#include "GMatrix4.h"

class GQuat
{
public:

	//! Default Constructor
	GQuat( ) : m_X(0.0f), m_Y(0.0f), m_Z(0.0f), m_W(1.0f) {}

	//! Constructor
	GQuat( float x, float y, float z, float w ) : m_X( x ), m_Y( y ), m_Z( z ), m_W( w ) { }

	//! Constructor which converts euler angles (radians) to a GQuat
	GQuat( float x, float y, float z );

	//! Constructor which converts euler angles (radians) to a GQuat
	GQuat( const GVector3& vec );

	//! Constructor which converts a matrix to a GQuat
	GQuat( const GMatrix4& mat );

	//! Equalilty operator
	bool operator==( const GQuat& other ) const;

	//! inequality operator
	bool operator!=( const GQuat& other ) const;

	//! Assignment operator
	inline GQuat& operator=( const GQuat& other );

	//! Matrix assignment operator
	inline GQuat& operator=( const GMatrix4& other );

	//! Add operator
	GQuat operator+( const GQuat& other ) const;

	//! Multiplication operator
	GQuat operator*( const GQuat& other ) const;

	//! Multiplication operator with scalar
	GQuat operator*( float s ) const;

	//! Multiplication operator with scalar
	GQuat& operator*=( float s );

	//! Multiplication operator
	GVector3 operator*( const GVector3& v ) const;

	//! Multiplication operator
	GQuat& operator*=( const GQuat& other );

	//! Calculates the dot product
	inline float DotProduct( const GQuat& other ) const;

	//! Sets new GQuat
	inline GQuat& Set( float x, float y, float z, float w );

	//! Sets new GQuat based on euler angles (radians)
	inline GQuat& Set( float x, float y, float z );

	//! Sets new GQuat based on euler angles (radians)
	inline GQuat& Set( const GVector3& vec );

	//! Sets new GQuat from other GQuat
	inline GQuat& Set( const GQuat& quat );

	//! returns if this GQuat equals the other one, taking floating point rounding errors into account
	inline bool Equals( const GQuat& other,
		const float tolerance = 0.000000001f ) const;

	//! Normalizes the GQuat
	inline GQuat& Normalize( );

	//! Creates a matrix from this GQuat
	GMatrix4 GetMatrix( ) const;

	//! Creates a matrix from this GQuat
	void GetMatrix( GMatrix4 &dest, const GVector3 &translation = GVector3() ) const;

	/*!
	Creates a matrix from this GQuat
	Rotate about a center point
	shortcut for
	GQuat q;
	q.rotationFromTo ( vin[i].Normal, forward );
	q.getMatrixCenter ( lookat, center, newPos );

	GMatrix4 m2;
	m2.setInverseTranslation ( center );
	lookat *= m2;

	GMatrix4 m3;
	m2.setTranslation ( newPos );
	lookat *= m3;

	*/
	void GetMatrixCenter( GMatrix4 &dest, const GVector3 &center, const GVector3 &translation ) const;

	//! Creates a matrix from this GQuat
	inline void GetMatrix_Transposed( GMatrix4 &dest ) const;

	//! Inverts this GQuat
	GQuat& Inverse( );

	//! Set this GQuat to the linear interpolation between two GQuats
	/** \param q1 First GQuat to be interpolated.
	\param q2 Second GQuat to be interpolated.
	\param time Progress of interpolation. For time=0 the result is
	q1, for time=1 the result is q2. Otherwise interpolation
	between q1 and q2.
	*/
	GQuat& Lerp( GQuat q1, GQuat q2, float time );

	//! Set this GQuat to the result of the spherical interpolation between two GQuats
	/** \param q1 First GQuat to be interpolated.
	\param q2 Second GQuat to be interpolated.
	\param time Progress of interpolation. For time=0 the result is
	q1, for time=1 the result is q2. Otherwise interpolation
	between q1 and q2.
	\param threshold To avoid inaccuracies at the end (time=1) the
	interpolation switches to linear interpolation at some point.
	This value defines how much of the remaining interpolation will
	be calculated with lerp. Everything from 1-threshold up will be
	linear interpolation.
	*/
	GQuat& Slerp( GQuat q1, GQuat q2,
		float time, float threshold = .05f );

	//! Create GQuat from rotation angle and rotation axis.
	/** Axis must be unit length.
	The GQuat representing the rotation is
	q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
	\param angle Rotation Angle in radians.
	\param axis Rotation axis. */
	GQuat& FromAngleAxis(float angle, const GVector3& axis);

	//! Fills an angle (radians) around an axis (unit vector)
	void ToAngleAxis(float &angle, GVector3& axis) const;

	//! Output this GQuat to an euler angle (radians)
	void ToEuler(GVector3& euler) const;

	//! Set GQuat to identity
	GQuat& Identify();

	//! Set GQuat to represent a rotation from one vector to another.
	GQuat& RotationFromTo(const GVector3& from, const GVector3& to);

	const static GQuat Identity;

	//! GQuat elements.
	float m_X; // vectorial (imaginary) part
	float m_Y;
	float m_Z;
	float m_W; // real part
};


// Constructor which converts euler angles to a GQuat
inline GQuat::GQuat(float i_x, float i_y, float i_z)
{
	Set( i_x, i_y, i_z );
}


// Constructor which converts euler angles to a GQuat
inline GQuat::GQuat( const GVector3& vec )
{
	Set( vec._x, vec._x, vec._x );
}

// Constructor which converts a matrix to a GQuat
inline GQuat::GQuat( const GMatrix4& mat )
{
	(*this) = mat;
}

// equal operator
inline bool GQuat::operator==( const GQuat& other ) const
{
	return ((m_X == other.m_X) &&
		(m_Y == other.m_Y) &&
		(m_Z == other.m_Z) &&
		(m_W == other.m_W));
}

// inequality operator
inline bool GQuat::operator!=( const GQuat& other ) const
{
	return !(*this == other);
}

// assignment operator
inline GQuat& GQuat::operator=( const GQuat& other )
{
	m_X = other.m_X;
	m_Y = other.m_Y;
	m_Z = other.m_Z;
	m_W = other.m_W;
	return *this;
}

// matrix assignment operator
inline GQuat& GQuat::operator=( const GMatrix4& m )
{
	// 0 1 2 3
	// 4 5 6 7
	// 8 9 10 11
	// 12 13 14 15
	//const float diag = m(0,0) + m(1,1) + m(2,2) + 1;
	const float diag = m(0,0) + m(1,1) + m(2,2) + 1;

	if (diag > 0.0f)
	{
		const float scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		// TODO: speed this up
		m_X = (m(1,2) - m(2,1)) / scale;
		m_Y = (m(2,0) - m(0,2)) / scale;
		m_Z = (m(0,1) - m(1,0)) / scale;
		m_W = 0.25f * scale;
	}
	else
	{
		if (m(0,0)>m(1,1) && m(0,0)>m(2,2))
		{
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			const float scale = sqrtf(1.0f + m(0,0) - m(1,1) - m(2,2)) * 2.0f;

			// TODO: speed this up
			m_X = 0.25f * scale;
			m_Y = (m(1,0) + m(0,1)) / scale;
			m_Z = (m(0,2) + m(2,0)) / scale;
			m_W = (m(1,2) - m(2,1)) / scale;
		}
		else if (m(1,1)>m(2,2))
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			const float scale = sqrtf(1.0f + m(1,1) - m(0,0) - m(2,2)) * 2.0f;

			// TODO: speed this up
			m_X = (m(1,0) + m(0,1)) / scale;
			m_Y = 0.25f * scale;
			m_Z = (m(2,1) + m(1,2)) / scale;
			m_W = (m(2,0) - m(0,2)) / scale;
		}
		else
		{
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			const float scale = sqrtf(1.0f + m(2,2) - m(0,0) - m(1,1)) * 2.0f;

			// TODO: speed this up
			m_X = (m(2,0) + m(0,2)) / scale;
			m_Y = (m(2,1) + m(1,2)) / scale;
			m_Z = 0.25f * scale;
			m_W = (m(0,1) - m(1,0)) / scale;
		}
	}

	return Normalize();
}


// multiplication operator
inline GQuat GQuat::operator*( const GQuat& other ) const
{
	GQuat tmp;

	tmp.m_W = (other.m_W * m_W) - (other.m_X * m_X) - (other.m_Y * m_Y) - (other.m_Z * m_Z);
	tmp.m_X = (other.m_W * m_X) + (other.m_X * m_W) + (other.m_Y * m_Z) - (other.m_Z * m_Y);
	tmp.m_Y = (other.m_W * m_Y) + (other.m_Y * m_W) + (other.m_Z * m_X) - (other.m_X * m_Z);
	tmp.m_Z = (other.m_W * m_Z) + (other.m_Z * m_W) + (other.m_X * m_Y) - (other.m_Y * m_X);

	return tmp;
}


// multiplication operator
inline GQuat GQuat::operator*( float s ) const
{
	return GQuat(s*m_X, s*m_Y, s*m_Z, s*m_W);
}


// multiplication operator
inline GQuat& GQuat::operator*=( float s )
{
	m_X *= s;
	m_Y *= s;
	m_Z *= s;
	m_W *= s;
	return *this;
}

// multiplication operator
inline GQuat& GQuat::operator*=( const GQuat& other )
{
	return (*this = other * (*this));
}

// add operator
inline GQuat GQuat::operator+(const GQuat& b) const
{
	return GQuat(m_X + b.m_X, m_Y + b.m_Y, m_Z + b.m_Z, m_W + b.m_W);
}

// Creates a matrix from this GQuat
inline GMatrix4 GQuat::GetMatrix() const
{
	GMatrix4 m;
	GetMatrix(m);
	return m;
}

/*!
Creates a matrix from this GQuat
*/
inline void GQuat::GetMatrix(GMatrix4 &dest,
	const GVector3 &center) const
{
	dest(0,0) = 1.0f - 2.0f*m_Y*m_Y - 2.0f*m_Z*m_Z;
	dest(0,1) = 2.0f*m_X*m_Y + 2.0f*m_Z*m_W;
	dest(0,2) = 2.0f*m_X*m_Z - 2.0f*m_Y*m_W;
	dest(0,3) = 0.0f;

	dest(1,0) = 2.0f*m_X*m_Y - 2.0f*m_Z*m_W;
	dest(1,1) = 1.0f - 2.0f*m_X*m_X - 2.0f*m_Z*m_Z;
	dest(1,2) = 2.0f*m_Z*m_Y + 2.0f*m_X*m_W;
	dest(1,3) = 0.0f;

	dest(2,0) = 2.0f*m_X*m_Z + 2.0f*m_Y*m_W;
	dest(2,1) = 2.0f*m_Z*m_Y - 2.0f*m_X*m_W;
	dest(2,2) = 1.0f - 2.0f*m_X*m_X - 2.0f*m_Y*m_Y;
	dest(2,3) = 0.0f;

	dest(3,0) = center._x;
	dest(3,1) = center._y;
	dest(3,2) = center._z;
	dest(3,3) = 1.f;

	//dest.setDefinitelyIdentityMatrix(false);
}


/*!
Creates a matrix from this GQuat
Rotate about a center point
shortcut for
GQuat q;
q.rotationFromTo(vin[i].Normal, forward);
q.getMatrix(lookat, center);

GMatrix4 m2;
m2.setInverseTranslation(center);
lookat *= m2;
*/
inline void GQuat::GetMatrixCenter(GMatrix4 &dest,
	const GVector3 &center,
	const GVector3 &translation) const
{
	dest(0,0) = 1.0f - 2.0f*m_Y*m_Y - 2.0f*m_Z*m_Z;
	dest(0,1) = 2.0f*m_X*m_Y + 2.0f*m_Z*m_W;
	dest(0,2) = 2.0f*m_X*m_Z - 2.0f*m_Y*m_W;
	dest(0,3) = 0.0f;

	dest(1,0) = 2.0f*m_X*m_Y - 2.0f*m_Z*m_W;
	dest(1,1) = 1.0f - 2.0f*m_X*m_X - 2.0f*m_Z*m_Z;
	dest(1,2) = 2.0f*m_Z*m_Y + 2.0f*m_X*m_W;
	dest(1,3) = 0.0f;

	dest(2,0) = 2.0f*m_X*m_Z + 2.0f*m_Y*m_W;
	dest(2,1) = 2.0f*m_Z*m_Y - 2.0f*m_X*m_W;
	dest(2,2) = 1.0f - 2.0f*m_X*m_X - 2.0f*m_Y*m_Y;
	dest(2,3) = 0.0f;

	//dest.setRotationCenter(center, translation);
}

// Creates a matrix from this GQuat
inline void GQuat::GetMatrix_Transposed( GMatrix4 &dest ) const
{
	dest(0,0) = 1.0f - 2.0f*m_Y*m_Y - 2.0f*m_Z*m_Z;
	dest(1,0) = 2.0f*m_X*m_Y + 2.0f*m_Z*m_W;
	dest(2,0) = 2.0f*m_X*m_Z - 2.0f*m_Y*m_W;
	dest(3,0) = 0.0f;

	dest(0,1) = 2.0f*m_X*m_Y - 2.0f*m_Z*m_W;
	dest(1,1) = 1.0f - 2.0f*m_X*m_X - 2.0f*m_Z*m_Z;
	dest(2,1) = 2.0f*m_Z*m_Y + 2.0f*m_X*m_W;
	dest(3,1) = 0.0f;

	dest(0,2) = 2.0f*m_X*m_Z + 2.0f*m_Y*m_W;
	dest(1,2) = 2.0f*m_Z*m_Y - 2.0f*m_X*m_W;
	dest(2,2) = 1.0f - 2.0f*m_X*m_X - 2.0f*m_Y*m_Y;
	dest(3,2) = 0.0f;

	dest(0,3) = 0.f;
	dest(1,3) = 0.f;
	dest(2,3) = 0.f;
	dest(3,3) = 1.f;

	//dest.setDefinitelyIdentityMatrix(false);
}


// Inverts this GQuat
inline GQuat& GQuat::Inverse()
{
	m_X = -m_X; m_Y = -m_Y; m_Z = -m_Z;
	return *this;
}


// sets new GQuat
inline GQuat& GQuat::Set(float x, float y, float z, float w)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
	m_W = w;
	return *this;
}


// sets new GQuat based on euler angles
inline GQuat& GQuat::Set( float x, float y, float z )
{
	float angle;

	angle = x * 0.5f;
	const float sr = sinf(angle);
	const float cr = cosf(angle);

	angle = y * 0.5f;
	const float sp = sinf(angle);
	const float cp = cosf(angle);

	angle = z * 0.5f;
	const float sy = sinf(angle);
	const float cy = cosf(angle);

	const float cpcy = cp * cy;
	const float spcy = sp * cy;
	const float cpsy = cp * sy;
	const float spsy = sp * sy;

	m_X = (float)(sr * cpcy - cr * spsy);
	m_Y = (float)(cr * spcy + sr * cpsy);
	m_Z = (float)(cr * cpsy - sr * spcy);
	m_W = (float)(cr * cpcy + sr * spsy);

	return Normalize();
}

// sets new GQuat based on euler angles
inline GQuat& GQuat::Set( const GVector3& vec )
{
	return Set( vec._x, vec._y, vec._z );
}

// sets new GQuat based on other GQuat
inline GQuat& GQuat::Set(const GQuat& quat)
{
	return (*this = quat);
}

/*
//! returns if this GQuat equals the other one, taking floating point rounding errors into account
inline bool GQuat::Equals(const GQuat& other, const float tolerance) const
{
	return Equals(m_X, other.m_X, tolerance) &&
		Equals(m_Y, other.m_Y, tolerance) &&
		Equals(m_Z, other.m_Z, tolerance) &&
		Equals(m_W, other.m_W, tolerance);
}
*/

// normalizes the GQuat
inline GQuat& GQuat::Normalize()
{
	const float n = m_X*m_X + m_Y*m_Y + m_Z*m_Z + m_W*m_W;

	if (n == 1)
		return *this;

	//n = 1.0f / sqrtf(n);
	return (*this *= 1 / sqrtf(n));
	// in reciprocal_squareroot is some inline asm func.  maybe look into this later for speed.
}


// set this GQuat to the result of the linear interpolation between two GQuats
inline GQuat& GQuat::Lerp( GQuat q1, GQuat q2, float time )
{
	const float scale = 1.0f - time;
	return (*this = (q1*scale) + (q2*time));
}


// set this GQuat to the result of the interpolation between two GQuats
inline GQuat& GQuat::Slerp(GQuat q1, GQuat q2, float time, float threshold)
{
	float angle = q1.DotProduct(q2);

	// make sure we use the short rotation
	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	if (angle <= (1 - threshold)) // spherical interpolation
	{
		const float theta = acosf(angle);
		const float invsintheta = 1.f / (sinf(theta));
		const float scale = sinf(theta * (1.0f - time)) * invsintheta;
		const float invscale = sinf(theta * time) * invsintheta;
		return (*this = (q1*scale) + (q2*invscale));
	}
	else // linear interploation
		return Lerp( q1, q2, time );
}


// calculates the dot product
inline float GQuat::DotProduct(const GQuat& q2) const
{
	return (m_X * q2.m_X) + (m_Y * q2.m_Y) + (m_Z * q2.m_Z) + (m_W * q2.m_W);
}


//! axis must be unit length, angle in radians
inline GQuat& GQuat::FromAngleAxis(float angle, const GVector3& axis)
{
	const float fHalfAngle = 0.5f*angle;
	const float fSin = sinf(fHalfAngle);
	m_W = cosf(fHalfAngle);
	m_X = fSin*axis._x;
	m_Y = fSin*axis._y;
	m_Z = fSin*axis._z;
	return *this;
}


inline void GQuat::ToAngleAxis( float &angle, GVector3 &axis ) const
{
	const float scale = sqrtf( m_X * m_X + m_Y * m_Y + m_Z * m_Z );

	if ( GMath::AlmostEqual(scale, 0.0f) || m_W > 1.0f || m_W < -1.0f)
	{
		angle = 0.0f;
		axis._x = 0.0f;
		axis._y = 1.0f;
		axis._z = 0.0f;
	}
	else
	{
		const float invscale = 1.f / scale;
		angle = 2.0f * acosf(m_W);
		axis._x = m_X * invscale;
		axis._y = m_Y * invscale;
		axis._z = m_Z * invscale;
	}
}

inline void GQuat::ToEuler(GVector3& euler) const
{
	const float sqw = m_W*m_W;
	const float sqx = m_X*m_X;
	const float sqy = m_Y*m_Y;
	const float sqz = m_Z*m_Z;
	const float test = 2.0f * (m_Y*m_W - m_X*m_Z);

	if (GMath::AlmostEqual(test, 1.0f))
	{
		// heading = rotation about z-axis
		euler._z = (float)(-2.0f*atan2f(m_X, m_W));
		// bank = rotation about x-axis
		euler._x = 0;
		// attitude = rotation about y-axis
		euler._y = (float)(GMath::Pi / 2.0f);
	}
	else if( GMath::AlmostEqual( test, -1.0f ) )
	{
		// heading = rotation about z-axis
		euler._z = (float)(2.0*atan2f(m_X, m_W));
		// bank = rotation about x-axis
		euler._x = 0;
		// attitude = rotation about y-axis
		euler._y = (float)(GMath::Pi / -2.0);
	}
	else
	{
		// heading = rotation about z-axis
		euler._z = (float)atan2f(2.0f * (m_X*m_Y + m_Z*m_W), (sqx - sqy - sqz + sqw));
		// bank = rotation about x-axis
		euler._x = (float)atan2f(2.0f * (m_Y*m_Z + m_X*m_W), (-sqx - sqy + sqz + sqw));
		// attitude = rotation about y-axis
		float test2 = test;
		GMath::Clamp( test2, -1.0, 1.0 ); // change clamp later..take in pointer...?
		euler._y = (float) asinf( test2 );
	}
}


inline GVector3 GQuat::operator* (const GVector3& v) const
{
	// nVidia SDK implementation

	GVector3 uv, uuv;
	GVector3 qvec(m_X, m_Y, m_Z);
	uv = qvec.Cross( v );
	uuv = qvec.Cross( uv );
	uv *= (2.0f * m_W);
	uuv *= 2.0f;

	return v + uv + uuv;
}

// set GQuat to identity
inline GQuat& GQuat::Identify()
{
	m_W = 1.f;
	m_X = 0.f;
	m_Y = 0.f;
	m_Z = 0.f;
	return *this;
}

inline GQuat& GQuat::RotationFromTo(const GVector3& from, const GVector3& to)
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	GVector3 v0 = from;
	GVector3 v1 = to;
	v0.Normalize();
	v1.Normalize();

	const float d = v0.Dot( v1 );
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return Identify();
	}
	else if (d <= -1.0f) // exactly opposite
	{
		GVector3 axis(1.0f, 0.f, 0.f);
		axis = axis.Cross( v0 );
		if (axis.LengthSquared() == 0.0f )
		{
			axis.Set(0.f, 1.f, 0.f);
			axis = axis.Cross( v0 );
		}
		// same as fromAngleAxis(PI, axis).normalize();
		return Set(axis._x, axis._y, axis._z, 0).Normalize();
	}

	const float s = sqrtf((1 + d) * 2); // optimize inv_sqrt
	const float invs = 1.f / s;
	const GVector3 c = v0.Cross( v1 )*invs;
	return Set(c._x, c._y, c._z, s * 0.5f ).Normalize();
}

#endif