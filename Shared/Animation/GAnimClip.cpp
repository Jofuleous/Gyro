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

void GAnimClip::UpdateSkeletonInstace( GSkeletonInstance* o_instance, float i_time )
{
	for ( int i = 0; i < m_Tracks.Count(); i++)
	{
		u32 keyOne, keyTwo;
		m_Tracks[i].GetKeyRotationIndices(keyOne, keyTwo, i_time);

		if (keyOne != keyTwo)
		{
			float alpha = (i_time - m_Tracks[i].m_RotKeys[keyOne].m_Time) / (m_Tracks[i].m_RotKeys[keyTwo].m_Time - m_Tracks[i].m_RotKeys[keyOne].m_Time);
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalRot = o_instance->m_Bones[m_TrackToBone[i]].m_LocalRot.Slerp(m_Tracks[i].m_RotKeys[keyOne].m_Rotation, m_Tracks[i].m_RotKeys[keyTwo].m_Rotation, alpha);
		}
		else
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalRot = m_Tracks[i].m_RotKeys[keyOne].m_Rotation;

		// we can probably just use key one, key two, and alpha if we keep our sampling method...until then, search again.
		m_Tracks[i].GetKeyTranslationIndices(keyOne, keyTwo, i_time);
		if (keyOne != keyTwo)
		{
			// potentially smooth step here.
			float alpha = GMath::SmoothStep(m_Tracks[i].m_TranslationKeys[keyOne].m_Time, m_Tracks[i].m_TranslationKeys[keyTwo].m_Time, i_time);
			//float alpha = (i_time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time) / (m_Tracks[i].m_TranslationKeys[keyTwo].m_Time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time);
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalTranslation = GVector3::Lerp( m_Tracks[i].m_TranslationKeys[keyOne].m_Translation, m_Tracks[i].m_TranslationKeys[keyTwo].m_Translation, alpha);
		}
		else
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalTranslation = m_Tracks[i].m_TranslationKeys[keyOne].m_Translation;
	}
}
