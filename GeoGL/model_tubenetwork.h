#pragma once
/*
* This is my agent based model of the tube network. In other words, it's of type ABM::Model and should be a distinct entity from the rest of the code.
* This is my use of the application for testing. Contains everything NetLogo would need to run a tube network.
*
*/
#include "main.h"

#include <string>
#include <map>
#include <unordered_map>

//#include "graph.h"
#include "netgraphgeometry.h"

#include "Model.h"

//struct TubeStationWGS84
//{
//	float lonlat[2];
//};

//forward declarations
class Graph;

/**
* If you create one of these then you're creating a real working tube network
*/
class ModelTubeNetwork : public ABM::Model {
public:
	static const std::string Filename_StationCodes; //station locations
	static const std::string Filename_TubeODNetwork; //network from JSON origin destination file
	static const std::string Filename_TrackernetPositions; //train positions

	//Graph* tube_graph;
	std::unordered_map<char,Graph*> tube_graphs; //one for each line
	std::unordered_map<std::string,struct GraphNameXYZ>* tube_stations;

	//std::map<string,struct TubeStationWGS84>* tube_stations;

	ModelTubeNetwork(SceneGraphType* SceneGraph);
	~ModelTubeNetwork();
	unsigned int LineCodeToColour(char Code);
	glm::vec3 LineCodeToVectorColour(char Code);
	void loadStations(std::string Filename);
	void loadLinks(std::string NetworkJSONFilename);
	void load(std::string NetworkJSONFilename, std::string StationsCSVFilename); 
	void loadPositions(std::string Filename);
	NetGraphGeometry* GenerateLineMesh(char LineCode);
	Object3D* GenerateMesh();

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);
};