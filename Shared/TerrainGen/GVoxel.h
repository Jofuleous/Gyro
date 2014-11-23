#ifndef __GCHUNK_H_
#define __GCHUNK_H_

class GVoxel
{
public:
	enum Type
	{
		AIR,
		GROUND
	};

	Type m_type;
};

#endif