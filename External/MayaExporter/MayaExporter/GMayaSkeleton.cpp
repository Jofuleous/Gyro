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
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include "d3dx9.h"


static float RightToLeftMatData[4][4] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, -1, 0,
	0, 0, 0, 1,
};

bool GMayaSkeleton::Extract( MDagPath i_path, int i_parent )
{
	GAnimClip clip;

	GAnimBone bone;
	bone.m_ChildCount = 0;
	bone.m_BoneId = m_Skeleton.m_RefBones.Count();
	bone.m_ParentId = i_parent;
	MFnTransform transform( i_path );
	double qx, qy, qz, qw;

	MMatrix rightToLeft(RightToLeftMatData);
	MMatrix localTransform = rightToLeft * transform.transformation().asMatrix() * rightToLeft;
	MMatrix inverseWorldTransform = rightToLeft * i_path.inclusiveMatrixInverse() * rightToLeft;
	MQuaternion localQuat;
	localQuat = localTransform;
	double* localTranslation = localTransform[3];
	MQuaternion inverseWorldQuat;
	inverseWorldQuat = inverseWorldTransform;
	double* inverseWorldTranslation = inverseWorldTransform[3];

	bone.m_LocalRot.Set(localQuat.x, localQuat.y, localQuat.z, localQuat.w);
	bone.m_LocalTranslation.x(localTranslation[0]);
	bone.m_LocalTranslation.y(localTranslation[1]);
	bone.m_LocalTranslation.z(localTranslation[2]);

	bone.m_BindRot.Set(inverseWorldQuat.x, inverseWorldQuat.y, inverseWorldQuat.z, inverseWorldQuat.w );
	bone.m_BindTranslation.x(inverseWorldTranslation[0]);
	bone.m_BindTranslation.y(inverseWorldTranslation[1]);
	bone.m_BindTranslation.z(inverseWorldTranslation[2]);

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
		// We have to rotate about Y to put this in left handed system...
		
		/*
		GQuat Rot180;
		Rot180.FromAngleAxis(GMath::Pi, GVector3(0.0f, 1.0f, 0.0f));
		MQuaternion quat(Rot180.m_X, Rot180.m_Y, Rot180.m_Z, Rot180.m_W);
		MFnTransform transform(i_path);
		transform.rotateBy(quat, MSpace::kTransform);
		*/

		Extract(i_path, parent);

		/*
		Rot180.FromAngleAxis(-GMath::Pi, GVector3(0.0f, 1.0f, 0.0f));
		MQuaternion reQuat(Rot180.m_X, Rot180.m_Y, Rot180.m_Z, Rot180.m_W);
		transform.rotateBy(reQuat, MSpace::kTransform);
		*/
	

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
				if (m_influences[pointCounter].size() > 4)
				{
					int highest = -1;
					float lastHighest = 0.0f;
					for (int k = 0; k < m_influences[pointCounter].size(); k++)
					{
						if (m_influences[pointCounter][k].m_Weight < mayaWeightArray[j] && lastHighest < m_influences[pointCounter][k].m_Weight)
						{
							lastHighest = m_influences[pointCounter][k].m_Weight;
							highest = k;
						}
					}
					if (highest >= 0)
					{
						m_influences[pointCounter][highest].m_Weight = mayaWeightArray[j];
						m_influences[pointCounter][highest].m_BoneId = infIndexToBoneID[j];
					}
				}
				else
				{
					GMayaInfluence influence;
					influence.m_BoneId = infIndexToBoneID[j];
					influence.m_Weight = mayaWeightArray[j];
					m_influences[pointCounter].push_back(influence);
				}
			}
		}
	}

	int infSize = m_influences.size();
	int infOneSize = m_influences[0].size();

	delete infIndexToBoneID;
}