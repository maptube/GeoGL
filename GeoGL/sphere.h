#pragma once

//http://paulbourke.net/geometry/circlesphere/

//#include "opengl4.h"
//#include "mesh.h"
#include "mesh2.h"

class Sphere : public Mesh2
{
public:
	Sphere(double Radius, int WidthSegments, int HeightSegments);
	Sphere(double A, double B, double C, int WidthSegments, int HeightSegments);
	//~Sphere();
private:
	void init(double A, double B, double C, int WidthSegments, int HeightSegments);
	void init2(double A, double B, double C, int WidthSegments, int HeightSegments); //second attempt at making init faster
};