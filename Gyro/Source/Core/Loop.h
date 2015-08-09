#ifndef _LOOP_H_
#define _LOOP_H_

#include <Windows.h>

namespace Gyro
{

	bool Initialize( HWND i_windowHandle );

	void BeginUpdate( );

	void Update( );

	void EndUpdate( );

	void Shutdown( );

	void DrawFrameStats( );
}

#endif