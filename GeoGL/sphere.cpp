#include "sphere.h"

//todo: allow user to pass in a list of vertex colours
/// <summary>Create a sphere with a single radius (i.e. not a spheroid)</summary>
Sphere::Sphere(double Radius,int WidthSegments,int HeightSegments) {
	init(Radius,Radius,Radius,WidthSegments,HeightSegments);
}

/// <summary>Overloaded constructor which passes three radii to allow a spheroid to be created</summary>
Sphere::Sphere(double A, double B, double C, int WidthSegments, int HeightSegments) {
	init(A,B,C,WidthSegments,HeightSegments);
}

/// <summary>
/// Private initialisation of the sphere as I need to call this from both constructors
/// </summary>
void Sphere::init(double A, double B, double C, int WidthSegments, int HeightSegments) {
	float anglon=2*glm::pi<float>()/(float)WidthSegments; //segment angle step around lon
	float anglat=2*glm::pi<float>()/(float)HeightSegments; //segment angle step around lat
	for (float lat=-glm::pi<float>(); lat<glm::pi<float>(); lat+=anglat) {
		for (float lon=0; lon<2*glm::pi<float>(); lon+=anglon) {
			//work out four patch coordinates based on current lat/lon angles +- each combination of next lat/lon coord
			glm::vec3 Pa,Pb,Pc,Pd;
			//TODO: you might want to check the orientation of the point
			Pa.x=sin(lon);			Pa.y=cos(lon)*sin(lat);					Pa.z=cos(lon)*cos(lat);
			Pb.x=sin(lon+anglon);	Pb.y=cos(lon+anglon)*sin(lat);			Pb.z=cos(lon+anglon)*cos(lat);
			Pc.x=sin(lon+anglon);	Pc.y=cos(lon+anglon)*sin(lat+anglat);	Pc.z=cos(lon+anglon)*cos(lat+anglat);
			Pd.x=sin(lon);			Pd.y=cos(lon)*sin(lat+anglat);			Pd.z=cos(lon)*cos(lat+anglat);
			//Pa,Pb,Pc,Pd are clockwise, so add in reverse
			AddFace(Pa,Pd,Pc,glm::vec3(0,1.0,0),glm::vec3(0,1.0,0),glm::vec3(0,1.0,0));
			AddFace(Pa,Pc,Pb,glm::vec3(0,1.0,0),glm::vec3(0,1.0,0),glm::vec3(0,1.0,0));
		}
	}

	//now scale up (or down) to the correct radius
	ScaleVertices(A,B,C);

	//and finally build the graphics
	CreateBuffers();
}

Sphere::~Sphere() {
}