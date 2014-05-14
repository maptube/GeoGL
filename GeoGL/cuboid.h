#pragma once
//#include "object3d.h"
//#include "opengl4.h"
#include "mesh2.h"

/// <summary>
/// Cuboid mesh
/// </summary>
class Cuboid : public Mesh2 {
public:
	Cuboid(float SX,float SY,float SZ);
	~Cuboid();
};