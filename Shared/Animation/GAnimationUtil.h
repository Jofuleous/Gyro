#ifndef __GANIMATION_UTIL_H_
#define __GANIMATION_UTIL_H_

#include "GAnimClip.h"
namespace GAnimationUtil
{
	GAnimClip* LoadAnimClip( const char* i_fileName )
	{
		char* filepath = (char*)malloc(128);
		sprintf(filepath, "data/Anims/%s", i_fileName );
		FILE* file = fopen(filepath, "r+b");

		GAnimClip* clip = new GAnimClip();
		clip->DeSerialize(file);
		return clip;
	}
}

#endif