#include "GMayaSkeleton.h"
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MFnTransform.h>
#include <maya/MVector.h>
#include "Animation/GAnimClip.h"
#include <maya/MFnMesh.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MItGeometry.h>
#include <maya/MFloatArray.h>

bool GMayaSkeleton::Extract( MDagPath i_path, int i_parent )
{
	GAnimClip clip;

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
	MStatus status;
	const char* pathName = i_path.fullPathName(&status).asChar();

	// Set the inverted bind pose values.
	transform.getRotationQuaternion(qx, qy, qz, qw, MSpace::kWorld);
	bone.m_BindRot.Set(qx, qy, qz, qw);
	bone.m_BindRot.Inverse();
	translation = transform.getTranslation(MSpace::kWorld);
	GVector3 transl(translation.x, translation.y, translation.z);
	bone.m_BindTranslation = -transl;

	m_Skeleton.m_RefBones.Push(bone);
	m_bonePaths.push_back(i_path);

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

bool GMayaSkeleton::ExtractSkinningData()
{
	MStatus status;

	MItDag i(MItDag::kDepthFirst, MFn::kMesh);
	// Get the first mesh.  If exporting is occuring properly, there should only ever be one mesh.
	while (!i.isDone())
	{
		if (i.getType(&status) != MFn::kJoint)
			break;
	}
	if (i.isDone(&status))
		return false;

	MFnMesh mesh(i.item());
	MDagPath meshDagPath;
	i.getPath(meshDagPath);

	MItDependencyNodes depNodeIt(MFn::kSkinClusterFilter);
	if (depNodeIt.isDone())
		return false;
	MObject depNodeObject = depNodeIt.item();

	MFnSkinCluster skinCluster( depNodeObject, &status );
	int shapeIndex = skinCluster.indexForOutputShape( meshDagPath.node(), &status );

	// make the influence index id to bone index map!
	int* infIndexToBoneID = new int[m_bonePaths.size()];
	for (int i = 0; i < m_bonePaths.size(); i++)
	{
		unsigned int index = skinCluster.indexForInfluenceObject(m_bonePaths[i], &status);
		infIndexToBoneID[index] = i;
	}

	MObject mayaInputObject = skinCluster.inputShapeAtIndex( shapeIndex, &status );
	// numVertices() should be the same for input and output shape.
	int numVertices = mesh.numVertices();
	m_influences.resize( numVertices );

	int pointCounter = 0;
	for (MItGeometry geometryIt(mayaInputObject);
		!geometryIt.isDone();
		geometryIt.next(), ++pointCounter)
	{
		MObject mayaComponent = geometryIt.component();
		MFloatArray mayaWeightArray;
		unsigned int numInfluences; // is going to be discarded

		skinCluster.getWeights( meshDagPath,
			mayaComponent,
			mayaWeightArray,
			numInfluences);

		// put them into a more convenient format
		for (int j = 0; j < mayaWeightArray.length(); ++j)
		{
			// I know: we are working with floats, but Maya has a
			// function to eliminate weights, that are too small
			if (mayaWeightArray[j] != 0)
			{
				GMayaInfluence influence;
				influence.m_BoneId = infIndexToBoneID[j];
				influence.m_Weight = mayaWeightArray[j];
				m_influences[pointCounter].push_back(influence);
			}
		}
	}

	int infSize = m_influences.size();
	int infOneSize = m_influences[0].size();

	delete infIndexToBoneID;
}