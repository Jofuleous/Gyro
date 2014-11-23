#include "Core.h"
#include "World.h"

bool Gyro::Initialize( )
{

	GameTime.Init();

	//Initialize input
	//Initialize renderer

	if( !World::Initialize( ) )
		return false;

	return true;
}

void Gyro::BeginUpdate( )
{
	GameTime.Update();
}

void Gyro::Update( )
{
	World::Update();
}

void Gyro::EndUpdate( )
{

}