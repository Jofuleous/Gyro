#ifndef __FBXSCENEBUILDER_H_
#define __FBXSCENEBUILDER_H_

// Header Files
//=============

#include "../GenericBuilder/cbGenericBuilder.h"

#include <fbxsdk.h>

// Class Definition
//=================
class FBXSceneBuilder : public cs6963::cbGenericBuilder
{
	// Inherited Interface
	//====================

private:

	static FbxManager* g_FbxSdkManager;

	virtual const char* GetToolName() const { return "FBXSceneBuilder"; }

	// Inherited Rendererlementation
	//=========================

private:
	virtual bool Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const;

public:
};

#endif