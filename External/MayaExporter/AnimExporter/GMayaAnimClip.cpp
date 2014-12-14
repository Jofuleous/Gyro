#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MAnimControl.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <maya/MObjectArray.h>
#include <maya/MItKeyframe.h>
#include <maya/MVector.h>
#include <maya/MAnimUtil.h>
#include <maya/MAnimControl.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>

#include "GMayaAnimClip.h"
#include "Containers/GArray.h"

static float RightToLeftMatData2[4][4] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, -1, 0,
	0, 0, 0, 1,
};


bool GMayaAnimClip::ProcessAnim( )
{
	m_BoneCount = -1;
	m_Clip.m_ClipLength = 0.0f;

	MStatus status;
	for (MItDag dagIt(MItDag::kDepthFirst, MFn::kJoint, &status);
		!dagIt.isDone();
		dagIt.next())
	{
		MDagPath i_path;
		dagIt.getPath(i_path);
		int parent = -1;

		ProcessJoints(i_path);

		// children have all been evaluated.
		dagIt.prune();
	}

	if (m_Clip.m_Tracks.Count() <= 0)
		return false;
	
	return true;
	
}

void GMayaAnimClip::ProcessJoints( MDagPath i_path )
{
	m_BoneCount++;
	if (MAnimUtil::isAnimated( i_path ))
		GetJointAnims( i_path );

	for (unsigned int i = 0; i < i_path.childCount(); ++i)
	{
		if (i_path.child(i).hasFn(MFn::kJoint))
		{
			MDagPath childPath;
			childPath.set( i_path );
			childPath.push( i_path.child(i) );
			ProcessJoints( childPath );
		}
	}
}

void GMayaAnimClip::GetJointAnims( const MDagPath& jointPath )
{
	// extract the key-frame times
	MPlugArray animatedPlugs;
	MAnimUtil::findAnimatedPlugs(jointPath, animatedPlugs);
	MTime currentTime = MAnimControl::animationStartTime();
	MMatrix rightToLeft( RightToLeftMatData2 );

	const char* pathName = jointPath.fullPathName().asChar();

	if (animatedPlugs.length() > 0)
	{
		u32 trackIndex = m_Clip.CreateTrack( m_BoneCount );
		MObjectArray curves;
		// if any curve of this track is keyed, they're all gonna get that key, so long as the spline extends as far as other channel splines.
		// so we only need to sample one curve with this constraint.  i hope the iter is in order.  if not, we have to get fancier...
		MAnimUtil::findAnimation(animatedPlugs[0], curves);
		MItKeyframe iter(curves[0]);

		while (!iter.isDone())
		{
			MTime frame = iter.time();
			MAnimControl::setCurrentTime(frame);
			MFnTransform transform( jointPath );

			MQuaternion mQuat;
			MMatrix mat = rightToLeft * transform.transformation().asMatrix() * rightToLeft;
			mQuat = mat;
			GQuat quat(mQuat.x, mQuat.y, mQuat.z, mQuat.w);
			MVector translation = transform.getTranslation( MSpace::kTransform );
			GVector3 vTrans(translation.x, translation.y, translation.z);
			m_Clip.PushRotKeyFrame( trackIndex, quat, (float) frame.value() ); // only push joint anims for now...
			//m_Clip->PushTransKeyFrame(vTrans, frame, (float)frame.value()); // push translation and rotation for now :(
			if ( frame.value() > m_Clip.m_ClipLength )
				m_Clip.m_ClipLength = frame.value();

			iter.next();
		}
	}
}