#pragma once

//http://paulbourke.net/geometry/circlesphere/

//#include "opengl4.h"
//#include "mesh.h"
#include "mesh2.h"

class Sphere : public Mesh2
{
public:
	Sphere(float Radius, int WidthSegments, int HeightSegments);
	Sphere(double A, double B, double C, int WidthSegments, int HeightSegments);
	~Sphere();
};