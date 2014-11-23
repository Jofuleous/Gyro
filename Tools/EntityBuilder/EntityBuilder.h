/*
	This class builds entities..
*/

#ifndef __ENTITY_BUILDER_H_
#define __ENTITY_BUILDER_H_

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

namespace cs6963
{
	class EntityBuilder : public cbGenericBuilder
	{
		// Inherited Interface
		//====================

	private:

		virtual const char* GetToolName() const { return "EntityBuilder"; }

		// Inherited Rendererlementation
		//=========================

	private:

		virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
	};
}

#endif	// CS6963_CTEXTUREBUILDER_H
