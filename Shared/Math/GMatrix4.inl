#include "GMatrix4.h"
#include "GVector3.h"
#include <math.h>
#include "Utility/Debugging.h"
#include "Utilities.h"
#include <assert.h>

void GMatrix4::Concatenate( const GMatrix4& i_other )
{
	GMatrix4 result = GMatrix4::Identity;

	for( int i = 0; i < 4; i++ )
	{

		result.m_elements[0][i] = m_elements[0][0] * i_other(0,i) + m_elements[0][1]*i_other(1,i) + m_elements[0][2]*i_other(2, i) + m_elements[0][3] * i_other(3,i);
		result.m_elements[1][i] = m_elements[1][0] * i_other(0,i) + m_elements[1][1]*i_other(1,i) + m_elements[1][2]*i_other(2, i) + m_elements[1][3] * i_other(3,i);
		result.m_elements[2][i] = m_elements[2][0] * i_other(0,i) + m_elements[2][1]*i_other(1,i) + m_elements[2][2]*i_other(2, i) + m_elements[2][3] * i_other(3,i);
		result.m_elements[3][i] = m_elements[3][0] * i_other(0,i) + m_elements[3][1]*i_other(1,i) + m_elements[3][2]*i_other(2, i) + m_elements[3][3] * i_other(3,i);

		/*
		result.m_elements[0][i] = m_elements[0][i] * i_other(0,0) + m_elements[1][i]*i_other(0,1) + m_elements[2][i]*i_other(0, 2) + m_elements[3][i] * i_other(0,3);
		result.m_elements[1][i] = m_elements[0][i] * i_other(1,0) + m_elements[1][i]*i_other(1,1) + m_elements[2][i]*i_other(1, 2) + m_elements[3][i] * i_other(1,3);
		result.m_elements[2][i] = m_elements[0][i] * i_other(2,0) + m_elements[1][i]*i_other(2,1) + m_elements[2][i]*i_other(2, 2) + m_elements[3][i] * i_other(2,3);
		result.m_elements[3][i] = m_elements[0][i] * i_other(3,0) + m_elements[1][i]*i_other(3,1) + m_elements[2][i]*i_other(3, 2) + m_elements[3][i] * i_other(3,3);
		*/
	}


#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	result.m_testMatrix = m_testMatrix * i_other.m_testMatrix;
	result.ConfirmEqual();
#endif

	*this = result;
}

void GMatrix4::PokeTranslation( float i_x, float i_y, float i_z )
{
	m_elements[3][0] = i_x; // x
	m_elements[3][1] = i_y; // y
	m_elements[3][2] = i_z; // z
	m_elements[3][3] = 1.0f; // w

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMATRIX temp;
	D3DXMatrixTranslation( &temp, i_x, i_y, i_z );
	m_testMatrix = m_testMatrix * temp;
	ConfirmEqual();
#endif

}

void GMatrix4::PokeTranslation( const GVector3& i_other )
{
	m_elements[3][0] = i_other.x(); // x
	m_elements[3][1] = i_other.y(); // y
	m_elements[3][2] = i_other.z(); // z
	m_elements[3][3] = 1.0f; // w


#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMATRIX temp;
	D3DXMatrixTranslation( &temp, i_other.x(), i_other.y(), i_other.z() );
	m_testMatrix = m_testMatrix * temp;
	ConfirmEqual();
#endif
}

void GMatrix4::SetTranslation( float i_x, float i_y, float i_z )
{
	//rots
	m_elements[0][0] = 1.0f;
	m_elements[0][1] = 0.0f;
	m_elements[1][0] = 0.0f;
	m_elements[1][1] = 1.0f;
	m_elements[2][0] = 0.0f;
	m_elements[0][2] = 0.0f;
	m_elements[2][1] = 0.0f;
	m_elements[1][2] = 0.0f;
	m_elements[2][2] = 1.0f;

	m_elements[3][0] = i_x; // x
	m_elements[3][1] = i_y; // y
	m_elements[3][2] = i_z; // z
	m_elements[3][3] = 1.0f; // w

	//the poop?
	m_elements[0][3] = 0.0f;
	m_elements[1][3] = 0.0f;
	m_elements[2][3] = 0.0f;

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMatrixTranslation( &m_testMatrix, i_x, i_y, i_z );
	ConfirmEqual();
#endif
}

void GMatrix4::SetTranslation( const GVector3& i_other )
{
	//rots
	m_elements[0][0] = 1.0f;
	m_elements[0][1] = 0.0f;
	m_elements[1][0] = 0.0f;
	m_elements[1][1] = 1.0f;
	m_elements[2][0] = 0.0f;
	m_elements[0][2] = 0.0f;
	m_elements[2][1] = 0.0f;
	m_elements[1][2] = 0.0f;
	m_elements[2][2] = 1.0f;

	m_elements[3][0] = i_other.x(); // x
	m_elements[3][1] = i_other.y(); // y
	m_elements[3][2] = i_other.z(); // z
	m_elements[3][3] = 1.0f; // w

	//the poop?
	m_elements[0][3] = 0.0f;
	m_elements[1][3] = 0.0f;
	m_elements[2][3] = 0.0f;


#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMatrixTranslation( &m_testMatrix, i_other.x(), i_other.y(), i_other.z() );
	ConfirmEqual();
#endif
}

void GMatrix4::PokeScale( const GVector3& i_scale )
{
	m_elements[0][0] = i_scale.x(); // x
	m_elements[1][1] = i_scale.y(); // y
	m_elements[2][2] = i_scale.z(); // z

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMATRIX temp;
	//D3DXMatrixScale( &temp, i_scale.x(), i_other.y(), i_other.z() );
	//m_testMatrix = m_testMatrix * temp;
	//ConfirmEqual();
#endif
}

void GMatrix4::PokeScale( float i_x, float i_y, float i_z )
{
	m_elements[0][0] = i_x; // x
	m_elements[1][1] = i_y; // y
	m_elements[2][2] = i_z; // z

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMATRIX temp;
	//D3DXMatrixScale( &temp, i_scale.x(), i_other.y(), i_other.z() );
	//m_testMatrix = m_testMatrix * temp;
	//ConfirmEqual();
#endif
}

void GMatrix4::SetScale( const GVector3& i_scale )
{
	m_elements[0][0] = i_scale.x(); // sx
	m_elements[1][1] = i_scale.y(); // sy
	m_elements[2][2] = i_scale.z(); // sz

	m_elements[0][1] = 0.0f;
	m_elements[1][0] = 0.0f;
	m_elements[2][0] = 0.0f;
	m_elements[0][2] = 0.0f;
	m_elements[2][1] = 0.0f;
	m_elements[1][2] = 0.0f;

	m_elements[3][0] = 0.0f; // x
	m_elements[3][1] = 0.0f; // y
	m_elements[3][2] = 0.0f; // z
	m_elements[3][3] = 1.0f; // w

	//the poop?
	m_elements[0][3] = 0.0f;
	m_elements[1][3] = 0.0f;
	m_elements[2][3] = 0.0f;

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMATRIX temp;
	//D3DXMatrixScale( &temp, i_scale.x(), i_other.y(), i_other.z() );
	//m_testMatrix = m_testMatrix * temp;
	//ConfirmEqual();
#endif
}


void GMatrix4::Identify( void )
{
	*this = Identity;
}

void GMatrix4::ToD3DX( void* o_d3dMatrix )
{
	memcpy(o_d3dMatrix, this, sizeof(GMatrix4));
}

/***********************
   About X-rotation
1     0     0     0  
0    cos   sin    0
0   -sin   cos    0
0     0     0     1
**********************/
void GMatrix4::SetRotX( float i_radians )
{
	m_elements[1][1] = cos( i_radians );
	m_elements[2][2] = cos( i_radians );

	m_elements[1][2] = sin( i_radians );
	m_elements[2][1] = -sin( i_radians );

	//the poop
	m_elements[0][0] = 1;
	m_elements[0][1] = 0;
	m_elements[0][2] = 0;

	m_elements[1][0] = 0;
	m_elements[1][3] = 0;
	m_elements[2][0] = 0;
	m_elements[2][3] = 0;

	m_elements[3][0] = 0;
	m_elements[3][1] = 0;
	m_elements[3][2] = 0;
	m_elements[3][3] = 1;

   
#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMatrixRotationX( &m_testMatrix, i_radians );
   	ConfirmEqual();
#endif
}

/***********************
   About Y-rotation
cos   0   -sin    0
0     1     0     0
sin   0    cos    0
0     0     0     1
**********************/

void GMatrix4::SetRotY( float i_radians )
{
	m_elements[0][0] = cos( i_radians );
	m_elements[2][2] = cos( i_radians );

	m_elements[0][2] = -sin( i_radians );
	m_elements[2][0] = sin( i_radians );

	//more poop
	m_elements[0][1] = 0.0f;
	m_elements[1][0] = 0.0f;
	m_elements[1][1] = 1.0f;
	m_elements[1][2] = 0.0f;
	m_elements[2][1] = 0.0f;

	m_elements[3][0] = 0.0f;
	m_elements[3][1] = 0.0f;
	m_elements[3][2] = 0.0f;
	m_elements[3][3] = 1.0f;
   
	m_elements[0][3] = 0.0f;
	m_elements[1][3] = 0.0f;
    m_elements[2][3] = 0.0f;
	m_elements[3][3] = 1.0f;

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMatrixRotationY( &m_testMatrix, i_radians );
	ConfirmEqual();
#endif
}

/***********************
   About Z-rotation
cos  sin    0     0
-sin cos    0     0
0     0     1     0
0     0     0     1
**********************/
void GMatrix4::SetRotZ( float i_radians )
{
	m_elements[0][0] = cos( i_radians );
	m_elements[1][1] = cos( i_radians );

	m_elements[0][1] = sin( i_radians );
	m_elements[1][0] = -sin( i_radians );

	//the poop.  i guess let's just clear.
	m_elements[0][2] = 0.0f;
	m_elements[0][3] = 0.0f;
	m_elements[1][2] = 0.0f;
	m_elements[1][3] = 0.0f;

	m_elements[2][0] = 0.0f;
	m_elements[2][1] = 0.0f;
	m_elements[2][2] = 1.0f;
	m_elements[2][3] = 0.0f;

	m_elements[3][0] = 0.0f;
	m_elements[3][1] = 0.0f;
	m_elements[3][2] = 0.0f;
	m_elements[3][3] = 1.0f;
   
#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMatrixRotationZ( &m_testMatrix, i_radians );
	ConfirmEqual();
#endif
}

void GMatrix4::Invert( void )
{
	float temp;

	//rotation
	temp = m_elements[0][1];
	m_elements[0][1] = m_elements[1][0];
	m_elements[1][0] = temp;
	temp = m_elements[2][0];
	m_elements[2][0] = m_elements[0][2];
	m_elements[0][2] = temp;
	temp = m_elements[2][1];
	m_elements[2][1] = m_elements[1][2];
	m_elements[1][2] = temp;

	//translation
	float t0 = (-m_elements[0][0] * m_elements[3][0]) + (-m_elements[1][0] * m_elements[3][1]) + (-m_elements[2][0] * m_elements[3][2]); //forget w for now..
	float t1 = (-m_elements[0][1] * m_elements[3][0]) + (-m_elements[1][1] * m_elements[3][1]) + (-m_elements[2][1] * m_elements[3][2]); //forget w for now..
	float t2 = (-m_elements[0][2] * m_elements[3][0]) + (-m_elements[1][2] * m_elements[3][1]) + (-m_elements[2][2] * m_elements[3][2]); //forget w for now..
	//float t3 = (-m_elements[0][3] * m_elements[3][0]) + (-m_elements[1][3] * m_elements[3][1]) + (-m_elements[2][3] * m_elements[3][2]); //forget w for now..

	m_elements[3][0] = t0;
	m_elements[3][1] = t1;
	m_elements[3][2] = t2;
	m_elements[3][3] = 1.0f;

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXMatrixInverse( &m_testMatrix, NULL, &m_testMatrix );
	//ConfirmEqual();
#endif
}

float GMatrix4::operator() (unsigned i_row, unsigned i_col ) const
{
	return m_elements[i_row][i_col];
}

float& GMatrix4::operator() (unsigned i_row, unsigned i_col)
{
	return m_elements[i_row][i_col];
}

inline GVector3 operator*( const GMatrix4& i_mOne, const GVector3& i_vector)
{
	GVector3 ret;
	ret.x( (i_vector.x() * i_mOne(0,0)) + (i_vector.y() * i_mOne(1,0)) + (i_vector.z() * i_mOne(2,0)) + i_mOne(3, 0));
	ret.y( (i_vector.x() * i_mOne(0,1)) + (i_vector.y() * i_mOne(1,1)) + (i_vector.z() * i_mOne(2,1)) + i_mOne(3, 1));
	ret.z( (i_vector.x() * i_mOne(0,2)) + (i_vector.y() * i_mOne(1,2)) + (i_vector.z() * i_mOne(2,2)) + i_mOne(3, 2));

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXVECTOR3 tempVector3( i_vector.x(), i_vector.y(), i_vector.z() );
	D3DXVECTOR4 result4;
	D3DXVec3Transform( &result4, &tempVector3, &i_mOne.m_testMatrix );
	//assert( (result4.x == ret.x()) && (result4.y == ret.y()) && (result4.z == ret.z())); 
#endif

	return ret;
}

inline GVector3 operator*( const GVector3& i_vector, const GMatrix4& i_mOne )
{

	GVector3 ret;
	ret.x( (i_vector.x() * i_mOne(0,0)) + (i_vector.y() * i_mOne(1,0)) + (i_vector.z() * i_mOne(2,0)) + i_mOne(3, 0));
	ret.y( (i_vector.x() * i_mOne(0,1)) + (i_vector.y() * i_mOne(1,1)) + (i_vector.z() * i_mOne(2,1)) + i_mOne(3, 1));
	ret.z( (i_vector.x() * i_mOne(0,2)) + (i_vector.y() * i_mOne(1,2)) + (i_vector.z() * i_mOne(2,2)) + i_mOne(3, 2));

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	D3DXVECTOR3 tempVector3( i_vector.x(), i_vector.y(), i_vector.z() );
	D3DXVECTOR4 result4;
	D3DXVec3Transform( &result4, &tempVector3, &i_mOne.m_testMatrix );
	assert( (result4.x == ret.x()) && (result4.y == ret.y()) && (result4.z == ret.z())); 
#endif

	return ret;
}


inline GMatrix4 operator*( const GMatrix4& i_mOne, const GMatrix4& i_mTwo)
{
	GMatrix4 result = GMatrix4::Identity;

	for( int i = 0; i < 4; i++ )
	{

		result.m_elements[0][i] = i_mOne.m_elements[0][0] * i_mTwo.m_elements[0][i] + i_mOne.m_elements[0][1]*i_mTwo.m_elements[1][i] + i_mOne.m_elements[0][2]*i_mTwo.m_elements[2][i] + i_mOne.m_elements[0][3] * i_mTwo.m_elements[3][i];
		result.m_elements[1][i] = i_mOne.m_elements[1][0] * i_mTwo.m_elements[0][i] + i_mOne.m_elements[1][1]*i_mTwo.m_elements[1][i] + i_mOne.m_elements[1][2]*i_mTwo.m_elements[2][i] + i_mOne.m_elements[1][3] * i_mTwo.m_elements[3][i];
		result.m_elements[2][i] = i_mOne.m_elements[2][0] * i_mTwo.m_elements[0][i] + i_mOne.m_elements[2][1]*i_mTwo.m_elements[1][i] + i_mOne.m_elements[2][2]*i_mTwo.m_elements[2][i] + i_mOne.m_elements[2][3] * i_mTwo.m_elements[3][i];
		result.m_elements[3][i] = i_mOne.m_elements[3][0] * i_mTwo.m_elements[0][i] + i_mOne.m_elements[3][1]*i_mTwo.m_elements[1][i] + i_mOne.m_elements[3][2]*i_mTwo.m_elements[2][i] + i_mOne.m_elements[3][3] * i_mTwo.m_elements[3][i];
	}

#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
	result.m_testMatrix = i_mOne.m_testMatrix * i_mTwo.m_testMatrix;
	result.ConfirmEqual();
#endif

	return result;
}

#ifdef _DEBUG
inline void GMatrix4::Debug_PrintMatrixToConsole( void )
{
	DEBUG_PRINT( "[ %f %f %f %f ]\n", m_elements[0][0], m_elements[0][1], m_elements[0][2], m_elements[0][3] );
	DEBUG_PRINT( "[ %f %f %f %f ]\n", m_elements[1][0], m_elements[1][1], m_elements[1][2], m_elements[1][3] );
	DEBUG_PRINT( "[ %f %f %f %f ]\n", m_elements[2][0], m_elements[2][1], m_elements[2][2], m_elements[2][3] );
	DEBUG_PRINT( "[ %f %f %f %f ]\n", m_elements[3][0], m_elements[3][1], m_elements[3][2], m_elements[3][3] );
}
#endif


#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
bool GMatrix4::ConfirmEqual( )
{
	//Debug_PrintMatrixToConsole();
	//Debug_PrintD3DMatrixToConsole( m_testMatrix );

	/*
	if( GMath::GoodEnough(m_testMatrix.m[0][0] , m_elements[0][0]  ) && GMath::GoodEnough( m_testMatrix.m[0][1] , m_elements[0][1]  ) && GMath::GoodEnough( m_testMatrix.m[0][2] , m_elements[0][2]  ) && GMath::GoodEnough( m_testMatrix.m[0][3] , m_elements[0][3]  ) )
	{
		if( GMath::GoodEnough( m_testMatrix.m[1][0] , m_elements[1][0]  ) && GMath::GoodEnough( m_testMatrix.m[1][1] , m_elements[1][1]  ) && GMath::GoodEnough( m_testMatrix.m[1][2] , m_elements[1][2]  ) && GMath::GoodEnough( m_testMatrix.m[1][3] , m_elements[1][3]  ) )
		{
			if( GMath::GoodEnough( m_testMatrix.m[2][0] , m_elements[2][0] ) && GMath::GoodEnough( m_testMatrix.m[2][1] , m_elements[2][1] ) && GMath::GoodEnough( m_testMatrix.m[2][2] , m_elements[2][2]  ) && GMath::GoodEnough( m_testMatrix.m[2][3] , m_elements[2][3]  ) )
			{
				if( GMath::GoodEnough( m_testMatrix.m[3][0] , m_elements[3][0]  ) && GMath::GoodEnough( m_testMatrix.m[3][1] , m_elements[3][1]  ) && GMath::GoodEnough( m_testMatrix.m[3][2] , m_elements[3][2]  ) && GMath::GoodEnough( m_testMatrix.m[3][3] , m_elements[3][3]  ) )
				{
					return true;
				}
			}
		}
	}
	*/
	
	//assert( 0 );
	return true;
}
#endif

GVector3 GMatrix4::GetTranslation( void ) const
{
	GVector3 ret( m_elements[3][0], m_elements[3][1], m_elements[3][2] );
	return ret;
}