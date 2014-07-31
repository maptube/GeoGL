
#include "ellipsoid.h"

#include <math.h>

Ellipsoid::Ellipsoid(void)
{
	//WGS84 ellipsoid
	a=6378137.0;
	b=a;
	c=6356752.314245;
	//squared
	a2=a*a;
	b2=b*b;
	c2=c*c;
	//recipricals
	ra2=1/a2;
	rb2=1/b2;
	rc2=1/c2;
}

Ellipsoid::Ellipsoid(double semimajor, double semiminor, double semiminor2)
{
	a=semimajor;
	b=semiminor;
	c=semiminor2;
	a2=a*a;
	b2=b*b;
	c2=c*c;
	ra2=1/a2;
	rb2=1/b2;
	rc2=1/c2;
}


Ellipsoid::~Ellipsoid(void)
{
}

/// <summary>
/// Convert a latitude and longitude into Cartesian coordinates
/// </summary>
/// <param name="lon">Geodetic longitude in radians</param>
/// <param name="lat">Geodetic latitude in radians</param>
/// <param name="geodeticHeight">Above ellipsoid in metres</param>
/// <returns>Cartesian position on the spheriod in metres</returns>
glm::vec3 Ellipsoid::toVector(double lon, double lat, double geodeticHeight)
{
	//work out normal position on surface of unit sphere using Euler formula and lat/lon
	//The latitude in this case is a geodetic latitude, so it's defined as the angle between the equatorial plane and the surface normal.
	//This is why the following works.
	double CosLat = cos(lat);
	glm::vec3 n(CosLat*cos(lon),CosLat*sin(lon),sin(lat));
	//so (nx,ny,nz) is the geodetic surface normal i.e. the normal to the surface at lat,lon

	//using |ns|=gamma * ns, find gamma ( where |ns| is normalised ns)
	//with ns=Xs/a^2 i + Ys/b^2 j + Zs/c^2 k
	//equation of ellipsoid Xs^2/a^2 + Ys^2/b^2 + Zs^2/c^2 = 1
	//So, basically, I've got two equations for the geodetic surface normal that are related by a linear factor gamma

	glm::vec3 k(a2,b2,c2);
	k=k*n;

	double gamma = sqrt(k.x*n.x+k.y*n.y+k.z*n.z);
	glm::vec3 rSurface(k.x/gamma,k.y/gamma,k.z/gamma);

	//NOTE: you do rSurface = rSurface + (geodetic.height * n) to add the height on if you need it
	rSurface= rSurface + glm::vec3(geodeticHeight*n.x,geodeticHeight*n.y,geodeticHeight*n.z);

	return rSurface;
}

/// <summary>
/// Calculates the distance between a point in space (P) and the nearest point on the ellipsoid surface. Returns height above the surface point.
/// TODO: there must be a better way of simplifying the maths if we only want the height. Looks a little long winded to me.
/// </summary>
/// <param name="P">Point above the ellipsoid in cartesian coordinates</param>
/// <returns>The distance between the point and the closest point on the sphere</returns>
double Ellipsoid::heightAboveSurfaceAtPoint(glm::dvec3 P)
{
	//If the eye vector (P) is a ray from the origin to P, then the ray is Lambda * P and we need to find Lambda by putting (L.Px, L.Py, L.Pz) into X^2/a^2+Y^2/b^2+Z^2/c^2 = 1
	//Having got Lambda, put it back into S=Lambda * P to calculate the surface point intersection with the ray from the eye through the origin. Then simply subtract h= |P| - |S|
	//to calculate the height as the distance from eye to origin minus surface point to origin.
	glm::dvec3 k(P.x*P.x/a2,P.y*P.y/b2,P.z*P.z/c2);
	double Lambda2 = 1/(k.x+k.y+k.z);
	double Lambda = sqrt(Lambda2);

	glm::dvec3 S(Lambda*P.x,Lambda*P.y,Lambda*P.z); // S=Lambda * P to calculate the surface point S

	//so the distance is O->P minus O->S, which are the two radii
	return glm::length(P) - glm::length(S);
}
