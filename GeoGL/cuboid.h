#pragma once
//#include "object3d.h"
#include "opengl4.h"
#include "mesh.h"

/// <summary>
/// Cuboid mesh
/// </summary>
class Cuboid : public Mesh {
public:
	Cuboid(float SX,float SY,float SZ);
	~Cuboid();
};