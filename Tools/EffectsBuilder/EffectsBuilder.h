/*
	This class builds entities..
*/

#ifndef __EFFECTS_BUILDER_H_
#define __EFFECTS_BUILDER_H_

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

namespace cs6963
{
	class EffectsBuilder : public cbGenericBuilder
	{
		// Inherited Interface
		//====================

	private:

		virtual const char* GetToolName() const { return "EffectsBuilder"; }

		// Inherited Rendererlementation
		//=========================

	private:

		virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
	};
}

#endif	// CS6963_CTEXTUREBUILDER_H
