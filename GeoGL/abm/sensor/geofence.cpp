/*
 * geofence.cpp
 *
 *  Created on: 19 Aug 2015
 *      Author: richard
 */

#include "geofence.h"

#include "abm/Agent.h"
#include "abm/Model.h"
#include "ellipsoid.h"
#include "mesh2.h"
#include "cuboid.h"
#include "scenegraph.h"

namespace ABM {
namespace sensor {

//TODO: a geofence sphere might be a lot more sensible, other shapes could be a cone (lat/lon etc) or cylinder.
//TODO: need Earth alignment and move away from AABB (axis aligned bounding box).

GeoFence::GeoFence() {
	// TODO Auto-generated constructor stub
}

GeoFence::GeoFence(ABM::Model* pModel, const glm::dvec3& LatLonHCentre, const glm::dvec3& LatLonHSize) {
	Counter=0; //always good to set the counter to zero first

	Ellipsoid ellipsoid; //bit naughty, but it gets us the default WGS84 ellipsoid
	glm::vec3 C = ellipsoid.toVector(glm::radians(LatLonHCentre.x),glm::radians(LatLonHCentre.y),LatLonHCentre.z);
	//now the two diagonals that I can use to get the max and min extents for each axis
	glm::dvec3 Pmin=LatLonHCentre-glm::dvec3(LatLonHSize.x/2,LatLonHSize.y/2,LatLonHSize.z/2);
	glm::dvec3 Pmax=LatLonHCentre+glm::dvec3(LatLonHSize.x/2,LatLonHSize.y/2,LatLonHSize.z/2);
	glm::vec3 Emin = ellipsoid.toVector(glm::radians(Pmin.x),glm::radians(Pmin.y),Pmin.z);
	glm::vec3 Emax = ellipsoid.toVector(glm::radians(Pmax.x),glm::radians(Pmax.y),Pmax.z);
	xmin=Emin.x; ymin=Emin.y; zmin=Emin.z;
	xmax=Emax.x; ymax=Emax.y; zmax=Emax.z;
	//then need to check that the max and mins are the right way around after the transformation
	if (xmin>xmax) { float tmp=xmin; xmin=xmax; xmax=tmp; }
	if (ymin>ymax) { float tmp=ymin; ymin=ymax; ymax=tmp; }
	if (zmin>zmax) { float tmp=zmin; zmin=zmax; zmax=tmp; }

	//create the object and add it to the scene graph
	this->pModel=pModel;
	SceneGraphType* sg = pModel->_pSceneGraph;
	Cuboid* cube = new Cuboid(xmax-xmin,ymax-ymin,zmax-zmin);
	cube->SetPos((xmin+xmax)/2,(ymin+ymax)/2,(zmin+zmax)/2); //don't forget to move it to the right position
	cube->AttachShader(pModel->_agents.agentShader,false); //and a better way of doing this would be good
	this->pMesh = cube;
	sg->push_back(this->pMesh);

	//and add the sensor to the model
	pModel->_sensors.push_back(this);
}

GeoFence::~GeoFence() {
	// TODO Auto-generated destructor stub
	//remove from scene graph?
	//remove from sensor list

	delete this->pMesh;
}

/// <summary>
/// Centre is lat, lon, height. Size is also in lat lon coordinates.
/// </summary>
GeoFence* GeoFence::CreateCuboid(ABM::Model* pModel, const glm::dvec3& LatLonHCentre, const glm::dvec3& LatLonHSize)
{
	GeoFence* gf = new GeoFence(pModel, LatLonHCentre, LatLonHSize);
	return gf;
}

//see: http://people.csail.mit.edu/amy/papers/box-jgt.pdf
bool GeoFence::IntersectRayCube(glm::dvec3 P0, glm::dvec3 P1)
{
	//axis aligned bounding box (AABB)
	//TODO: this bit comes out
	//double xmin=Centre.x-Size.x/2;
	//double xmax=Centre.x+Size.x/2;
	//double ymin=Centre.y-Size.y/2;
	//double ymax=Centre.y+Size.y/2;
	//double zmin=Centre.z-Size.z/2;
	//double zmax=Centre.z+Size.z/2;
	//end of this bit comes out
	double dx=P1.x-P0.x;
	double dy=P1.y-P0.y;
	double dz=P1.z-P0.z;
	double t0=0, t1=1; //intersection interval
	double tmin,tmax,tymin,tymax,tzmin,tzmax;
	if (dx>=0) {
		tmin=(xmin-P0.x)/dx;
		tmax=(xmax-P0.x)/dx;
	}
	else {
		tmin=(xmax-P0.x)/dx;
		tmax=(xmin-P0.x)/dx;
	}
	if (dy>=0) {
		tymin=(ymin-P0.y)/dy;
		tymax=(ymax-P0.y)/dy;
	}
	else {
		tymin=(ymax-P0.y)/dy;
		tymax=(ymin-P0.y)/dy;
	}
	if ((tmin>tymax)||(tymin>tmax)) return false;

	if (tymin>tmin) tmin=tymin;
	if (tymax<tmax) tmax=tymax;

	if (dz>=0) {
		tzmin=(zmin-P0.z)/dz;
		tzmax=(zmax-P0.z)/dz;
	}
	else {
		tzmin=(zmax-P0.z)/dz;
		tzmax=(zmin-P0.z)/dz;
	}

	if ((tmin>tzmax)||(tzmin>tmax)) return false;

	if (tzmin>tmin) tmin=tzmin;
	if (tzmax<tmax) tmax=tzmax;

	return ((tmin<t1)&&(tmax>t0));
}

bool GeoFence::IntersectPointCube(glm::dvec3 P)
{
	return ((P.x>=xmin)&&(P.x<=xmax)&&(P.y>=ymin)&&(P.y<=ymax)&&(P.z>=zmin)&&(P.z<=zmax));
}

/// <summary>
/// Put the counter back to zero
/// </summary>
void GeoFence::Clear()
{
	Counter=0;
}

/// <summary>
/// This is the method which all sensors need to implement, which is called by the model inside of the simulation step loop.
/// </summary>
void GeoFence::Test(std::vector<ABM::Agent*> agents)
{
	for (std::vector<ABM::Agent*>::iterator it = agents.begin(); it!=agents.end(); ++it) {
		ABM::Agent* a = (*it);
		//if you have a movement vector from the last known position, then use this
		//if (IntersectRayCube()) {
		//	++Counter;
		//}
		//otherwise, just use its current position
		glm::dvec3 P = a->GetXYZ();
		if (IntersectPointCube(P)) {
			++Counter;
			std::cout<<"GeoFence: "<<a->Name<<" "<<a->_BreedName<<std::endl;
		}
	}
}

} // namespace sensor
} // namespace ABM
