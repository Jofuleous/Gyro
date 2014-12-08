#include "cMeshBuilder.h"

#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <fstream>
#include <vector>
#include <malloc.h>
#include <cstdint>

#include "Animation/GSkeleton.h"

#define UINT8 uint8_t
#define UINT16 uint16_t


struct s_vertexColor
{
	float x, y, z;
	UINT8 alpha;
	UINT8 red;
	UINT8 green;
	UINT8 blue;
};

struct s_vertexTexture
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
};

struct s_vertexTNTB //vertex texture, normal, tangent, binormal
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
};

struct s_SkinnedVertexTNTB
{
	float		x, y, z;
	float		u, v;
	float		nx, ny, nz;
	float		tx, ty, tz;
	float		bx, by, bz;
	float		weight[4];
	unsigned char		bonedId[4];
};

bool cs6963::cMeshBuilder::Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const
{
	using namespace std;
	std::ifstream file( i_fileName_source );

	if( !file )
	{
		std::cerr << "Could not find file: \"" << i_fileName_source << "\"";
		return false;
	}

	unsigned vertexCount;
	unsigned triangleCount;
	unsigned boneCount;
	unsigned vertexType;

	std::string ignore;

	// get triangle and vertex count..
	file >> ignore >> vertexCount; // "VertexCount" "#"
	file >> ignore >> triangleCount; // "TriangleCount" "#"
	file >> ignore >> vertexType;
	file >> ignore; // "Vertices"


	// hack: figure out a way of doing this better later.
	// NON-SKINNED VERTEX
	std::vector<s_vertexTNTB> vertices(vertexCount);
	if (vertexType == 0)
	{
		for (unsigned i = 0; i < vertexCount; ++i)
		{
			file >> vertices[i].x >> vertices[i].y >> vertices[i].z;

			file >> vertices[i].u; // u

			file >> vertices[i].v; // v

			file >> vertices[i].nx >> vertices[i].ny >> vertices[i].nz; // n

			file >> vertices[i].tx >> vertices[i].ty >> vertices[i].tz; // t

			file >> vertices[i].bx >> vertices[i].by >> vertices[i].bz; // b
		}
	}

	// SKINNED VERTEX
	std::vector<s_SkinnedVertexTNTB> skinnedVerts(vertexCount);
	if (vertexType == 1)
	{
		for (unsigned i = 0; i < vertexCount; ++i)
		{
			file >> skinnedVerts[i].x >> skinnedVerts[i].y >> skinnedVerts[i].z;

			file >> skinnedVerts[i].u; // u

			file >> skinnedVerts[i].v; // v

			file >> skinnedVerts[i].nx >> skinnedVerts[i].ny >> skinnedVerts[i].nz; // n

			file >> skinnedVerts[i].tx >> skinnedVerts[i].ty >> skinnedVerts[i].tz; // t

			file >> skinnedVerts[i].bx >> skinnedVerts[i].by >> skinnedVerts[i].bz; // b

			unsigned bone0, bone1, bone2, bone3;
			file >> bone0 >> bone1 >> bone2 >> bone3;
			skinnedVerts[i].bonedId[0] = bone0;
			skinnedVerts[i].bonedId[1] = bone1;
			skinnedVerts[i].bonedId[2] = bone2;
			skinnedVerts[i].bonedId[3] = bone3;

			file >> skinnedVerts[i].weight[0] >> skinnedVerts[i].weight[1] >> skinnedVerts[i].weight[2] >> skinnedVerts[i].weight[3];
		}
	}

	file >> ignore; //"Indices"
	
	unsigned indexCount = triangleCount * 3;
	std::vector<unsigned short> indices( indexCount );

	for( unsigned i = 0; i < indexCount; i++ )
	{
		file >> indices[i];
	}

	// get skeletal data, if any...
	file >> ignore >> boneCount;
	GSkeleton skelly;
	if (boneCount > 0 && ignore.compare("SkeletonBoneCount:") == 0)
	{
		skelly.m_RefBones.Resize(boneCount);
		for (int i = 0; i < boneCount; i++)
		{
			GAnimBone bone;
			file >> bone.m_BoneId; // bone id.
			file >> bone.m_ParentId; // parent id
			file >> bone.m_LocalRot.m_X >> bone.m_LocalRot.m_Y >> bone.m_LocalRot.m_Z >> bone.m_LocalRot.m_W; // local rot
			file >> bone.m_LocalTranslation._x >> bone.m_LocalTranslation._y >> bone.m_LocalTranslation._z; // local translation
			file >> bone.m_BindRot.m_X >> bone.m_BindRot.m_Y >> bone.m_BindRot.m_Z >> bone.m_BindRot.m_W; // bind rot
			file >> bone.m_BindTranslation._x >> bone.m_BindTranslation._y >> bone.m_BindTranslation._z; // local translation
			file >> bone.m_ChildCount; // child count.
			for (int q = 0; q < bone.m_ChildCount; q++)
				file >> bone.m_Children[q];
			skelly.m_RefBones.Push(bone);
		}
	}
	else
		boneCount = 0;

	file.close();

	//copy should be faster than a ton of different file writes...i think?
	//s_vertex* onHeap = (s_vertex*) malloc( sizeof( s_vertex ) * verts.size() );
	
	FILE* targetFile = fopen( i_fileName_target, "w+b" );
	//wow
	//http://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array-c
	fwrite( &vertexCount, sizeof( unsigned ), 1, targetFile );
	fwrite( &triangleCount, sizeof( unsigned ), 1, targetFile );
	fwrite(&vertexType, sizeof(unsigned), 1, targetFile);
	fwrite( &boneCount, sizeof(unsigned), 1, targetFile);
	if ( vertexType == 0 )
		fwrite( &vertices[0], sizeof( s_vertexTNTB ), vertices.size(), targetFile );
	else if ( vertexType == 1 )
		fwrite(&skinnedVerts[0], sizeof(s_SkinnedVertexTNTB), skinnedVerts.size(), targetFile);
	fwrite( &indices[0], sizeof( unsigned short ), indices.size(), targetFile );
	fwrite(&skelly.m_RefBones[0], sizeof(GAnimBone), skelly.m_RefBones.Count(), targetFile);
	fclose( targetFile );

	return true;
}