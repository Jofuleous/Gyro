/*
	This class builds our audio files.
*/

#ifndef __AUDIOBUILDER_H
#define __AUDIOBUILDER_H

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

class AudioBuilder : public cs6963::cbGenericBuilder
{
	// Inherited Interface
	//====================

private:

	virtual const char* GetToolName() const { return "AudioBuilder"; }

	// Inherited Rendererlementation
	//=========================

private:

	virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
};

#endif
