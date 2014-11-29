#ifndef __GANIMATION_CLIP_H_
#define __GANIMATION_CLIP_H_

#include "Math/GTransform.h"
#include "Containers/GArray.h"
#include "Containters/GHashMap.h"

// Keys for the translation track.
class GTranslationKeys
{
	GArray<GVector3>	m_Translations;
	GArray<float>		m_Times;
};

// Keys for the rotation track.
class GRotationKeys
{
	GArray<GQuat>	m_Rotations;
	GArray<float>	m_Times;
};

class GAnimTrack
{
	GArray<GRotationKeys>		m_RotTracks;
	GArray<GTranslationKeys>	m_TransTracks;
};

class GAnimClip
{
	GArray<GAnimTrack>			m_Tracks;
	char						m_ClipName[64];
	void						Serialize();
};

#endif