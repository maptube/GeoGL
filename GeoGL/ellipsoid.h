#pragma once

#include "main.h"

class Ellipsoid
{
private:
	double a,a2,a4,ra2; //semi major radius and the same squared, quartic and one over squared (reciprical)
	double b,b2,b4,rb2;
	double c,c2,c4,rc2;
public:
	Ellipsoid(void);
	Ellipsoid(double semimajor, double semiminor, double semiminor2);
	~Ellipsoid(void);
	glm::vec3 toVector(double lon, double lat, double geodeticHeight);
	glm::dvec3 ScaleToGeocentricSurface(glm::dvec3 P);
	glm::dvec3 ScaleToGeodeticSurface(glm::dvec3 P);
	glm::dvec3 GeodeticSurfaceNormal(glm::dvec3 P);
	glm::dvec2 ToGeodetic2D(glm::dvec3 P);
	glm::dvec3 ToGeodetic3D(glm::dvec3 Position);
	double heightAboveSurfaceAtPoint(glm::dvec3 P);
	double A() { return a; }
	double B() { return b; }
	double C() { return c; }
	glm::vec3 OneOverRadiiSquared() { return glm::vec3(ra2,rb2,rc2); }
};

