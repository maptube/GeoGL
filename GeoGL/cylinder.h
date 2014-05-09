#pragma once
//#include "object3d.h"
#include "opengl4.h"
#include "mesh.h"

/**
* 
*/
class Cylinder : public Mesh {
public:
	Cylinder(float Length,float Radius,unsigned int NumSegments);
};