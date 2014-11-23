/*
	This class builds textures
*/

#ifndef __MATERIAL_BUILDER_H_
#define __MATERIAL_BUILDER_H_

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

namespace cs6963
{
	class MaterialBuilder : public cbGenericBuilder
	{
		// Inherited Interface
		//====================

	private:

		virtual const char* GetToolName() const { return "MaterialBuilder"; }

		// Inherited Rendererlementation
		//=========================

	private:

		virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
	};
}

#endif	// CS6963_CTEXTUREBUILDER_H
