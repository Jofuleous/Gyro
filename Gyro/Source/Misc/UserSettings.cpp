#include "UserSettings.h"
#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaState.h"
#include "LuaPlus/LuaTableIterator.h"

namespace
{
	unsigned int screen_height = 512;
	unsigned int screen_width = 512;
}

unsigned int Gyro::UserSettings::GetScreenHeight()
{
	return screen_height;
}

unsigned int Gyro::UserSettings::GetScreenWidth()
{
	return screen_width;
}

bool Gyro::UserSettings::Initialize()
{
	LuaPlus::LuaState* theLuaState = LuaPlus::LuaState::Create( true );
	if( theLuaState->DoFile( "data/UserSettings.ini" ) )
		if( theLuaState->GetTop() == 1 )
			assert( 0 ); // an error occured.


	LuaPlus::LuaObject& settings = theLuaState->GetGlobal( "UserSettings" );
	LuaPlus::LuaObject& screen = settings["Screen"];
	screen_width = screen["Width"].GetInteger();
	screen_height = screen["Height"].GetInteger();

	return true;
}