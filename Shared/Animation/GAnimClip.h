#ifndef __GANIMCLIP_H_
#define __GANIMCLIP_H_

#include "Containers/GArray.h"
#include "GAnimStructs.h"
#include "GSkeleton.h"


// Put in optimization later 
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
	void	UpdateSkeletonInstace( GSkeletonInstance* o_instance, float i_time ); // This is not efficient or very modular.  hack.
	//void	UpdateSkeletonInstance(GSkeletonInstance* o_instance, float i_time);

	GArray<GAnimTrack>	m_Tracks;		// All tracks
	GArray<u32>			m_TrackToBone;	// Track index -> Bone Index
	float				m_ClipLength;
	char				m_Name[64];		// Clip Name
	bool				m_HasTranslation;
	bool				m_HasScale;
	bool				m_HasRotation;
};

#endif