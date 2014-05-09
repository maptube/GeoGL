#pragma once

#include "mesh.h"

/// <summary>
/// 3D Sierpinski pyramid fractal
/// </summary>
class SierpinskiPyramid : public Mesh
{
public:
	int PyramidCount; //count of how many pyramids have been created
	SierpinskiPyramid(float Size, unsigned int Depth);
	~SierpinskiPyramid(void);
private:
	void MakePyramids(float Size,unsigned int Depth,glm::vec3 Pc);
};

