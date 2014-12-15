#include "GAnimStructs.h"

void GAnimTrack::GetKeyRotationIndices(u32& i_lowerKey, u32& i_upperKey, float i_time)
{
	i_lowerKey = 0;
	i_upperKey = m_RotKeys.Count() - 1;

	while (i_lowerKey<i_upperKey - 1)
	{
		int middle = (i_lowerKey + i_upperKey) / 2;

		if (i_time >= m_RotKeys[middle].m_Time)
			i_lowerKey = middle;
		else
			i_upperKey = middle;
	}
}

void GAnimTrack::GetKeyTranslationIndices(u32& i_lowerKey, u32& i_upperKey, float i_time)
{
	i_lowerKey = 0;
	i_upperKey = m_RotKeys.Count() - 1;

	while (i_lowerKey<i_upperKey - 1)
	{
		int middle = (i_lowerKey + i_upperKey) / 2;

		if (i_time >= m_TranslationKeys[middle].m_Time)
			i_lowerKey = middle;
		else
			i_upperKey = middle;
	}
}

void GAnimTrack::Serialize(FILE* o_file)
{
	m_RotKeys.Serialize(o_file);
	m_TranslationKeys.Serialize(o_file);
}

void GAnimTrack::DeSerialize(FILE* i_file)
{
	m_RotKeys.DeSerialize(i_file);
	m_TranslationKeys.DeSerialize(i_file);
}
