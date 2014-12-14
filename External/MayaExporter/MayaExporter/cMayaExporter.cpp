// Header Files
//=============

#define _CRT_SECURE_NO_WARNINGS

#include "cMayaExporter.h"

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
#include <maya/MFnSkinCluster.h>
#include <sstream>
#include <string>
#include <malloc.h>
#include <stdio.h>
#include "Math/GQuat.h"
#include "Animation/GSkeleton.h"

// Vertex Definition
//==================

namespace
{
	struct s_vertex
	{
		int positionIndex;
		float x, y, z;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float u, v;
		uint8_t r, g, b, a;

		s_vertex( int i_positionIndex, const MPoint& i_position, const MFloatVector& i_normal, const MFloatVector& i_tangent, const MFloatVector& i_binormal,
			const float i_texcoordU, const float i_texcoordV,
			const MColor& i_vertexColor )
			:
			positionIndex(i_positionIndex),
			x( static_cast<float>( i_position.x ) ), y( static_cast<float>( i_position.y ) ), z( static_cast<float>( i_position.z ) ),
			nx( i_normal.x ), ny( i_normal.y ), nz( i_normal.z ),
			tx( i_tangent.x ), ty( i_tangent.y ), tz( i_tangent.z ),
			bx( i_binormal.x ), by( i_binormal.y ), bz( i_binormal.z ),
			u( i_texcoordU ), v( i_texcoordV ),
			r( static_cast<uint8_t>( ( i_vertexColor.r * 255.0f ) + 0.5f ) ),
			g( static_cast<uint8_t>( ( i_vertexColor.g * 255.0f ) + 0.5f ) ),
			b( static_cast<uint8_t>( ( i_vertexColor.b * 255.0f ) + 0.5f ) ),
			a( static_cast<uint8_t>( ( i_vertexColor.a * 255.0f ) + 0.5f ) )
		{

		}
	};
}

// Helper Function Declarations
//=============================

namespace
{
	std::string CreateUniqueVertexKey( const int i_positionIndex, const int i_normalIndex,
		const int i_texcoordIndex, const int i_vertexColorIndex );
	MStatus ProcessAllMeshes( std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus ProcessSelectedMeshes( std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus ProcessSingleMesh( const MFnMesh& i_mesh,
		std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus FillVertexAndIndexBuffer( const MFnMesh& i_mesh,
		std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus ProcessSkeleton( GMayaSkeleton& i_skeleton );
}

// Inherited Interface
//====================

MStatus cs6963::cMayaExporter::writer( const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode )
{
	// Get the name of the file that should be exported
	const MString fileName = i_file.fullName();

	// Gather the vertex and index buffer information
	std::vector<const s_vertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	// Gather the skeletal data.
	GMayaSkeleton skeleton;

	{
		// The user will decide whether to consider the entire scene or just a selection
		MStatus status;
		if ( i_mode == MPxFileTranslator::kExportAccessMode )
		{
			status = ProcessAllMeshes( vertexBuffer, indexBuffer );
			if ( !status )
				return status;
			ProcessSkeleton( skeleton );
		}
		else if ( i_mode == MPxFileTranslator::kExportActiveAccessMode )
		{
			status = ProcessSelectedMeshes( vertexBuffer, indexBuffer );
			if ( !status )
				return status;
			ProcessSkeleton( skeleton );
		}
		else
		{
			MGlobal::displayError( "Unexpected file access mode" );
			return MStatus::kFailure;
		}
	}

	// Export the file
	return Export( fileName, vertexBuffer, indexBuffer, skeleton );
}

// Implementation
//===============

MStatus cs6963::cMayaExporter::Export(const MString& i_fileName, std::vector<const s_vertex>& i_vertexBuffer, std::vector<unsigned int>& i_indexBuffer, const GMayaSkeleton& i_skelly )
{

	FILE* targetFile = fopen( i_fileName.asChar(), "w" );

	// at max, we support 16 characters per value.  10 for the significand and 6 for the precision.
	// we have a max vertex size of: 
	// x = 16, y = 16, z = 16, u = 16, v = 16, xn = 16, yn = 16, zn = 16, tx = 16, ty = 16, tz = 16, bx = 16, by = 16, bz = 16 = 224 bytes
	// + spaces (13) + new line (1) = 128 + 17 = 238

	unsigned int size = 0;

	//write header
	char vertexCount[128];
	int vertexType = 0; // later 
	if (i_skelly.m_influences.size() > 0)
		vertexType = 1;
	size = sprintf(vertexCount, "VertexCount: %d\nTriangleCount: %d\nVertexType: %d\nVertices\n", i_vertexBuffer.size(), i_indexBuffer.size() /3, vertexType );
	fwrite( vertexCount, size, 1, targetFile );

	unsigned int position = 0;
	char* vertexBuffer = (char*) malloc( 350 * i_vertexBuffer.size() ); // hack: figure out later.
	for(unsigned int i = 0; i < i_vertexBuffer.size(); i++ )
	{
		//X
		//if(i_vertexBuffer[i].x > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].x );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//Y
		//if(i_vertexBuffer[i].y > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].y );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//Z
		//if(i_vertexBuffer[i].z > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", -i_vertexBuffer[i].z ); //flip z because Maya is right handed...possibly.
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//U
		//if(i_vertexBuffer[i].u > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].u );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//V
		//if(i_vertexBuffer[i].v > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", 1 - i_vertexBuffer[i].v );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//NX
		//if(i_vertexBuffer[i].nx > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].nx );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//NY
		//if(i_vertexBuffer[i].ny > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].ny );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//NZ
		//if(i_vertexBuffer[i].nz > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", -i_vertexBuffer[i].nz ); //flip z because Maya is right handed...possibly.
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//TX
		//if(i_vertexBuffer[i].tx > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].tx );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//TY
		//if(i_vertexBuffer[i].ty > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].ty );
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//TZ
		//if(i_vertexBuffer[i].tz > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", -i_vertexBuffer[i].tz ); //flip z because Maya is right handed...possibly.
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//BX
		//if(i_vertexBuffer[i].bx > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", -i_vertexBuffer[i].bx ); //negate these because of the right hand conversion
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//BY
		//if(i_vertexBuffer[i].by > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", -i_vertexBuffer[i].by ); //negate these because of the right hand conversion
		vertexBuffer[position+size]=' '; //space..
		position += (size + 1);

		//BZ
		//if(i_vertexBuffer[i].bz > 9999999999)
		//	return MStatus::kFailure;
		size = sprintf( vertexBuffer+position, "%f", i_vertexBuffer[i].bz ); //negate these because of the right hand conversion

		//////////////////////////////////////////////////////
		// Skinning Data
		//////////////////////////////////////////////////////

		if (vertexType > 0 )
		{
			vertexBuffer[position + size] = ' '; //space..
			position += (size + 1);

			float mag = 0.0f;
			for (int j = 0; j < 4; j++)
			{
				if (j < i_skelly.m_influences[i_vertexBuffer[i].positionIndex].size())
				{
					size = sprintf(vertexBuffer + position, "%d", i_skelly.m_influences[i_vertexBuffer[i].positionIndex][j].m_BoneId);
					mag += i_skelly.m_influences[i_vertexBuffer[i].positionIndex][j].m_Weight;
				}
				else
					size = sprintf( vertexBuffer + position, "%d", 0 );

					vertexBuffer[position + size] = ' '; //space..
					position += (size + 1);
			}

			for (int j = 0; j < 4; j++)
			{
				if (j < i_skelly.m_influences[i_vertexBuffer[i].positionIndex].size())
				{
					if ( mag <= 0.0f )
						size = sprintf(vertexBuffer + position, "%f", i_skelly.m_influences[i_vertexBuffer[i].positionIndex][j].m_Weight);
					else
						size = sprintf(vertexBuffer + position, "%f", i_skelly.m_influences[i_vertexBuffer[i].positionIndex][j].m_Weight / mag);
				}
				else
					size = sprintf(vertexBuffer + position, "%f", 0.0f );

				if (j < 4 - 1)
				{
					vertexBuffer[position + size] = ' '; //space..
					position += (size + 1);
				}
			}
		}

		vertexBuffer[position + size] = '\n'; //new line..
		position += (size + 1);
	}
	
	fwrite( vertexBuffer, position, 1, targetFile );
	char indices[16];
	size = sprintf(indices, "Indices\n" );
	fwrite( indices, size, 1, targetFile );


	//maximum size for a u16 (which is what i read in..) is 65536.  maximum length of a number is 5, so...
	// one line = 5 + 5 + 5 + null terminator(3) + spaces (2) + newline (1) = 21
	position = 0;
	//if( i_indexBuffer.size() > 65536 )
	//	return MStatus::kFailure;
	char* indexBuffer = (char*) malloc( 7 * i_indexBuffer.size() ); //because lazy..
	for( unsigned int j = 0; j < i_indexBuffer.size(); j+=3 )
	{
		size = sprintf( indexBuffer+position, "%d", i_indexBuffer[j+2] );
		indexBuffer[position+size]='\n'; //newline..
		position += (size + 1);

		size = sprintf( indexBuffer+position, "%d", i_indexBuffer[j+1] );
		indexBuffer[position+size]=' '; //space..
		position += (size + 1);

		size = sprintf( indexBuffer+position, "%d", i_indexBuffer[j] );
		indexBuffer[position+size]=' '; //space..
		position += (size + 1);
	}
	fwrite( indexBuffer, position-1, 1, targetFile );


	//write skeleton header
	char boneCount[64];
	size = sprintf( boneCount, "\nSkeletonBoneCount: %d\n", i_skelly.m_Skeleton.m_RefBones.Count() );
	fwrite(boneCount, size, 1, targetFile);

	if (i_skelly.m_Skeleton.m_RefBones.Count() > 0)
	{
		// at max, we support 16 characters per value.  10 for the significand and 6 for the precision.
		// we have a max bone size of
		// bone id = 4
		// parent id = 4
		// qx = 16, qy = 16, qz = 16, qw = 16 --local rot = 64
		// x = 16, y = 16, z = 16 = 112 bytes --local trans = 48
		// qx = 16, qy = 16, qz = 16, qw = 16 -- absolute rot = 64
		// x = 16, y = 16, z = 16 --absolute trans... = 48
		// + spaces (10) + new line (6) =  240..round up to 256 for the children :)
		position = 0;
		char* skeletonBuffer = (char*)malloc(i_skelly.m_Skeleton.m_RefBones.Count() * 300);
		for (int i = 0; i < i_skelly.m_Skeleton.m_RefBones.Count(); i++)
		{
			// bone id
			size = sprintf(skeletonBuffer + position, "%d", i);
			skeletonBuffer[position + size] = '\n'; //newline..
			position += (size + 1);

			// parent id
			size = sprintf(skeletonBuffer + position, "%d", i_skelly.m_Skeleton.m_RefBones[i].m_ParentId);
			skeletonBuffer[position + size] = '\n'; //newline..
			position += (size + 1);

			// LOCAL ROTATION
			// qx
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_LocalRot.m_X);
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// qy
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_LocalRot.m_Y);
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// qz
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_LocalRot.m_Z);
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// qw
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_LocalRot.m_W);
			skeletonBuffer[position + size] = '\n'; //newline..
			position += (size + 1);

			// LOCAL TRANSLATION
			// tx
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_LocalTranslation.x());
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// ty
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_LocalTranslation.y());
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// tz
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_LocalTranslation.z());
			skeletonBuffer[position + size] = '\n'; //newline..
			position += (size + 1);

			// BIND ROTATION.
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_BindRot.m_X);
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// qy
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_BindRot.m_Y);
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// qz
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_BindRot.m_Z);
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// qw
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_BindRot.m_W);
			skeletonBuffer[position + size] = '\n'; //newline..
			position += (size + 1);

			// BIND TRANSLATION
			// tx
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_BindTranslation.x());
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// ty
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_BindTranslation.y());
			skeletonBuffer[position + size] = ' '; //space..
			position += (size + 1);
			// tz
			size = sprintf(skeletonBuffer + position, "%f", i_skelly.m_Skeleton.m_RefBones[i].m_BindTranslation.z());
			skeletonBuffer[position + size] = '\n'; //newline..
			position += (size + 1);

			// now let's write out all the children...
			size = sprintf(skeletonBuffer + position, "%d", i_skelly.m_Skeleton.m_RefBones[i].m_ChildCount);
			skeletonBuffer[position + size] = '\n'; //newline..
			position += (size + 1);

			for (int q = 0; q < i_skelly.m_Skeleton.m_RefBones[i].m_ChildCount; q++)
			{
				size = sprintf(skeletonBuffer + position, "%d", (int)i_skelly.m_Skeleton.m_RefBones[i].m_Children[q]);
				if (q + 1 < i_skelly.m_Skeleton.m_RefBones[i].m_ChildCount)
				{
					skeletonBuffer[position + size] = ' '; //newline..
					position += (size + 1);
				}
			}

			if (i_skelly.m_Skeleton.m_RefBones[i].m_ChildCount > 0)
			{
				skeletonBuffer[position + size] = '\n'; //newline..
				position += (size + 1);
			}
		}

		fwrite(skeletonBuffer, position - 1, 1, targetFile);
		free(skeletonBuffer);
	}


	free(indexBuffer);
	free(vertexBuffer);

	fclose( targetFile );

	// You will need to write this function so that it exports the vertex and index buffer information in your file format
	return MStatus::kSuccess;
}

// Helper Function Definitions
//============================

namespace
{
	std::string CreateUniqueVertexKey( const int i_positionIndex, const int i_normalIndex,
		const int i_texcoordIndex, const int i_vertexColorIndex )
	{
		std::ostringstream vertexKey;
		vertexKey << i_positionIndex << "_" << i_normalIndex << "_" << i_texcoordIndex << "_" << i_vertexColorIndex;
		return vertexKey.str();
	}

	MStatus ProcessAllMeshes( std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		for ( MItDag i( MItDag::kDepthFirst, MFn::kMesh ); !i.isDone(); i.next() )
		{
			MFnMesh mesh( i.item() );
			if ( !ProcessSingleMesh( mesh, o_vertexBuffer, o_indexBuffer ) )
			{
				return MStatus::kFailure;
			}
		}

		return MStatus::kSuccess;
	}

	MStatus ProcessSkeleton( GMayaSkeleton& i_skeleton )
	{
		if (i_skeleton.Extract())
		{
			i_skeleton.ExtractSkinningData();
			return MStatus::kSuccess;
		}

		return MStatus::kFailure;
	}

	MStatus ProcessSelectedMeshes( std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		// Iterate through each selected mesh
		MSelectionList selectionList;
		MStatus status = MGlobal::getActiveSelectionList( selectionList );
		if ( status )
		{
			for ( MItSelectionList i( selectionList, MFn::kMesh ); !i.isDone(); i.next() )
			{
				MDagPath dagPath;
				i.getDagPath( dagPath );
				MFnMesh mesh( dagPath );
				if ( !ProcessSingleMesh( mesh, o_vertexBuffer, o_indexBuffer ) )
				{
					return MStatus::kFailure;
				}
			}
		}
		else
		{
			MGlobal::displayError( MString( "Failed to get active selection list: " ) + status.errorString() );
			return MStatus::kFailure;
		}

		return MStatus::kSuccess;
	}

	MStatus ProcessSingleMesh( const MFnMesh& i_mesh,
		std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		if ( i_mesh.isIntermediateObject() )
		{
			return MStatus::kSuccess;
		}

		return FillVertexAndIndexBuffer( i_mesh, o_vertexBuffer, o_indexBuffer );
	}

	MStatus FillVertexAndIndexBuffer( const MFnMesh& i_mesh,
		std::vector<const s_vertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		MStatus status;

		// Get a list of the positions
		MPointArray positions;
		{
			status = i_mesh.getPoints( positions );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		// Get a list of the normals
		MFloatVectorArray normals;
		{
			status = i_mesh.getNormals( normals );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		MFloatVectorArray binormals;
		{
			status = i_mesh.getBinormals( binormals );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		MFloatVectorArray tangents;
		{
			status = i_mesh.getTangents( tangents );
			if( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		// Get a list of the texture coordinates
		MFloatArray texcoordUs, texcoordVs;
		{
			status = i_mesh.getUVs( texcoordUs, texcoordVs );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}


		// Get a list of the vertex colors
		MColorArray vertexColors;
		{
			int colorSetCount = i_mesh.numColorSets();
			if ( colorSetCount > 0 )
			{
				MString* useDefaultColorSet = NULL;
				MColor defaultColor( 1.0f, 1.0f, 1.0f, 1.0f );
				status = i_mesh.getColors( vertexColors, useDefaultColorSet, &defaultColor );
				if ( !status )
				{
					MGlobal::displayError( status.errorString() );
					return status;
				}
			}
		}

		// Gather vertex and triangle information
		std::map<const std::string, const s_vertex> uniqueVertices;
		std::vector<const std::string> triangles;
		{
			MPointArray trianglePositions;
			MIntArray positionIndices;
			for ( MItMeshPolygon i( i_mesh.object() ); !i.isDone(); i.next() )
			{
				if ( i.hasValidTriangulation() )
				{
					// Store information for each vertex in the polygon
					std::map<int, const std::string> indexToKeyMap;
					{
						MIntArray vertices;
						status = i.getVertices( vertices );
						if ( status )
						{
							for ( unsigned int j = 0; j < vertices.length(); ++j )
							{
								const int positionIndex = vertices[j];
								const int normalIndex = i.normalIndex( j );
								const int tangentIndex = i.tangentIndex( j );
								const int binormalIndex = tangentIndex;
								int texcoordIndex;
								{
									status = i.getUVIndex( j, texcoordIndex );
									if ( !status )
									{
										MGlobal::displayError( status.errorString() );
										return status;
									}
								}
								
								int vertexColorIndex = -1;
								MColor vertexColor( 1.0f, 1.0f, 1.0f, 1.0f );
								{
									int colorSetCount = i_mesh.numColorSets();
									if ( colorSetCount > 0 )
									{
										status = i.getColorIndex( j, vertexColorIndex );
										if ( status )
										{
											vertexColor = vertexColors[vertexColorIndex];
										}
										else
										{
											MGlobal::displayError( status.errorString() );
											return status;
										}
									}
								}
								const std::string vertexKey = CreateUniqueVertexKey( positionIndex, normalIndex,
									texcoordIndex, vertexColorIndex );
								indexToKeyMap.insert( std::make_pair( positionIndex, vertexKey ) );
								uniqueVertices.insert( std::make_pair( vertexKey,
									s_vertex( positionIndex, positions[positionIndex], normals[normalIndex], tangents[tangentIndex], binormals[binormalIndex],
										texcoordUs[texcoordIndex], texcoordVs[texcoordIndex],
										vertexColor ) ) );
							}
						}
						else
						{
							MGlobal::displayError( status.errorString() );
							return status;
						}
					}
					// Store information for each individual triangle in the polygon
					{
						int triangleCount = 0;
						i.numTriangles( triangleCount );
						for ( int j = 0; j < triangleCount; ++j )
						{
							i.getTriangle( j, trianglePositions, positionIndices );
							for ( unsigned int k = 0; k < positionIndices.length(); ++k )
							{
								const int positionIndex = positionIndices[k];
								std::map<int, const std::string>::iterator mapLookUp = indexToKeyMap.find( positionIndex );
								if ( mapLookUp != indexToKeyMap.end() )
								{
									const std::string& vertexKey = mapLookUp->second;
									triangles.push_back( vertexKey );
								}
								else
								{
									MGlobal::displayError( "A triangle gave a different vertex index than the polygon gave" );
									return MStatus::kFailure;
								}
							}
						}
					}
				}
				else
				{
					MGlobal::displayError( "This mesh has an invalid triangulation" );
					return MStatus::kFailure;
				}
			}
		}

		// Convert the triangle information to vertex and index buffers
		o_vertexBuffer.clear();
		o_indexBuffer.clear();
		o_indexBuffer.resize( triangles.size() );
		{
			std::map<const std::string, unsigned int> keyToIndexMap;
			for ( std::map<const std::string, const s_vertex>::iterator i = uniqueVertices.begin(); i != uniqueVertices.end(); ++i )
			{
				keyToIndexMap.insert( std::make_pair( i->first, static_cast<unsigned int>( o_vertexBuffer.size() ) ) );
				o_vertexBuffer.push_back( i->second );
			}
			for ( size_t i = 0; i < triangles.size(); ++i )
			{
				const std::string& key = triangles[i];
				unsigned int index = keyToIndexMap.find( key )->second;
				o_indexBuffer[i] = index;
			}
		}

		return MStatus::kSuccess;
	}
}
