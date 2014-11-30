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

#include "GMayaAnimClip.h"


bool GMayaAnimClip::ProcessAnim( )
{
	m_BoneCount = -1;

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

			double qx, qy, qz, qw;
			transform.getRotationQuaternion(qx, qy, qz, qw, MSpace::kTransform );
			GQuat quat(qx, qy, qz, qw);
			MVector translation = transform.getTranslation( MSpace::kTransform );
			GVector3 vTrans(translation.x, translation.y, translation.z);
			m_Clip.PushRotKeyFrame( trackIndex, quat, (float) frame.value() ); // only push joint anims for now...
			//m_Clip->PushTransKeyFrame(vTrans, frame, (float)frame.value()); // push translation and rotation for now :(

			iter.next();
		}
	}
}