#include "GSkeleton.h"

GHashMap<unsigned int, GSkeleton*> GSkeleton::s_SkeletonMap;

GSkeletonInstance::GSkeletonInstance(GSkeleton* i_skeleton)
{
	m_MasterSkeleton = i_skeleton;
	//m_Bones.Resize(i_skeleton->m_RefBones.Size());
	for (u32 i = 0; i < i_skeleton->m_RefBones.Count(); i++)
	{
		GAnimBoneInstance boneInstance;
		boneInstance.m_LocalRot = i_skeleton->m_RefBones[i].m_LocalRot;
		boneInstance.m_LocalTranslation = i_skeleton->m_RefBones[i].m_LocalTranslation;
		boneInstance.m_ParentId = i_skeleton->m_RefBones[i].m_ParentId;
		boneInstance.m_SkeletonInstance = this;
		m_Bones.Push( boneInstance );
		m_DeltaBoneTransform.Resize(m_Bones.Count());
		m_DeltaBoneTransform.SetCount(m_Bones.Count());
	}

	BuildBindPose();

	m_MasterSkeleton = i_skeleton;
}

void GSkeletonInstance::BuildBindPose()
{
	if (m_MasterSkeleton->m_RefBones.Count() > 0)
	{
		m_Bones[0].m_AbsRot = m_MasterSkeleton->m_RefBones[0].m_LocalRot;
		m_Bones[0].m_AbsTranslation = m_MasterSkeleton->m_RefBones[0].m_LocalTranslation;
	}
	for (int i = 1; i < m_MasterSkeleton->m_RefBones.Count(); i++)
	{
		m_Bones[i].m_AbsRot = m_MasterSkeleton->m_RefBones[i].m_LocalRot * m_Bones[m_Bones[i].m_ParentId].m_AbsRot;
		m_Bones[i].m_AbsTranslation = m_Bones[m_Bones[i].m_ParentId].m_AbsTranslation + (m_Bones[m_Bones[i].m_ParentId].m_AbsRot * m_MasterSkeleton->m_RefBones[i].m_LocalTranslation);
	}
}

void GSkeletonInstance::EvaluateFullInstance( )
{
	if (m_MasterSkeleton->m_RefBones.Count() > 0)
	{
		m_Bones[0].m_AbsRot = m_Bones[0].m_LocalRot;
		m_Bones[0].m_AbsTranslation = m_Bones[0].m_LocalTranslation;
	}
	for ( int i = 1; i < m_MasterSkeleton->m_RefBones.Count(); i++ )
	{
		m_Bones[i].m_AbsRot = m_Bones[i].m_LocalRot * m_Bones[m_Bones[i].m_ParentId].m_AbsRot;
		m_Bones[i].m_AbsTranslation = m_Bones[m_Bones[i].m_ParentId].m_AbsTranslation + (m_Bones[m_Bones[i].m_ParentId].m_AbsRot * m_Bones[i].m_LocalTranslation);
	}
}

void GSkeletonInstance::CreateBoneMatrices()
{
	for (int i = 0; i < m_Bones.Count(); i++)
	{
		(m_MasterSkeleton->m_RefBones[i].m_BindRot * m_Bones[i].m_AbsRot).GetMatrix(m_DeltaBoneTransform[i]);
		m_DeltaBoneTransform[i].PokeTranslation(m_Bones[i].m_AbsTranslation + (m_Bones[i].m_AbsRot * m_MasterSkeleton->m_RefBones[i].m_BindTranslation));
	}
}

GSkeletonInstance* GSkeleton::LoadSkeleton(const char* i_fileName, FILE* i_file, unsigned int i_boneCount)
{
	GHashNode<unsigned int, GSkeleton*>* node = s_SkeletonMap[GHashedString::Hash(i_fileName)];
	GSkeleton* skelly;
	if (node)
		skelly = node->m_data;
	else
	{
		skelly = new GSkeleton();
		strcpy(skelly->m_SkeletonName, i_fileName);
		skelly->m_RefBones.Resize(i_boneCount);
		fread(&skelly->m_RefBones[0], sizeof(GAnimBone), i_boneCount, i_file);
		skelly->m_RefBones.SetCount(i_boneCount);
		s_SkeletonMap.Set(GHashedString::Hash(i_fileName), skelly);
	}

	return new GSkeletonInstance(skelly);
}


void GSkeleton::Serialize(FILE* io_file)
{
	//fwrite();
}

void GSkeleton::DeSerialize(FILE* io_file)
{

}