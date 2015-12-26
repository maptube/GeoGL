
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
	//fourth power
	a4=a2*a2;
	b4=b2*b2;
	c4=c2*c2;
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
	a4=a2*a2;
	b4=b2*b2;
	c4=c2*c2;
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
/// Scale a geocentric point to the surface of the ellipsoid (along the geocentric normal)
/// </summary>
glm::dvec3 Ellipsoid::ScaleToGeocentricSurface(glm::dvec3 P)
{
	double beta = 1.0/glm::sqrt(
			(P.x*P.x) * ra2 +
			(P.y*P.y) * rb2 +
			(P.z*P.z) * rc2
		);
	return beta*P;
}

/// <summary>
/// Cozzi and Ring method using Newton Raphson from 3D Engine Design for Virtual Globes book
/// </summary
glm::dvec3 Ellipsoid::ScaleToGeodeticSurface(glm::dvec3 P)
{
	double beta = 1.0/glm::sqrt(
			(P.x*P.x) * ra2 +
			(P.y*P.y) * rb2 +
			(P.z*P.z) * rc2
			);
	double n = glm::length(glm::dvec3(beta*P.x*ra2,beta*P.y*rb2,beta*P.z*rc2));
	double alpha = (1.0-beta)*(glm::length(P)/n);

	double x2=P.x*P.x;
	double y2=P.y*P.y;
	double z2=P.z*P.z;

	double da=0.0;
	double db=0.0;
	double dc=0.0;
	double s=0.0;
	double dSdA=1.0;

	do {
		alpha-=(s/dSdA);
		da=1.0+(alpha*ra2);
		db=1.0+(alpha*rb2);
		dc=1.0+(alpha*rc2);

		double da2=da*da;
		double db2=db*db;
		double dc2=dc*dc;

		double da3=da*da2;
		double db3=db*db2;
		double dc3=dc*dc2;

		s=x2/(a2*da2)+y2/(b2*db2)+z2/(c2*dc2)-1.0;

		dSdA=-2.0*(x2/(a4*da3)+y2/(b4*db3)+z2/(c4*dc3));

	} while (abs(s)>1e-10);

	return glm::dvec3(P.x/da,P.y/db,P.z/dc);
}

/// <summary>
/// Convert a point on the surface of the ellipsoid into the Geodetic surface normal at that point
/// </summary>
glm::dvec3 Ellipsoid::GeodeticSurfaceNormal(glm::dvec3 P)
{
	glm::dvec3 Normal(P.x*ra2,P.y*rb2,P.z*rc2);
	Normal = glm::normalize(Normal);
	return Normal;
}

/// <summary>
/// Given a point P on the surface of the ellipsoid, return the lat lon coordinates (radians)
/// </summary>
glm::dvec2 Ellipsoid::ToGeodetic2D(glm::dvec3 P)
{
	glm::dvec3 n = GeodeticSurfaceNormal(P);
	glm::dvec2 LL(glm::atan(n.y,n.x),glm::asin(n.z/glm::length(n)));
	return LL;
}

/// <summary>
/// Return the lon, lat, height of an arbitrary point
/// </summary>
glm::dvec3 Ellipsoid::ToGeodetic3D(glm::dvec3 Position)
{
	glm::dvec3 P = ScaleToGeodeticSurface(Position);
	glm::dvec3 H = Position-P;
	double height = glm::sign(glm::dot(H,Position))*glm::length(H);
	glm::dvec2 Geodetic2D = ToGeodetic2D(P);
	return glm::dvec3(Geodetic2D.x,Geodetic2D.y,height);
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
