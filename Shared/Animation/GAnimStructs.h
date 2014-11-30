#ifndef __GANIMSTRUCTS_H_
#define __GANIMSTRUCTS_H_

#include "Math/GTransform.h"
#include "Containers/GArray.h"

// Keys for the translation track.
class GTranslationKey
{
public:
	GTranslationKey(){}
	GTranslationKey(GVector3& i_trans, float i_time)
	: m_Translation(i_trans), m_Time(i_time)
	{
	}
	GVector3	m_Translation;
	float		m_Time;
};

// Keys for the rotation track.
class GRotationKey
{
public:
	GRotationKey(){}
	GRotationKey(GQuat& i_rot, float i_time)
	: m_Rotation(i_rot), m_Time(i_time)
	{
	}
	GQuat	m_Rotation;
	float	m_Time;
};

// A "track" is a list of all rotatons and translation key frames that exist on a bone.
class GAnimTrack
{
public:
	GAnimTrack() {}
	GArray<GRotationKey>	m_RotKeys;
	GArray<GTranslationKey>	m_TranslationKeys;
	void GetKeyRotationIndices( u32& i_keyOne, u32& i_keyTwo, float i_time );
	void GetKeyTranslationIndices(u32& i_keyOne, u32& i_keyTwo, float i_time );
};
#endif