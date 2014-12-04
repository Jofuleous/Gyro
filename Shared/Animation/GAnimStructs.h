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
	GAnimTrack(const GAnimTrack& i_other) { Copy(i_other); }
	void GetKeyRotationIndices( u32& i_keyOne, u32& i_keyTwo, float i_time );
	void GetKeyTranslationIndices(u32& i_keyOne, u32& i_keyTwo, float i_time );
	void Serialize(FILE* o_file);
	void DeSerialize(FILE* o_file);
	void Copy( const GAnimTrack& i_other)
	{
		// Do array deep copy.
		m_RotKeys = i_other.m_RotKeys;
		m_TranslationKeys = i_other.m_TranslationKeys;
	}
	GAnimTrack& operator=(const GAnimTrack& i_other)
	{
		Copy(i_other);
		return *this;
	}
};
#endif