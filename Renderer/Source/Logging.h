/*
	This provides very basic functionality for logging messages to a file
*/

#ifndef CS6963_LOGGING_H
#define CS6963_LOGGING_H

// Header Files
//=============

#include "Initialization.h"

#include <string>

// Function Declaration
//=====================

namespace cs6963
{
	bool LogMessage( const std::string& i_messageToLog );
}

#endif	// CS6963_LOGGING_H
