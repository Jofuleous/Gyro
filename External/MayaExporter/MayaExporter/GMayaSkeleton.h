#ifndef __GMAYA_SKELETON_H_
#define __GMAYA_SKELETON_H_

#include "Animation/GSkeleton.h"
#include <maya/MDagPath.h>
class GMayaSkeleton
{
public:
	GMayaSkeleton() {}
	bool	Extract();
	bool	Extract(MDagPath i_path, int i_parent );

	GSkeleton m_Skeleton;
};

#endif