#include "GMayaSkeleton.h"
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MFnTransform.h>
#include <maya/MVector.h>

bool GMayaSkeleton::Extract( MDagPath i_path, int i_parent )
{
	GAnimBone bone;
	bone.m_ChildCount = 0;
	bone.m_BoneId = m_Skeleton.m_RefBones.Count();
	bone.m_ParentId = i_parent;
	MFnTransform transform( i_path );
	double qx, qy, qz, qw;

	// Set the local transform data.
	transform.getRotationQuaternion(qx, qy, qz, qw, MSpace::kTransform);
	bone.m_LocalRot.Set(qx, qy, qz, qw );
	MVector translation = transform.getTranslation(MSpace::kTransform);
	bone.m_LocalTranslation.x(translation.x);
	bone.m_LocalTranslation.y(translation.y);
	bone.m_LocalTranslation.z(translation.z);

	// Set the inverted bind pose values.
	transform.getRotationQuaternion(qx, qy, qz, qw, MSpace::kWorld);
	bone.m_BindRot.Set(qx, qy, qz, qw);
	bone.m_BindRot.Inverse();
	translation = transform.getTranslation(MSpace::kWorld);
	GVector3 transl(translation.x, translation.y, translation.z);
	bone.m_BindTranslation = -transl;

	m_Skeleton.m_RefBones.Push(bone);

	for (unsigned int i = 0; i < i_path.childCount(); ++i)
	{
		if (i_path.child(i).hasFn(MFn::kJoint))
		{
			MDagPath childPath;
			childPath.set(i_path);
			childPath.push(i_path.child(i));
			m_Skeleton.m_RefBones[bone.m_BoneId].m_Children[i] = m_Skeleton.m_RefBones.Count();
			m_Skeleton.m_RefBones[bone.m_BoneId].m_ChildCount++;
			Extract(childPath, bone.m_BoneId );
		}
	}
	return true;
}

bool GMayaSkeleton::Extract()
{
	MStatus status;
	for (MItDag dagIt(MItDag::kDepthFirst, MFn::kJoint, &status);
		!dagIt.isDone();
		dagIt.next())
	{
		MDagPath i_path;
		dagIt.getPath(i_path);
		int parent = -1;
		Extract( i_path, parent );

		// children have all been evaluated.
		dagIt.prune();
	}

	if (m_Skeleton.m_RefBones.Count() > 0)
		return true;
	else
		return false;
}