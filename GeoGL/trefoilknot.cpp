
#include "trefoilknot.h"

using namespace std;

/**
* Trefoil knot made from a TubeGeometry.
* Not particularly useful for GIS, but good for testing tube geometry generation.
* Radius is the radius of the tube formed around the knot spline
* NumSegments is the number of circle segments used to from the tube
*/
TrefoilKnot::TrefoilKnot(float Radius, int NumSegments)
{
	//trefoil knot based on a parametric curve in t
	//TODO: might want to tweak the t+= increment in the for loop - originally was 0.2,
	//but set it too low with high NumSegments and you get performance problems
	//TODO 2: the 6.30 is a fiddle to make the two bits join up
	vector<glm::vec3> spline;
	vector<glm::vec3> colours;
	colours.push_back(glm::vec3(1.0,0,0));
	colours.push_back(glm::vec3(0,0,1.0));
	for (float t=0; t<=6.30f; t+=0.1f) { //t=0..2PI
		glm::vec3 P;
		P.x=sin(t)+2*sin(2*t); P.y=cos(t)-2*cos(2*t); P.z=-sin(3*t);
		//another possibility is (2+cos(3t))*cos(2*t), (2+cos(3*t))*sin(2*t)), sin(3*t)
		spline.push_back(P);
	}
	AddSpline(spline,colours);
	GenerateMesh(Radius,NumSegments);

	CreateBuffers();
}


TrefoilKnot::~TrefoilKnot(void)
{
}
