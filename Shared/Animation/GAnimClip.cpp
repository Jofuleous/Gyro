#include "GAnimClip.h"

void GAnimClip::Serialize( FILE* o_file )
{
	// write out the track to bone map.  the count tells us how many tracks there are.

	// CLIP LENGTH
	fwrite(&this->m_ClipLength, sizeof(float), 1, o_file);

	m_TrackToBone.Serialize(o_file);
	for (int i = 0; i < m_Tracks.Count(); i++)
		m_Tracks[i].Serialize(o_file);
}

void GAnimClip::DeSerialize( FILE* i_file )
{
	// CLIP LENGTH
	fread(&this->m_ClipLength, sizeof(float), 1, i_file);

	// TRACK 2 BONE
	m_TrackToBone.DeSerialize(i_file);

	m_Tracks.Resize(m_TrackToBone.Count());
	for (int i = 0; i < m_TrackToBone.Count(); i++)
	{
		m_Tracks[i].DeSerialize(i_file);
		m_Tracks.SetCount(i);
	}
}

u32 GAnimClip::CreateTrack( u32 i_boneId )
{
	m_Tracks.Push(GAnimTrack());
	m_TrackToBone.Push( i_boneId );
	return m_Tracks.Count() - 1;
}

void GAnimClip::PushRotKeyFrame(u32 i_track, GQuat& i_quat, float i_time)
{
	m_Tracks[i_track].m_RotKeys.Push(GRotationKey(i_quat, i_time));
}

void GAnimClip::PushTransKeyFrame(u32 i_track, GVector3& i_trans, float i_time)
{
	m_Tracks[i_track].m_TranslationKeys.Push(GTranslationKey(i_trans, i_time));
}

