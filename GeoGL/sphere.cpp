#include "sphere.h"

//todo: allow user to pass in a list of vertex colours
/// <summary>Create a sphere with a single radius (i.e. not a spheroid)</summary>
Sphere::Sphere(double Radius,int WidthSegments,int HeightSegments) {
	//init(Radius,Radius,Radius,WidthSegments,HeightSegments);
	init2(Radius,Radius,Radius,WidthSegments,HeightSegments);
}

/// <summary>Overloaded constructor which passes three radii to allow a spheroid to be created</summary>
Sphere::Sphere(double A, double B, double C, int WidthSegments, int HeightSegments) {
	//init(A,B,C,WidthSegments,HeightSegments);
	init2(A,B,C,WidthSegments,HeightSegments);
}

/// <summary>Overloaded constructor which allows the vertex format to be set for the mesh</summary>
/// <param name="VF">Format of vertices used by mesh e.g. Position only or the Mesh2 default of PositionColour</param>
Sphere::Sphere(double A, double B, double C, int WidthSegments, int HeightSegments, gengine::VertexFormat VF) {
	_VertexFormat = VF;
	init2(A,B,C,WidthSegments,HeightSegments);
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

//optimised version of original init which is horrible, but not much different in speed terms
void Sphere::init2(double A, double B, double C, int WidthSegments, int HeightSegments) {
	//work out and store the sin and cos of the positions around one of the horizontal slices as it gets used going up the sphere
	//and also the heights of the slices
	float* sinlon = new float[WidthSegments];
	float* coslon = new float[WidthSegments];
	float* sinlat = new float[HeightSegments];
	float* coslat = new float[HeightSegments];

	float anglon=2*glm::pi<float>()/(float)WidthSegments; //segment angle step around lon
	float anglat=2*glm::pi<float>()/(float)HeightSegments; //segment angle step around lat
	int i=0;
	for (float lon=0; lon<2*glm::pi<float>(); lon+=anglon) {
		sinlon[i]=glm::sin(lon); //sin(lon);
		coslon[i]=glm::cos(lon); //cos(lon);
		++i;
	}
	//now store the height slices
	i=0;
	for (float lat=-glm::pi<float>(); lat<glm::pi<float>(); lat+=anglat) {
		sinlat[i]=glm::sin(lat); //sin(lat);
		coslat[i]=glm::cos(lat); //cos(lat);
		++i;
	}

	//OK, that's enough pre-calculation, wire up the points into faces
	for (int ilat=0; ilat<HeightSegments; ++ilat) {
		for (int ilon=0; ilon<WidthSegments; ++ilon) {
			int ilat2=(ilat+1)%HeightSegments; int ilon2=(ilon+1)%WidthSegments;
			//work out four patch coordinates based on current lat/lon angles +- each combination of next lat/lon coord
			glm::vec3 Pa,Pb,Pc,Pd;
			//TODO: you might want to check the orientation of the point
			Pa.x=sinlon[ilon];		Pa.y=coslon[ilon]*sinlat[ilat];			Pa.z=coslon[ilon]*coslat[ilat];
			Pb.x=sinlon[ilon2];		Pb.y=coslon[ilon2]*sinlat[ilat];		Pb.z=coslon[ilon2]*coslat[ilat];
			Pc.x=sinlon[ilon2];		Pc.y=coslon[ilon2]*sinlat[ilat2];		Pc.z=coslon[ilon2]*coslat[ilat2];
			Pd.x=sinlon[ilon];		Pd.y=coslon[ilon]*sinlat[ilat2];		Pd.z=coslon[ilon]*coslat[ilat2];
			//Pa,Pb,Pc,Pd are clockwise, so add in reverse
			AddFace(Pa,Pd,Pc,glm::vec3(0,1.0,0),glm::vec3(0,1.0,0),glm::vec3(0,1.0,0));
			AddFace(Pa,Pc,Pb,glm::vec3(0,1.0,0),glm::vec3(0,1.0,0),glm::vec3(0,1.0,0));
		}
	}

	//now scale up (or down) to the correct radius
	ScaleVertices(A,B,C);

	//and finally build the graphics
	CreateBuffers();

	delete [] sinlon;
	delete [] coslon;
	delete [] sinlat;
	delete [] coslat;
}

//Sphere::~Sphere() {
//}