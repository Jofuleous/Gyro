#ifndef __GEDITOR_CAMERA_H_
#define __GEDITOR_CAMERA_H_

#include "Camera.h"
#include "RenderManager.h"
#include "Scene.h"

class GEditorCamera
{

public:

	GEditorCamera() : m_altDown( false ), m_MMBDown( false ), m_RMBDown( false ), m_LMBDown( false ),
		m_mouseStartX ( 0 ), m_mouseStartY( 0 ), m_deltaX( 0 ), m_deltaY( 0 ), m_zoomDelta( 0 )
	{
	}
	void	SetAspectRatio( int i_width, int i_height );

	void	Update( );

	bool	m_altDown;	// used for rotating.
	bool	m_MMBDown;	// used for panning.
	bool	m_RMBDown;
	bool	m_LMBDown;			//
	int		m_mouseStartX;
	int		m_mouseStartY;
	int		m_deltaX;
	int		m_deltaY;
	int		m_width;
	int		m_height;
	int		m_zoomDelta;
	int		m_zoomYLast; // it's really bad to do this by pixel.  should be done by float based on aspect ratio...
	float	m_LMBStart;
};

#endif