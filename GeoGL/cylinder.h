#pragma once
//#include "object3d.h"
//#include "opengl4.h"
#include "mesh2.h"

/**
* 
*/
class Cylinder : public Mesh2 {
public:
	Cylinder(float Length,float Radius,unsigned int NumSegments);
};