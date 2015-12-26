/*
 * geofence.h
 *
 *  Created on: 19 Aug 2015
 *      Author: richard
 */

#pragma once

#include <vector>

#include "gengine/gengine.h"

//forward declarations
class Mesh2;

namespace ABM {
	class Agent;
	class Model;
}

namespace ABM {
namespace sensor {

/// <summary>
/// Defines an area in 3D space in which counters can count the number of agents passing through,
/// or generate other statistics.
/// TODO: needs to derive from a base sensor class
/// TODO: add sensor start and stop methods which you can pass an AgentTime to which allow recording of the time interval used for measurement
/// </summary>
class GeoFence {
public:
	GeoFence();
	GeoFence(ABM::Model* pModel, const glm::dvec3& LatLonHCentre, const glm::dvec3& LatLonHSize);
	virtual ~GeoFence();

	//static constructors
	static GeoFence* CreateCuboid(ABM::Model* pModel, const glm::dvec3& Centre, const glm::dvec3& Size);

	//public counters i.e. you can do what you like with them
	int Counter;
	//TODO: you could count breeds?

	//public methods
	void Clear();
	void Test(std::vector<ABM::Agent*> agents);

protected:
	//SceneGraphType* pSceneGraph;
	ABM::Model* pModel; //pointer to the model this is attached to
	Mesh2* pMesh;
	//position, width, height etc
	glm::dvec3 LLHCentre; //lat lon height centre
	glm::dvec3 LLHSize;
	//TODO: geometry i.e. sphere, cube, cylinder

	//this is the bounding box in Cartesian coords, which the agents are in
	double xmin, xmax;
	double ymin, ymax;
	double zmin, zmax;

	bool IntersectRayCube(glm::dvec3 P0, glm::dvec3 P1);
	bool IntersectPointCube(glm::dvec3 P);
};

} // namespace sensor
} // namespace ABM

