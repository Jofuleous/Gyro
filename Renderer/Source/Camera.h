#ifndef __H_CAMERA_
#define __H_CAMERA_

//#include "../Initialization.h"

#include <d3d9.h>
#include <D3DX9Shader.h>

class Camera
{
public:
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAtPoint;
	D3DXVECTOR3 m_up;
	D3DXVECTOR3 m_lookAtMatrixY;

	float m_FOV;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;
	float m_yRotation;

	Camera( );

	void ScreenToWorld( float i_sx, float i_sy, float& i_x, float& i_y, float& i_z );
	void Update( float i_dt );
	void InitializeFreeCam( );
	void HandleInput( float i_x, float i_y, float i_z, float i_dt );
	void Position( const D3DXVECTOR3& i_position );
	void Translate( const D3DXVECTOR3& i_offset );
	void AppendOrientationY( float i_radians );
	void LookAtPoint( const D3DXVECTOR3& i_lookAtPoint );
	float FarPlaneDistance() { return m_farPlane;}

	D3DXMATRIX ViewTransform( );
	D3DXMATRIX OrthoProjection( );
	D3DXMATRIX ProjectionTransform( ) const;
};

#endif