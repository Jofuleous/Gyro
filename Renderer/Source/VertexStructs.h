#ifndef __VERTEXSTRUCTS_H_
#define __VERTEXSTRUCTS_H_

namespace GVertices
{
	//until i have some better way of doing this.
	struct s_vertexColor
	{
		float x, y, z;
		char alpha;
		char red;
		char green;
		char blue;
	};

	struct s_vertexTexture
	{
		float x, y, z;
		float u, v;
	};

	struct s_vertexTextureNormal
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};

	struct s_vertexTNTB //vertex texture, normal, tangent, binormal
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};
}

#endif