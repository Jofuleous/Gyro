/*
	This class builds entities..
*/

#ifndef __CHARACTER_BUILDER_H_
#define __CHARACTER_BUILDER_H_

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

class CharacterBuilder : public cs6963::cbGenericBuilder
{
	// Inherited Interface
	//====================

private:

	virtual const char* GetToolName() const { return "CharacterBuilder"; }

	// Inherited Rendererlementation
	//=========================

private:

	virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
};

#endif
