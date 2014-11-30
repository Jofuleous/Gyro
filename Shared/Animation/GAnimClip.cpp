#include "GAnimClip.h"

void GAnimClip::Serialize( FILE* o_file )
{

}

void GAnimClip::DeSerialize( FILE* o_file )
{

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
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalRot.Slerp(m_Tracks[i].m_RotKeys[keyOne].m_Rotation, m_Tracks[i].m_RotKeys[keyTwo].m_Rotation, alpha);
		}
		else
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalRot = m_Tracks[i].m_RotKeys[keyOne].m_Rotation;


		m_Tracks[i].GetKeyTranslationIndices(keyOne, keyTwo, i_time);
		if (keyOne != keyTwo)
		{
			float alpha = (i_time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time) / (m_Tracks[i].m_TranslationKeys[keyTwo].m_Time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time);
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalTranslation = GVector3::Lerp( m_Tracks[i].m_TranslationKeys[keyOne].m_Translation, m_Tracks[i].m_TranslationKeys[keyTwo].m_Translation, 1.0f - alpha);
		}
		else
			o_instance->m_Bones[m_TrackToBone[i]].m_LocalTranslation = m_Tracks[i].m_TranslationKeys[keyOne].m_Translation;
	}
}