/*
	This class defines a Maya file translator
*/

#ifndef CS6963_CMAYAEXPORTER_H
#define CS6963_CMAYAEXPORTER_H

// Header Files
//=============

#include <maya/MPxFileTranslator.h>
#include <vector>
#include "GMayaSkeleton.h"

// Forward Declarations
//=====================

namespace
{
	struct s_vertex;
}

// Class Definition
//=================

namespace cs6963
{
	class cMayaExporter : public MPxFileTranslator
	{
		// Inherited Interface
		//====================

	public:

		// The write method is what exports the file
		virtual bool haveWriteMethod() const { return true; }
		virtual MStatus writer( const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode );

		virtual MString defaultExtension() const { return "mod"; }

		// Interface
		//==========

	public:

		static void* Create()
		{
			return new cMayaExporter;
		}

		// Implementation
		//===============

	private:

		MStatus Export(const MString& i_fileName, std::vector<const s_vertex>& i_vertexBuffer, std::vector<unsigned int>& i_indexBuffer, const GMayaSkeleton& i_skelly );
	};
}

#endif	// CS6963_CMAYAEXPORTER_H
