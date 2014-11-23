/*
	This class builds vertex shaders
*/

#ifndef CS6963_CVERTEXSHADERBUILDER_H
#define CS6963_CVERTEXSHADERBUILDER_H

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

// Class Definition
//=================

namespace cs6963
{
	class cVertexShaderBuilder : public cbGenericBuilder
	{
		// Inherited Interface
		//====================

	private:

		virtual const char* GetToolName() const { return "VertexShaderBuilder"; }

		// Inherited Rendererlementation
		//=========================

	private:

		virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;
	};
}

#endif	// CS6963_CVERTEXSHADERBUILDER_H
