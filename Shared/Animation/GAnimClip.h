#ifndef __ANIMATION_GANIMCLIP_H_
#define __ANIMATION_GANIMCLIP_H_

//===========================================================================================
// GAnimClip.h
// Author:	Jon Futch
// Created:	December, 2014
//
// Represents an animation for a set of bones.
//
// TODO: Make sure that this is only representing the bones that are keyed in Maya.
// Right now, I think when we "bake" the animations to the rig's bones, we're baking 
// keys to bones that aren't even being animated.  This could be a very big performance
// optimization if we get rid of this.
//===========================================================================================

#include "Containers/GArray.h"
#include "GAnimStructs.h"
#include "GSkeleton.h"

class GAnimClip
{
public:
			GAnimClip() {}

	// Creation helpers
	void	Serialize( FILE* o_file );
	void	DeSerialize( FILE* o_file );
	u32		CreateTrack( u32 i_boneId );
	void	PushRotKeyFrame( u32 i_track, GQuat& i_quat, float i_time );
	void	PushTransKeyFrame(u32 i_track, GVector3& i_trans, float i_time);


	void	GetBoneTranslation( GQuat& o_rot, float i_time );
	void	GetBoneRotation( GQuat& o_rot, float i_time );
	void	UpdateSkeletonInstance(GSkeletonInstance* o_instance, GAnimClip* i_other, float i_time );

	GArray<GAnimTrack>	m_Tracks;		// All tracks
	GArray<u32>			m_TrackToBone;	// Track index -> Bone Index
	float				m_ClipLength;
	char				m_Name[64];		// Clip Name
	bool				m_HasTranslation;
	bool				m_HasScale;
	bool				m_HasRotation;
};

#endif