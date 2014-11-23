#include "GVector3.h"
#include "Utility/Debugging.h"

//Should this be level or zone specific?
#define POOL_SIZE 200

const GVector3 GVector3::Zero(0.0f, 0.0f, 0.0f);
const GVector3 GVector3::One(1.0f, 1.0f, 1.0f); 
const GVector3 GVector3::Up(0.0f, 1.0f, 0.0f); 
const GVector3 GVector3::Down(0.0f, -1.0f, 0.0f);
const GVector3 GVector3::Right(1.0f, 0.0f, 0.0f);
const GVector3 GVector3::Forward(0.0f, 0.0f, 1.0f);

GMemoryPool* GVector3::m_allocator = GMemoryPool::Create( sizeof( GVector3 ), POOL_SIZE);

GVector3 GVector3::FromD3DX( const void* i_vector )
{
	GVector3 vector;
	memcpy( &vector, i_vector, sizeof( float ) * 3 );
	return vector;
}

GVector3 GVector3::Lerp( const GVector3& i_source, const GVector3& i_dest, float i_destPercent )
{
	return i_source * ( 1.0f - i_destPercent ) + i_dest * i_destPercent;
}

#ifdef _DEBUG
void GVector3::Vector3_UnitTest()
{
	const GVector3 constVector(5.0f, 5.0f, 5.0f);

	//Can't do this!
	//constVector.Normalize();
	//Can do this!
	constVector.Normal();

	GVector3 vot;


	//Normalize
	GVector3 v(17.0f, 14.0f, 12.0f);
	v.Normalize();
	DEBUG_PRINT("Normalized: x=%f, y=%f, z=%f \n", v.x(), v.y(), v.z());

	//Multiply by floating point
	v *= 3.0f;
	DEBUG_PRINT("Applied scalar of 3: x=%f, y=%f, z=%f \n", v.x(), v.y(), v.z());

	//Add vectors
	GVector3 plus = v + constVector;
	DEBUG_PRINT("Non-const + const: x=%f, y=%f, z=%f \n", plus.x(), plus.y(), plus.z());
	GVector3 vConst = constVector + constVector;
	DEBUG_PRINT("const + const: x=%f, y=%f, z=%f \n", vConst.x(), vConst.y(), vConst.z());

	//Multiplying vectors by a scalar
	GVector3 multid = GVector3::One * 3;
	GVector3 multidTwo = 3 * GVector3::One;
	GVector3 large = v -constVector * multid + multidTwo;
	GVector3 reflected = 2.0f * constVector * ( multid.Dot( multidTwo) ) - multid;

	//FINDING THE CROSS PRODUCT OF A TRIANGLE!!!
	//Top
	GVector3 v1(0.0f, 1.0f, 0.0f);
	//Bottom left
	GVector3 v2(-0.5f, 0.0f, 0.0f);
	//Bottom right
	GVector3 v3(0.5f, 0.0f, 0.0f);
	//Compose the triangle
	GVector3 a = v3 - v1;
	GVector3 b = v2 - v1;

	//a X b = ...
	GVector3 cp = a.Cross(b);
	DEBUG_PRINT("Triangle face normal: x=%f, y=%f, z=%f \n", cp.x(), cp.y(), cp.z());


	GVector3 v4 = -v3;
	DEBUG_PRINT("Negative vector: x=%f, y=%f, z=%f\n", v4.x(), v4.y(), v4.z());
}
#else
void GVector3::Vector3_UnitTest()
{
	return;
}
#endif


