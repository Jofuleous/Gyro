/*
	This class builds environment map (cubic) textures
*/

#ifndef CS6963_ENVIRONMENTMAPBUILDER_H
#define CS6963_ENVIRONMENTMAPBUILDER_H

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

namespace cs6963
{
	class EnvironmentMapBuilder : public cbGenericBuilder
	{
		// Inherited Interface
		//====================

	private:

		virtual const char* GetToolName() const { return "EnvironmentMapBuilder"; }

		// Inherited Rendererlementation
		//=========================

	private:

		virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
	};
}

#endif	// CS6963_CTEXTUREBUILDER_H
