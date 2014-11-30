#ifndef __GMAYAANIM_H_
#define __GMAYAANIM_H_

// This class build an anim clip given the Maya DAG.
#include <maya/MDagPath.h>

class GAnimClip;

class GMayaAnimClip
{
public:
	bool		ProcessAnim();
	void		GetJointAnims( const MDagPath& jointPath );
	void		ProcessJoints( MDagPath i_path );

	u32			m_BoneCount;
	GAnimClip*	m_Clip;
};

#endif