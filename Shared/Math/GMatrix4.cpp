#include "GMatrix4.h"
#include "d3dx9.h"

GMatrix4::GMatrix4( bool identityHack )
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
	//translation
	m_elements[3][0] = 0.0f;
	m_elements[3][1] = 0.0f;
	m_elements[3][2] = 0.0f;
	m_elements[3][3] = 1.0f;
	//the poop?
	m_elements[0][3] = 0.0f;
	m_elements[0][3] = 0.0f;
	m_elements[0][3] = 0.0f;
	
#if defined(_DEBUG) && defined( TEMP_MATRIX )
	D3DXMatrixIdentity( &m_testMatrix );
#endif
}

void GMatrix4::OrientVector3( GVector3& o_dest, const GVector3& i_src, const GMatrix4& i_transform )
{
	o_dest.x( i_src.x() * i_transform(0,0) + (i_src.y() * i_transform(1,0)) + (i_src.z() * i_transform(2,0)));
	o_dest.y( i_src.x() * i_transform(0,1) + (i_src.y() * i_transform(1,1)) + (i_src.z() * i_transform(2,1)));
	o_dest.z( i_src.x() * i_transform(0,2) + (i_src.y() * i_transform(1,2)) + (i_src.z() * i_transform(2,2)));
}

const GMatrix4& GMatrix4::Identity = GMatrix4( true );


#if defined(_DEBUG) && defined(_WIN32)
void GMatrix4::Debug_PrintD3DMatrixToConsole( void* i_matrix )
{
	D3DMATRIX* mat = (D3DMATRIX*)i_matrix;
	DEBUG_PRINT( "[ %f %f %f %f ]\n", mat->m[0][0], mat->m[0][1], mat->m[0][2], mat->m[0][3] );
	DEBUG_PRINT( "[ %f %f %f %f ]\n", mat->m[1][0], mat->m[1][1], mat->m[1][2], mat->m[1][3] );
	DEBUG_PRINT( "[ %f %f %f %f ]\n", mat->m[2][0], mat->m[2][1], mat->m[2][2], mat->m[2][3] );
	DEBUG_PRINT( "[ %f %f %f %f ]\n", mat->m[3][0], mat->m[3][1], mat->m[3][2], mat->m[3][3] );	
}
#endif 


void GMatrix4::Inverse(GMatrix4& o_dest) const
{
	//float temp;

	//giant hack:
	//o_dest = *this;

	D3DXMATRIX* them = (D3DXMATRIX*)&o_dest;
	D3DXMATRIX* us = (D3DXMATRIX*) this;
	D3DXMatrixInverse(them, NULL, us);

	/*
	//rotation
	temp = m_elements[0][1];
	o_dest.m_elements[0][1] = m_elements[1][0];
	o_dest.m_elements[1][0] = temp;
	o_dest.m_elements[1][1] = m_elements[1][1];
	temp = m_elements[2][0];
	o_dest.m_elements[2][0] = m_elements[0][2];
	o_dest.m_elements[2][2] = m_elements[2][2];
	o_dest.m_elements[0][2] = temp;
	temp = m_elements[2][1];
	o_dest.m_elements[2][1] = m_elements[1][2];
	o_dest.m_elements[1][2] = temp;
	*/
}