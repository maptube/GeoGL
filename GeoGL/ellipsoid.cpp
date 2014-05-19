
#include "ellipsoid.h"

#include <math.h>

Ellipsoid::Ellipsoid(void)
{
	//WGS84 ellipsoid
	a=6378137.0;
	b=a;
	c=6356752.314245;
	a2=a*a;
	b2=b*b;
	c2=c*c;
}

Ellipsoid::Ellipsoid(double semimajor, double semiminor, double semiminor2)
{
	a=semimajor;
	b=semiminor;
	c=semiminor2;
	a2=a*a;
	b2=b*b;
	c2=c*c;
	//TODO: cache the reciprical of the squares
}


Ellipsoid::~Ellipsoid(void)
{
}

/// <summary>
/// Convert a latitude and longitude into Cartesian coordinates
/// </summary>
/// <param name="lon">Geodetic longitude in radians</param>
/// <param name="lat">Geodetic latitude in radians</param>
/// <returns>Cartesian position on the spheriod in metres</returns>
glm::vec3 Ellipsoid::toVector(double lon, double lat)
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

	return rSurface;
}

/// <summary>
/// Calculates the distance between 
/// </summary>
/// <param name="P">Point above the ellipsoid in cartesian coordinates</param>
/// <returns>The distance between the point and the closest point on the sphere</returns>
double Ellipsoid::heightAboveSurfaceAtPoint(glm::vec3 P)
{
	//If the eye vector (P) is a ray from the origin to P, then the ray is Lambda * P and we need to find Lambda by putting (L.Px, L.Py, L.Pz) into X^2/a^2+Y^2/b^2+Z^2/c^2 = 1
	// (where Lambda is the radius of the ellipsoid at that point).
	glm::vec3 k(P.x*P.x/a2,P.y*P.y/b2,P.z*P.z/c2);
	double r2 = 1/(k.x+k.y+k.z);
	double r = sqrt(r2);

	//so the distance is O->P minus r, which is the radius
	return glm::length(P)-r;
}
