/*
This class defines a Maya file translator
*/

#ifndef __GANIMEXPORTER_H_
#define __GANIMEXPORTER_H_

// Header Files
//=============

#include <maya/MPxFileTranslator.h>
#include <vector>
#include "GMayaAnimClip.h"

// Class Definition
//=================

class GAnimExporter : public MPxFileTranslator
{
	// Inherited Interface
	//====================

public:

	// The write method is what exports the file
	virtual bool haveWriteMethod() const { return true; }
	virtual MStatus writer(const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode);

	virtual MString defaultExtension() const { return "anm"; }

	// Interface
	//==========

public:

	static void* Create()
	{
		return new GAnimExporter;
	}

	// Implementation
	//===============

private:

	MStatus Export( const MString& i_fileName, GMayaAnimClip& i_clip );
};

#endif	// CS6963_CMAYAEXPORTER_H
