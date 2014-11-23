#include <d3d9.h>
#include <D3DX9Shader.h>

class PointLight
{
public:
	PointLight() {}
	PointLight( D3DXVECTOR3 i_position, D3DXCOLOR i_color, float i_intensity ) : m_position( i_position ), m_color( i_color ), m_intensity( i_intensity )
	{
	}

	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_color;
	float m_intensity;
	float m_radius;
	const char*	m_lightName;
};