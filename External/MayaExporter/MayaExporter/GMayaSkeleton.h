#ifndef __GMAYA_SKELETON_H_
#define __GMAYA_SKELETON_H_

#include "Animation/GSkeleton.h"
#include <maya/MDagPath.h>
#include <vector>

class GMayaInfluence
{
public:
	int		m_BoneId;
	float	m_Weight;
};

class GMayaSkeleton
{
public:
	GMayaSkeleton() {}
	bool	Extract();
	bool	Extract(MDagPath i_path, int i_parent );
	bool	ExtractSkinningData();

	std::vector<MDagPath>	m_bonePaths;
	std::vector<std::vector<GMayaInfluence>>	m_influences;
	GSkeleton m_Skeleton;
};

#endif