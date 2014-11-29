#ifndef __GSKELETON_H_
#define __GSKELETON_H_

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
	void						BuildBindPose();
};

#endif