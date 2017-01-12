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
#include <unordered_set>

#include "netgraphgeometry.h"

#include "abm/Model.h"
#include "abm/agenttime.h"


//forward declarations
class Graph;

struct tube_anim_record {
	char LineCode;
	std::string StationCode;
	float TimeToStation;
	short Direction;
	short DestinationCode; //added for dm
	std::string Platform; //added for dm
};

struct dm_route_record {
	//int TimePoint; //1min
	int DestinationCode;
	//int trip_num;
	//int set_num;
	std::string OriginStationCode;
	std::string DestinationStationCode;
	int Count;
};

/// <summary>
/// Used to switch between a real-time animation and animation from archive data
/// </summary>
enum EAnimationType { RealTime, Archive };

//TODO: how about an event definition for the analytics? time, trip, set, station code, platform, destination code, (time to station?)

/// <summary>
/// ModelTubeNetwork
/// If you create one of these then you're creating a real working tube network
/// </summary>
class ModelTubeNetwork : public ABM::Model {
protected:
	//datamining tables
	//std::unordered_map<int,std::unordered_set<std::string>> DMRoutes; //i.e. 123->{WIM,WPK,STH...UPM} set of stations identified by a destination code
	//std::map<int,std::unordered_map<std::string,std::vector<dm_route_record>>> DMRoutes; //i.e. 123->{1_76->[WIM,WPK,STH...UPM] } set of stations identified by a destination code and vehicle id (NOTE dest unique for lines i.e. not D_1_76)
	//std::map<std::string,std::vector<dm_route_record>> DMRoutes; //i.e. 123_1_76->[ (WIM,WPK), (WPK,STH), (STH,...), (...,UPM) ] unique vehicle identifier of dest_trip_set with a list of origin and destination links and counts
	std::map<int,std::vector<dm_route_record>> DMRoutes; //i.e. 123->[ (WIM,WPK), (WPK,STH), (STH,...), (...,UPM) ]
	std::map<int,std::unordered_map<char,int>> DMLines; //i.e. 123->{ D->100, P->50, C->10, V->20 } possible lines for a destination code with a count for each (TODO: look at Frugal max count algorithm?)


	//debug+mine
	void datamineDestinationCodes();
	void dataminePrintDestinationCodes();
	void dataminePrintRoutes();
	void DisplayStatistics();
public:
	static const std::string Filename_StationCodes; //station locations
	static const std::string Filename_TubeODNetwork; //network from JSON origin destination file
	static const std::string Filename_TrackernetPositions; //train positions
	static const std::string Filename_AnimationDir; //directory containing animation data as csv files
	static const std::string RealTimePositionsURL; //url which returns a csv file containing the current positions of tubes
	static const float LineSize; //size of track
	static const int LineTubeSegments; //number of segments making up the tube geometry
	static const float StationSize; //size of station geometry object
	static const float TrainSize; //size of train geometry object

	EAnimationType AnimateMode; //switch between real-time and archive data animations
	AgentTime AnimationDT; //current animation time
	AgentTime AnimationDataDT; //time of the last data downloaded for real-time
	AgentTime LastDownloadDT; //time data was last downloaded in real-time mode
	time_t FrameTimeN; //frame of animation data that we're currently working towards i.e. next frame after time now
	float AnimationTimeMultiplier; //so you can run the animation at 2x, 4x, anything you like

	//animation
	std::map<time_t,std::map<std::string,struct tube_anim_record>> tube_anim_frames;


	ModelTubeNetwork(SceneGraphType* SceneGraph);
	~ModelTubeNetwork();
	unsigned int LineCodeToColour(char Code);
	glm::vec3 LineCodeToVectorColour(char Code);
	void loadStations(std::string Filename);
	void loadLinks(std::string NetworkJSONFilename);
	void loadPositions(std::string Filename);
	void LoadAnimatePositions();
	ABM::Agent* HatchAgentFromAnimationRecord(const std::string& UniqueName, const tube_anim_record& rec); //TODO: this needs to be removed in favour of the other version below
	ABM::Agent* HatchAgent(const std::string& id, char lineCode, float timeToStation, const std::string& stationCode, int direction, int destinationCode, const std::string& platform);
	void LoadAnimation(const std::string& DirectoryName);

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
