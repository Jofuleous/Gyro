#include "GAABB.h"

GAABB::GAABB()
{
}

GAABB::GAABB( const GVector3& i_center, float i_halfX, float i_halfY, float i_halfZ ) :
	m_Center( i_center ),
	m_HalfX( i_halfX ),
	m_HalfY( i_halfY ),
	m_HalfZ( i_halfZ )
{
}

GAABB::GAABB( const GVector3& i_Max, const GVector3& i_Min ) :
	m_HalfX( i_Max.x() - ((i_Max.x() + i_Min.x() ) / 2 )), // i believe these don't need to be fabs'd
	m_HalfY( i_Max.y() - ((i_Max.y() + i_Min.y() ) / 2 )),
	m_HalfZ( i_Max.z() - ((i_Max.z() + i_Min.z() ) / 2 )),
	m_Center( (i_Max.x() + i_Min.x()) / 2, (i_Max.y() + i_Min.y()) / 2, (i_Max.z() + i_Min.z()) / 2 )
{
}

GAABB::GAABB( float i_xMax, float i_yMax, float i_zMax, float i_xMin, float i_yMin, float i_zMin ) : 
	m_HalfX( i_xMax - ((i_xMax + i_xMin) / 2) ),
	m_HalfY( i_yMax - ((i_yMax + i_yMin) / 2) ),
	m_HalfZ( i_zMax - ((i_zMax + i_zMin) / 2) ),
	m_Center( (i_xMax + i_xMin) / 2, (i_yMax + i_yMin) / 2, (i_zMax + i_zMin) / 2)
{
}

bool GAABB::RayIntersects( GVector3& i_origin, GVector3& i_dir, float i_length )
{
	i_length += 0.001f;
	GVector3 p1 = i_origin + ( i_dir * i_length );
    GVector3 m = (i_origin + p1) * 0.5f;			// Segment midpoint
    GVector3 d = p1 - m;							// Segment halflength vector
    m = m - m_Center;								// Translate box and segment to origin
    
    // Try world coordinate axes as separating axes
    float adx = fabsf(d._x);
    if (fabsf(m._x) > m_HalfX + adx)
		return 0;
    float ady = fabsf(d._y);
    if (fabsf(m._y) > m_HalfY + ady)
		return 0;
    float adz = fabsf(d._z);
    if (fabsf(m._z) > m_HalfZ + adz)
		return 0;

    // Add in an epsilon term to counteract arithmetic errors when segment is
    // (near) parallel to a coordinate axis
    adx += 0.001f;
    ady += 0.001f;
    adz += 0.001f;

    // Try cross products of segment direction vector with coordinate axes
    if (fabsf(m._y * d._z - m._z * d._y) > m_HalfY * adz + m_HalfZ * ady) // was i_extentLength
		return 0;
    if (fabsf(m._z * d._x - m._x * d._z) > m_HalfX * adz + m_HalfZ * adx)
		return 0;
    if (fabsf(m._x * d._y - m._y * d._x) > m_HalfX * ady + m_HalfY * adx)
		return 0;
}

bool GAABB::RayIntersects( GVector3& i_origin, GVector3& i_dir, GMatrix4& i_parent, float i_length )
{
	GVector3 dir;
	GVector3 origin;
	GMatrix4 inverse;
	i_parent.Inverse( inverse );
	GMatrix4::OrientVector3( dir, i_dir, inverse );
	origin = inverse * i_origin;

	return RayIntersects( origin, dir, i_length );
}

bool GAABB::RayIntersectsDistance( GVector3& i_origin, GVector3& i_dir, float& o_distance )
{
	float tmin = ((m_Center._x - m_HalfX) - i_origin._x) / i_dir._x;
	float tmax = ((m_Center._x + m_HalfX) - i_origin._x) / i_dir._x;

	// swap
	if( tmax < tmin )
	{
		float temp = tmin;
		tmin = tmax;
		tmax = temp;
	}

	float miny = ((m_Center._y - m_HalfY) - i_origin._y) / i_dir._y;
	float maxy = ((m_Center._y + m_HalfY) - i_origin._y) / i_dir._y;

	// swap
	if( maxy < miny )
	{
		float temp = miny;
		miny = maxy;
		maxy = temp;
	}

	// return false if the axis enter and exit times do not overlap.
	if( tmin > maxy || miny > tmax )
		return false;
	if( miny > tmin )
		tmin = miny;
	if( maxy < tmax )
		tmax = maxy;

	float minz = ((m_Center._z - m_HalfZ) - i_origin._z) / i_dir._z;
	float maxz = ((m_Center._z + m_HalfZ) - i_origin._z) / i_dir._z;

	// swap
	if( maxz < minz )
	{
		float temp = minz;
		minz = maxz;
		maxz = temp;
	}

	if( tmin > maxz || minz > tmax )
		return false;
	if( minz > tmin )
		tmin = minz;
	//if( maxz < tmax )
	//	tmax = maxz;

	//if( tmin > i_rayLength )
	//	return false;

	o_distance = tmin;

	return true;
}

bool GAABB::RayIntersectsDistance( GVector3& i_origin, GVector3& i_dir, GMatrix4& i_parent, float& o_length )
{
	GVector3 dir;
	GVector3 origin;
	GMatrix4 inverse;
	i_parent.Inverse( inverse );
	GMatrix4::OrientVector3( dir, i_dir, inverse );
	GVector3 otherDir = inverse * (i_origin + i_dir);
	origin = inverse * i_origin;
	otherDir = otherDir - origin;
	//GVector3 otherDir;

	bool intersect = RayIntersectsDistance( origin, otherDir, o_length );

	// this is costly...but get the point we hit...=/
	if( intersect )
	{
		origin = origin + otherDir * o_length;
		origin = i_parent * origin;// this is now the point of intersection...
		o_length = (i_origin - origin).Length();
		return true;
	}

	return false;
}

bool GAABB::PointInside( GVector3& i_point )
{
	if( (i_point._x < m_Center._x + m_HalfX) && (i_point._x > m_Center._x - m_HalfX ) )
		if( (i_point._y < m_Center._y + m_HalfY) && (i_point._y > m_Center._y - m_HalfY ) )
			if( (i_point._y < m_Center._y + m_HalfY) && (i_point._y > m_Center._y - m_HalfY ) )
				return true;

	return false;
}

bool GAABB::PointInside( GVector3& i_point, GMatrix4& i_parent )
{
	GMatrix4 inverse;
	i_parent.Inverse( inverse );
	GVector3 pointInLocal = inverse * i_point;
	return PointInside( pointInLocal );
}