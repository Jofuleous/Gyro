/*
	This class manages user-configurable settings
*/

#ifndef __USERSETTINGS_H
#define __USERSETTINGS_H


namespace Gyro
{
	namespace UserSettings
	{
		unsigned int GetScreenHeight();
		unsigned int GetScreenWidth();
		unsigned int IsFullScreenModeEnabled();

		bool Initialize();
	}
}

#endif	// __USERSETTINGS_H
