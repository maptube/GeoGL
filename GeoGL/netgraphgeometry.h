#pragma once
//#include "opengl4.h"
//#include "mesh2.h"
#include "tubegeometry.h"
//#include "graph.h"
#include <unordered_map>

//forward references
class Graph;

/**
* Used to join Name on network graph with a location.
*/
struct GraphNameXYZ {
	std::string Name;
	glm::vec3 P;
};

/**
* Network Graph Geometry based on a TubeGeometry built from a network graph of origin destination links.
* The general idea is to keep the graph and geometry representing it attached to each other.
*/
class NetGraphGeometry : public TubeGeometry
{
public:
	Graph* pG;
	std::vector<struct GraphNameXYZ> locations;
	NetGraphGeometry(Graph *pGraph, std::unordered_map<std::string,struct GraphNameXYZ>* VertexPositions, float Radius, unsigned int NumSegments, glm::vec3 Colour);
	~NetGraphGeometry(void);
};

