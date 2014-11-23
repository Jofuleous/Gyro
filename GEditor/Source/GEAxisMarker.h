#ifndef __GE_AXISMARKER_H_
#define __GE_AXISMARKER_H_

#include "Math/GVector3.h"
#include "Math/GAABB.h"

class Entity;

class GEAxisMarker
{
public:
				GEAxisMarker( )
				{
					m_xBox = GAABB( 112.0f, 4.5f, 4.5f, 0.0f, -4.5f, -4.5f ); 
					m_yBox = GAABB( 4.5f, 112.0f, 4.5f, -4.5f, 0.0f, -4.5f );
					m_zBox = GAABB( 4.5f, 4.5f, 112.0f, -4.5f, -4.5f, 0.0f ); 
					m_selectedAxis = 0;
					m_markerEntity = NULL;
				}

				~GEAxisMarker( );

	// 0 = no intersection, 1 = x-axis, 2 = y-axis, 3 = z-axis
	int			Select( GVector3& i_origin, GVector3& i_dir ); 

	// x and y are screen coords.
	void		HandleMouse( int x, int y, int width, int height );
	void		LoadEntity();
	void		Update();
	void		UnSelect();

	float		m_yOrient;
	GVector3	m_position;

	GAABB		m_xBox;
	GAABB		m_yBox;
	GAABB		m_zBox;


	// movement shit...this is a tough algorithm.
	int			m_selectedAxis;
	GVector3	m_axisStart;
	GVector3	m_axisVector; // eventually, this should just be a component of the parent 3x3 matrix.
	GVector3	m_parentStart; // this also acts as a point on the plane for our calculations...
	GVector3	m_currentPointOnPlane;
	GVector3	m_planeNormal; // eventually, this should just be a component of the parent 3x3 matrix.

	Entity*		m_markerEntity;

};

#endif