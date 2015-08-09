#include "GDebugDraw.h"

#include "../../../Renderer/Source/RenderManager.h"
#include "Misc/UserSettings.h"
#include <d3dx9math.h>

static D3DXCOLOR ConvertHexToD3DXColor( uint32_t i_iColor )
{
	D3DXCOLOR color( static_cast<float>( (i_iColor & 0xFF000000) >> 24 ) / 255.f, static_cast<float>( (i_iColor & 0x00FF0000) >> 16 ) / 255.f, static_cast<float>( (i_iColor & 0x0000FF00) >> 8 ) / 255.f,
		static_cast<float>( i_iColor & 0x000000FF ) / 255.f );

	return color;
}

void GDebugDraw::DrawLine( const GVector3& i_vStart, const GVector3& i_vEnd, float i_fStartRadius, float i_fEndRadius, uint32_t i_iColor )
{
	// Convert vectors to D3D vectors...
	D3DXVECTOR3	vStart( i_vStart.x( ), i_vStart.y( ), i_vStart.z( ) );
	D3DXVECTOR3 vEnd( i_vEnd.x( ), i_vEnd.y( ), i_vEnd.z( ) );

	// Convert hex color to D3DCOLOR
	D3DXCOLOR color = ConvertHexToD3DXColor( i_iColor );

	float fAspectRatio = static_cast<float>( Gyro::UserSettings::GetScreenWidth( ) ) / static_cast<float>( Gyro::UserSettings::GetScreenHeight( ) );

	g_RenderManager.m_lines.AddLine( vStart, vEnd, i_fStartRadius, i_fEndRadius, fAspectRatio, color );
}

void GDebugDraw::DrawHudBox( float i_fTopLeftXPerc, float i_fTopLeftYPerc, float i_fWidthPerc, float i_fHeightPerc, uint32_t i_iColor )
{
	g_RenderManager.m_hud.AddHUDBox( i_fTopLeftXPerc, i_fTopLeftYPerc, i_fWidthPerc, i_fHeightPerc, ConvertHexToD3DXColor( i_iColor ) );
}

void GDebugDraw::DrawHudText( int32_t i_iLeft, int32_t i_iTop, int32_t i_iRight, int32_t i_iBottom, uint32_t i_iColor, const char* i_cText )
{
	g_RenderManager.m_hud.AddHUDText( i_iLeft, i_iTop, i_iRight, i_iBottom, ConvertHexToD3DXColor( i_iColor ), i_cText );
}

const uint32_t GDebugDraw::BLACK = 0x000000FF;
const uint32_t GDebugDraw::WHITE = 0xFFFFFFFF;
const uint32_t GDebugDraw::RED = 0xFF0000FF;
const uint32_t GDebugDraw::GREEN = 0x00FF00FF;
const uint32_t GDebugDraw::BLUE = 0x0000FFFF;
const uint32_t GDebugDraw::DARK_GREY = 0x373737FF;