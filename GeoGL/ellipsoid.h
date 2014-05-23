#pragma once

#include "main.h"

class Ellipsoid
{
private:
	double a,a2; //semi major radius and the same squared
	double b,b2;
	double c,c2;
public:
	Ellipsoid(void);
	Ellipsoid(double semimajor, double semiminor, double semiminor2);
	~Ellipsoid(void);
	glm::vec3 toVector(double lon, double lat);
	double heightAboveSurfaceAtPoint(glm::dvec3 P);
	double A() { return a; }
	double B() { return b; }
	double C() { return c; }
};

