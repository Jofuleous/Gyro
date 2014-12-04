/*
	This class builds textures
*/

#ifndef __ANIMBUILDER_H_
#define __ANIMBUILDER_H_

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

class AnimBuilder : public cs6963::cbGenericBuilder
{
	// Inherited Interface
	//====================

private:

	virtual const char* GetToolName() const { return "AnimBuilder"; }

	// Inherited Rendererlementation
	//=========================

private:

	virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
};

#endif	//
