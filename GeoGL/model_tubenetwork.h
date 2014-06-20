#pragma once
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

//#include "graph.h"
#include "netgraphgeometry.h"

#include "Model.h"
#include "agenttime.h"

//struct TubeStationWGS84
//{
//	float lonlat[2];
//};

//forward declarations
class Graph;

struct tube_anim_record {
	char LineCode;
	std::string StationCode;
	float TimeToStation;
	short Direction;
};

/**
* If you create one of these then you're creating a real working tube network
*/
class ModelTubeNetwork : public ABM::Model {
protected:
	void DisplayStatistics();
public:
	static const std::string Filename_StationCodes; //station locations
	static const std::string Filename_TubeODNetwork; //network from JSON origin destination file
	static const std::string Filename_TrackernetPositions; //train positions
	static const float LineSize; //size of track
	static const int LineTubeSegments; //number of segments making up the tube geometry
	static const float StationSize; //size of station geometry object
	static const float TrainSize; //size of train geometry object

	bool AnimateMode; //set to true to make positions come from tube_anim_frames
	//struct tm AnimateDT; //current animation time
	//time_t AnimationDT; //current animation time
	AgentTime AnimationDT; //current animation time
	time_t FrameTimeN; //frame of animation data that we're currently working towards i.e. next frame after time now
	float AnimationTimeMultiplier; //so you can run the animation at 2x, 4x, anything you like

	//Graph* tube_graph;
	std::unordered_map<char,Graph*> tube_graphs; //one for each line
	std::unordered_map<std::string,struct GraphNameXYZ>* tube_stations;

	//animation
	std::map<time_t,std::map<std::string,struct tube_anim_record>> tube_anim_frames;

	//std::map<string,struct TubeStationWGS84>* tube_stations;

	ModelTubeNetwork(SceneGraphType* SceneGraph);
	~ModelTubeNetwork();
	unsigned int LineCodeToColour(char Code);
	glm::vec3 LineCodeToVectorColour(char Code);
	void loadStations(std::string Filename);
	void loadLinks(std::string NetworkJSONFilename);
	void loadPositions(std::string Filename);
	void LoadAnimatePositions();
	ABM::Agent* HatchAgentFromAnimationRecord(const std::string& UniqueName, const tube_anim_record& rec);
	void LoadAnimation(const std::string& DirectoryName);
	NetGraphGeometry* GenerateLineMesh(char LineCode);
	Object3D* GenerateMesh();

	bool PositionAgent(ABM::Agent* agent,char LineCode, float TimeToStation, std::string StationName, int Direction);

	time_t GetFirstAnimationTime();
	//time_t ModelTubeNetwork::GetNextAnimationTimeFrom(const time_t Now);
	time_t GetNextAnimationTime(const time_t Current);
	void StepAnimation(double Ticks);
	ABM::Agent* NextNodeOnPath(const std::string& LineCode, ABM::Agent* Begin, ABM::Agent* End);
	double GetPathDistance(const glm::dvec3& Begin,const glm::dvec3& End,const std::vector<ABM::Agent*>& Path);
	void RecalculateWaypoint(const tube_anim_record& anim_rec, ABM::Agent* Driver);
	void StepRealTime(double Ticks);

	//ABM::Model virtuals
	void Setup();
	void Step(double Ticks);
};
