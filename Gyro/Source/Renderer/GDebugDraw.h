#ifndef __GYRO_RENDERER_GDEBUGDRAW_H_
#define __GYRO_RENDERER_GDEBUGDRAW_H_

// =========================================
// GDebugDraw
// Wrapper on the renderer debug draw functions.
// Maybe move this to the renderer project one day?
// =========================================

#include "Math/GVector3.h"

class GDebugDraw
{
public:
	static void	DrawLine( const GVector3& i_vStart, const GVector3& i_vEnd, float i_fStartRadius, float i_fEndRadius, uint32_t i_iColor );
	static void	DrawHudBox( float i_fTopLeftXPerc, float i_fTopLeftYPerc, float i_fWidthPerc, float i_fHeightPerc, uint32_t i_iColor );
	static void DrawHudText( int32_t i_iLeft, int32_t i_iTop, int32_t i_iRight, int32_t i_iBottom, uint32_t i_iColor, const char* i_cText );

	static const uint32_t BLACK;
	static const uint32_t WHITE;
	static const uint32_t RED;
	static const uint32_t BLUE;
	static const uint32_t GREEN;
	static const uint32_t LIGHT_GREY;
	static const uint32_t DARK_GREY;
};

#endif