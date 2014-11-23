#include "Vector3.h"

//
class GTriangle
{
	GTriangle( ) {} // nothing for now.
	GTriangle( const Gyro::Vector3& i_zero, const Gyro::Vector3 i_one, const Gyro::Vector3 i_two ) :
		zero( i_zero ),
		one( i_one ),
		two( i_two )
		{

		}

	Gyro::Vector3 zero;
	Gyro::Vector3 one;
	Gyro::Vector3 two;
	Gyro::Vector3 normal;
};