#pragma once

#include "main.h"

class Ellipsoid
{
private:
	double a,a2,ra2; //semi major radius and the same squared and one over squared (reciprical)
	double b,b2,rb2;
	double c,c2,rc2;
public:
	Ellipsoid(void);
	Ellipsoid(double semimajor, double semiminor, double semiminor2);
	~Ellipsoid(void);
	glm::vec3 toVector(double lon, double lat, double geodeticHeight);
	double heightAboveSurfaceAtPoint(glm::dvec3 P);
	double A() { return a; }
	double B() { return b; }
	double C() { return c; }
	glm::vec3 OneOverRadiiSquared() { return glm::vec3(ra2,rb2,rc2); }
};

