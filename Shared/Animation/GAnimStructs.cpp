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
		//break;
	}
}

void GAnimTrack::GetKeyTranslationIndices(u32& i_lowerKey, u32& i_upperKey, float i_time)
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
		//break;
	}
}
