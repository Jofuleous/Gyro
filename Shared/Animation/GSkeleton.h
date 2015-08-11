#ifndef __ANIMATION_GSKELETON_H_
#define __ANIMATION_GSKELETON_H_

//===========================================================================
// GAnimNode.h
// Author:	Jon Futch
// Created:	December, 2014
//
// The classes that make up a skeleton and skeleton instance.
// TODO: The bone instances should maybe be stored in a cache-aligned buffer so that a threaded animation task can blow through them.
// TODO: Thread this shit.
//===========================================================================

#include "Containers/GArray.h"
#include "Math/GQuat.h"
#include "Math/GVector3.h"
#include "Containers/GHashMap.h"
#include "Utility/GHashedString.h"

class GSkeletonInstance;

class GAnimBone
{
public:
	GAnimBone() {}
	// The reference rotations are multiplied by the absolute transform to give what I think of as a "bone motion delta" for the vertices.
	GQuat		m_BindRot;
	GVector3	m_BindTranslation;
	GQuat		m_LocalRot;
	GVector3	m_LocalTranslation;
	int			m_ParentId;
	int			m_BoneId;
	u32			m_Children[8];
	u32			m_ChildCount;
};

class GAnimBoneInstance
{
public:
	int					m_ParentId;
	GSkeletonInstance*	m_SkeletonInstance;
	GQuat				m_LocalRot;
	GVector3			m_LocalTranslation;
	GQuat				m_AbsRot;
	GVector3			m_AbsTranslation;
};

class GSkeleton
{
public:
	// We assume that the root bone is named 
	GArray<GAnimBone> m_RefBones;
	
	void	Serialize( FILE* io_file );
	void	DeSerialize( FILE* i_file );
	static	GSkeletonInstance* LoadSkeleton(const char* i_fileName, FILE* i_file, unsigned int i_boneCount);
	char	m_SkeletonName[64];

	// Until we get a better asset loader system...sigh.
	static	GHashMap<unsigned int, GSkeleton*>	s_SkeletonMap;

protected:
};

class GSkeletonInstance
{
public:
	GSkeletonInstance( GSkeleton* i_skeleton);
	GSkeleton*					m_MasterSkeleton;
	GArray<GAnimBoneInstance>	m_Bones;
	GArray<GMatrix4>			m_DeltaBoneTransform;
	void						BuildBindPose();
	void						EvaluateFullInstance();
	void						CreateBoneMatrices();
};

#endif