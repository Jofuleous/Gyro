#ifndef _CORE_H_
#define _CORE_H_

#include "Clock.h"

namespace Gyro
{
	static Clock GameTime;

	bool Initialize( );

	void BeginUpdate( );

	void Update( );

	void EndUpdate( );
}

#endif