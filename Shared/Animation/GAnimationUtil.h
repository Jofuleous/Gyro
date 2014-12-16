#ifndef __GANIMATION_UTIL_H_
#define __GANIMATION_UTIL_H_

#include "GAnimClip.h"

namespace GAnimationUtil
{
	GAnimClip* LoadAnimClip(const char* i_fileName);

	void Evaluate_AnimClip(GSkeletonInstance* io_instance, GAnimClip* i_clip, float i_time);
	// linear blend for the time being...blah.
	void Evaluate_AnimClipBlend(GSkeletonInstance* io_instance, GAnimClip* i_intoClip, GAnimClip* i_outOfClip, float i_intoTime, float i_outOfTime, float i_intoAlpha);
}

#endif