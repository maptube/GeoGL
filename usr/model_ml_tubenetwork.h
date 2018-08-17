#pragma once
//model_ml_tubenetwork
/*
* This is my agent based model of the tube network. In other words, it's of type ABM::Model and should be a distinct entity from the rest of the code.
* This is my use of the application for testing. Contains everything NetLogo would need to run a tube network.
*
*/
#include "main.h"

#include <string>
#include <ctime>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "netgraphgeometry.h"

#include "abm/Model.h"
#include "abm/agenttime.h"


//forward declarations
class Graph;



/// <summary>
/// ModelMLTubeNetwork
/// Model to learn data from the tube network
/// </summary>
class ModelMLTubeNetwork : public ABM::Model {
protected:
	std::map<char,std::shared_ptr<float[]>> LinkProbTables; //link count (probability) tables for each line
public:
	static const std::string Filename_StationCodes; //station locations
	//static const std::string Filename_TubeODNetwork; //network from JSON origin destination file
	//static const std::string Filename_TrackernetPositions; //train positions
	static const std::string Filename_AnimationDir; //directory containing animation data as csv files
	//static const std::string RealTimePositionsURL; //url which returns a csv file containing the current positions of tubes
	static const float LineSize; //size of track
	static const int LineTubeSegments; //number of segments making up the tube geometry
	static const float StationSize; //size of station geometry object
	static const float TrainSize; //size of train geometry object

	AgentTime AnimationDataDT; //time of the last data downloaded
	int NumStations;
	int epoch;


	ModelMLTubeNetwork(SceneGraphType* SceneGraph);
	~ModelMLTubeNetwork();
	unsigned int LineCodeToColour(char Code);
	glm::vec3 LineCodeToVectorColour(char Code);
	void LoadStations(std::string Filename);
	void LoadPositions(std::string Filename);

	int CoalesceLinks();
	void InitialiseLinkProbTables();
	void LinkProbTablesUpdate(char LineCode, ABM::Agent* o, ABM::Agent* d);
	void LinkProbTablesUpdateDirect(char LineCode, ABM::Agent* o, ABM::Agent* d, int count);
	void WriteLinkProbTables();
	int CullLinkProbTables();
	int Reachable(ABM::Agent* o, ABM::Agent* d, char lineCode, int direction, int limit);
	std::vector<ABM::Agent*> GetLongestRoute(ABM::Agent* o, ABM::Agent* d, char lineCode, int direction, int limit);
	int NodePrune();

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);
};

