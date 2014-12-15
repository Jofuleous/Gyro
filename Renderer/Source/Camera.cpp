#include "Camera.h"

#include "cTime.h"
//#include "../UserInput.h"
//#include "../UserSettings.h"
#include "MainWindow.h"


Camera::Camera( void ) :
	m_position( 0.0f, 0.0f, -10.f ),
	m_lookAtPoint( 0.0f, 0.0f, 0.0f ),
	m_up( 0.0f, 1.0f, 0.0f ),
	m_FOV( D3DXToRadian(45.0f) ),
	m_aspectRatio( ((float)g_MainWindow.GetWidth()) / ((float)g_MainWindow.GetHeight()) ),
	m_nearPlane( 1.0f ),
	m_farPlane( 3000.0f ), // increase in this is probably going to fuck up shadow maps...
	m_yRotation( 0.0f )
{
}

	// sx, sy = -1 to 1
void Camera::ScreenToWorld( float i_sx, float i_sy, float& i_x, float& i_y, float& i_z )
{
	D3DXMATRIX projection = ProjectionTransform();
	D3DXMATRIX inverseViewMatrix;
	D3DXMatrixInverse( &inverseViewMatrix, NULL, &ViewTransform() );

	D3DXVECTOR3 click( i_sx / projection._11, i_sy / projection._22, 1.0f );

	D3DXVECTOR3 dir;
	dir.x = (click.x * inverseViewMatrix._11) + (click.y * inverseViewMatrix._21) + inverseViewMatrix._31;
	dir.y = (click.x * inverseViewMatrix._12) + (click.y * inverseViewMatrix._22) + inverseViewMatrix._32;
	dir.z = (click.x * inverseViewMatrix._13) + (click.y * inverseViewMatrix._23) + inverseViewMatrix._33;

	D3DXVec3Normalize(&dir, &dir);

	i_x = dir.x;
	i_y = dir.y;
	i_z = dir.z;
}

void Camera::Update( float i_dt )
{
	/*
	D3DXVECTOR3 offset( 0.0f, 0.0f, 0.0f );
	float yRot = 0.0f;

	if( cs6963::UserInput::IsKeyPressed('W') )
		offset.z = 1.0f;
	if( cs6963::UserInput::IsKeyPressed('A') )
	{
		yRot = -1.0f;
	}
	if( cs6963::UserInput::IsKeyPressed('S') )
		offset.z = -1.0f;
	if( cs6963::UserInput::IsKeyPressed('D') )
	{
		yRot = 1.0f;
		//offset.x = 1.0f;
	}
	if( cs6963::UserInput::IsKeyPressed('R') )
		offset.y = 1.0f;
	if( cs6963::UserInput::IsKeyPressed('F') )
		offset.y = -1.0f;

	m_yRotation += ( yRot * i_dt );
	offset *= (15.0f * i_dt) ; // 15 units/second

	D3DXMATRIX lookAtMatrixY;
	D3DXMatrixRotationY( &lookAtMatrixY, m_yRotation );
	D3DXVec3TransformNormal( &offset, &offset, &lookAtMatrixY );


	Translate( offset );
	*/
}

void Camera::InitializeFreeCam( )
{
	// look straight ahead...
	m_lookAtPoint.x = (m_lookAtPoint.x - m_position.x);
	m_lookAtPoint.z = (m_lookAtPoint.z - m_position.z );
	m_lookAtPoint.y = m_position.y;
	D3DXVec3Normalize( &m_lookAtPoint, &m_lookAtPoint );
	m_yRotation = atan2( m_lookAtPoint.x, m_lookAtPoint.z );
	D3DXVec3Add( &m_lookAtPoint, &m_lookAtPoint, &m_position );

}

void Camera::HandleInput( float i_x, float i_y, float i_z, float i_dt )
{
	// this is crap, but whatever...it's all for debug...just get the grade and go to bed...
	m_yRotation += (i_dt * (3.14159f * 1.5f ) * -i_x );
	float z = sin( m_yRotation );
	float x = cos( m_yRotation );
	D3DXVECTOR3 facing( x, 0.0f, z );
	D3DXVECTOR3 velocity( facing );
	D3DXVec3Scale( &velocity, &velocity, i_dt * 500.0f * i_z ); // move 500 units in one second?
	D3DXVECTOR3 velocityY( 0.0f, i_y * i_dt * 500.0f, 0.0f);
	D3DXVec3Add( &velocity, &velocity, &velocityY ); 
	D3DXVec3Add( &m_position, &velocity, &m_position );
	D3DXVec3Add( &m_lookAtPoint, &facing, &m_position );
}

D3DXMATRIX Camera::ViewTransform( void )
{
	D3DXMATRIX viewTransform;
	D3DXMATRIX lookAtMatrixY;
	//D3DXMatrixRotationY( &lookAtMatrixY, m_yRotation );
	//D3DXVECTOR3 lookAtVector( 0.0f, 0.0f, 1.0f );
	//D3DXVec3TransformNormal( &lookAtVector, &lookAtVector, &lookAtMatrixY );
	//lookAtVector *= 2;
	//D3DXVec3Add( &m_lookAtPoint, &m_position, &lookAtVector );
	D3DXMatrixLookAtLH( &viewTransform, &m_position, &m_lookAtPoint, &m_up );
	
	return viewTransform;
}

D3DXMATRIX Camera::OrthoProjection( void )
{
	D3DXMATRIX projectionTransform;
	D3DXMatrixOrthoLH( &projectionTransform, g_MainWindow.GetWidth(), g_MainWindow.GetHeight(), m_nearPlane, m_farPlane );

	return projectionTransform;
}

D3DXMATRIX Camera::ProjectionTransform( void ) const
{
	D3DXMATRIX projectionTransform;
	D3DXMatrixPerspectiveFovLH( &projectionTransform, m_FOV, m_aspectRatio, m_nearPlane, m_farPlane );

	return projectionTransform;
}

void Camera::Position( const D3DXVECTOR3& i_position )
{
	m_position = i_position;
}

void Camera::Translate( const D3DXVECTOR3& i_offset )
{
	m_position += i_offset;
	m_lookAtPoint += i_offset;
}

void Camera::LookAtPoint( const D3DXVECTOR3& i_lookAtPoint )
{
	m_lookAtPoint = i_lookAtPoint;
}