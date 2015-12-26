#include "tubegeometry.h"

using namespace std;

/*
* Create a tube which is centred on a spline, or create multiple tubes centred on each spline in a list.
*/
TubeGeometry::TubeGeometry(void)
{
}


TubeGeometry::~TubeGeometry(void)
{
}

/**
* TubeGeometry::AddSpline
* @param points The points along the spline to be added as a tube mesh
* @param point_colours The colour of the tube at this point along the spline. This does mean that you can't have a stripey tube though, as the colour must
*						be the same around the circumference at this spline point. There do not have to be as many colours as there are spline vertex
*						points as they get repeated as necessary, so you can pass a single colour for the whole segment.
*/
void TubeGeometry::AddSpline(vector<glm::vec3> points, vector<glm::vec3> point_colours)
{
	splines.push_back(points);
	spline_colours.push_back(point_colours);
}

/**
* Make mesh geometry from currently loaded splines based in a radius and number of segments (points around circle at each vertex).
see: http://paulbourke.net/geometry/circlesphere/ (creating a plane disk segment perpendicular to a line)
TODO: this isn't even close to being finished - Frenet-Serret formula (as used in THREE.js)
http://www.lab4games.net/zz85/blog/2012/04/24/spline-extrusions-tubes-and-knots-of-sorts/
http://facstaff.unca.edu/mcmcclur/class/Fall11CalcIII/handouts/TubesAndTNB.pdf
this is good on moving frames of reference: http://www.cs.cmu.edu/afs/andrew/scs/cs/15-462/web/old/asst2camera.html
NOTE: the three.js code puts each radius at equidistant points along the spline, not necessarily at the point locations.
This function puts the joints at the point locations.
*
* @param Radius The radius of the generated tube
* @param NumSegments The number of polygon segments around the circumference (i.e. 6 means the tube is an extruded hexagon)
* @param p The points along the centre of the tube spline
* @param colours A list of colours for each point along the spline p.
*
* TODO: you could do a lot more with the colours and allow stripey tubes to be created by passing a per face, per triangle or per vertex list instead of per spline point.
*/
void TubeGeometry::GenerateTube(float Radius, unsigned int NumSegments, std::vector<glm::vec3> p, std::vector<glm::vec3> colours)
{
	//Radius, number of segments to create around circle, list of points forming spline
	//calculate TNB frame along path
	vector<glm::vec3> tangents;
	vector<glm::vec3> normals;
	vector<glm::vec3> binormals;

	//calculate tangents - not quite right yet? - T0=p0->p1, T1=p1->p2 etc
	for (size_t i=1; i<p.size(); i++)
		tangents.push_back(glm::normalize(p.at(i)-p.at(i-1)));
	tangents.push_back(glm::normalize(p.at(p.size()-1)-p.at(p.size()-2))); //push extra copy of final tangent

	//calculate normals and binormals
	glm::vec3 V,T0=tangents.at(0);
	V.x=-T0.y; V.y=T0.x; V.z=T0.z; //pick arbitrary vector V, which must not be parallel to T0
	glm::vec3 N0 = glm::normalize(glm::cross(T0,V)); //N0 = T0 x V
	glm::vec3 B0 = glm::cross(T0,N0); //B0 = T0 x N0
	normals.push_back(N0);
	binormals.push_back(B0);

	//move normals and binormals along the path
	for (size_t i=1; i<p.size(); i++) {
		glm::vec3 Ni=glm::normalize(glm::cross(binormals.at(i-1),tangents.at(i)));
		glm::vec3 Bi=glm::normalize(glm::cross(tangents.at(i),Ni));
		normals.push_back(Ni);
		binormals.push_back(Bi);
	}

	//now generate the points and faces (you could just make the rings and join the faces manually without the AddFace helper function)
	int seg_col = 0;
	float ang = 2*glm::pi<float>()/(float)NumSegments;
	for (unsigned int i=0; i<tangents.size()-1; i++) {
		glm::vec3 C=colours.at(seg_col);
		seg_col=(seg_col+1)%colours.size();
		glm::vec3 P0=p.at(i), T0=tangents.at(i), N0=normals.at(i), B0=binormals.at(i);
		glm::vec3 P1=p.at(i+1), T1=tangents.at(i+1), N1=normals.at(i+1), B1=binormals.at(i+1);
		for (unsigned int j=0; j<NumSegments; j++) {
			//note the vector maths - four points going clockwise abcd - also j=0..NumSegments-1 with j+1 used times angle step which wraps round to 2PI on final segment vertices
			glm::vec3 Qa = P0 + Radius*cos(j*ang)*N0 + Radius*sin(j*ang)*B0;
			glm::vec3 Qb = P0 + Radius*cos((j+1)*ang)*N0 + Radius*sin((j+1)*ang)*B0; //next point around circle (doesn't matter if j+1 wraps past 2PI)
			glm::vec3 Qc = P1 + Radius*cos((j+1)*ang)*N1 + Radius*sin((j+1)*ang)*B1; //next point around next circle
			glm::vec3 Qd = P1 + Radius*cos(j*ang)*N1 + Radius*sin(j*ang)*B1; //first point on next circle
			//AddFace(Qa,Qb,Qc,glm::vec3(1,0,0),glm::vec3(1,0,0),glm::vec3(1,0,0));
			//AddFace(Qa,Qc,Qd,glm::vec3(0,0,1),glm::vec3(0,0,1),glm::vec3(0,0,1));
			AddFace(Qa,Qb,Qc,C,C,C);
			AddFace(Qa,Qc,Qd,C,C,C);
		}
	}
}

void TubeGeometry::GenerateMesh(float Radius, unsigned int NumSegments)
{
	//TODO: check whether generate has been called before and free the gpu buffers if necessary
	//go through all separate splines and generate a tube geometry for each one
	vector<vector<glm::vec3>>::iterator itSplines;
	vector<vector<glm::vec3>>::iterator itCols;
	for (itSplines=splines.begin(), itCols=spline_colours.begin(); itSplines!=splines.end(); ++itSplines, ++itCols) {
		vector<glm::vec3> spline = *itSplines;
		vector<glm::vec3> colours = *itCols;
		//cout<<"spline size="<<spline.size()<<" colours size="<<colours.size()<<endl;
		if (spline.size()>1)
			GenerateTube(Radius,NumSegments,spline,colours);
	}
	//now put mesh to buffers
	CreateBuffers();

}
