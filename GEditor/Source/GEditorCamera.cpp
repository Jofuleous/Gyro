#include "GEditorCamera.h"

void GEditorCamera::SetAspectRatio( int i_width, int i_height )
{
	m_width = i_width;
	m_height = i_height;
	// don't worry about tearing when resizing.
	Camera* camera = g_RenderManager.GetCurrentScene()->GetCamera();
	if( camera )
		camera->m_aspectRatio = (float)i_width / (float) i_height;
}

void GEditorCamera::Update( )
{
	Camera* camera = g_RenderManager.GetCurrentScene()->GetCamera();
	if( camera )
	{
		// create the toal delta vector:
		float pitchRads = (m_deltaY / (float) m_width / 2.0f) * ( (3.14159f));
		float yawRads = -(m_deltaX / (float) m_height / 2.0f) * ( (3.14159f) ); /* ( (3.14159f) */;

		float x = cos( yawRads )*cos( pitchRads );
		float y = sin( pitchRads);
		float z = sin( yawRads )*cos( pitchRads );

		D3DXVECTOR3 dir( -x, -y, -z );
		D3DXVec3Add( &camera->m_position, &camera->m_position, &(m_zoomDelta * dir) );

		D3DXVECTOR3 pos;
		D3DXVec3Add( &pos, &camera->m_position, &dir );
		camera->m_lookAtPoint = pos;

		m_zoomDelta = 0;
	}
}