// Header Files
//=============

#define _CRT_SECURE_NO_WARNINGS

#include "GAnimExporter.h"
#include <cstdint>
#include <map>
#include <maya/MDagPath.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnIkJoint.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDag.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItSelectionList.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>
#include <sstream>
#include <string>
#include <malloc.h>
#include <stdio.h>
#include "Math/GQuat.h"
#include "Animation/GSkeleton.h"

// Inherited Interface
//====================

MStatus GAnimExporter::writer( const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode)
{
	// Get the name of the file that should be exported
	const MString fileName = i_file.fullName();

	GMayaAnimClip clip;
	{
		// The user will decide whether to consider the entire scene or just a selection
		MStatus status;
		if (i_mode == MPxFileTranslator::kExportAccessMode)
		{
			if (! clip.ProcessAnim() )
				return MStatus::kFailure;
		}
		else if (i_mode == MPxFileTranslator::kExportActiveAccessMode)
		{
			if( !clip.ProcessAnim() )
				return MStatus::kFailure;
		}
		else
		{
			MGlobal::displayError("Unexpected file access mode");
			return MStatus::kFailure;
		}
	}

	// Export the file
	return Export( fileName, clip );
}

// Implementation
//===============

MStatus GAnimExporter::Export( const MString& i_fileName, GMayaAnimClip& i_clip )
{

	FILE* targetFile = fopen(i_fileName.asChar(), "w+b");
	i_clip.m_Clip.Serialize(targetFile);
	
	fclose(targetFile);
	// You will need to write this function so that it exports the vertex and index buffer information in your file format
	return MStatus::kSuccess;
}