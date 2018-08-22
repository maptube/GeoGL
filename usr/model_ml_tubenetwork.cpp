//Tube Network functions
//Specialisation of functions required to load and build a London Underground network
#include "model_ml_tubenetwork.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iterator>
#include <math.h>
#include <ctime>
#include <memory>

#include <iomanip>
#include <unordered_map>

#include <dirent.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std;

#include "json/json.h"
//#include "json/reader.h"

#include "ellipsoid.h"
#include "netgraphgeometry.h"
#include "graph.h"
#include "net/httprequest.h"
#include "data/datatable.h"

#include "abm/Link.h"
#include "abm/Agent.h"
//#include "Model.h"
#include "agentobject.h"

//define locations of configuration files
const std::string ModelMLTubeNetwork::Filename_StationCodes =
//"data/station-codes.csv";
"../data/station-codes.csv"; //station locations
//const std::string ModelTubeNetwork::Filename_TubeODNetwork =
/*"data/tube-network.json";*/
//"../data/tube-network.json"; //network from JSON origin destination file
//const std::string ModelTubeNetwork::Filename_TrackernetPositions =
/*"data/trackernet_20140127_154200.csv";*/
//"../data/trackernet_20140127_154200.csv"; //train positions
const std::string ModelMLTubeNetwork::Filename_AnimationDir =
"D:\\richard\\data\\realtime\\2017_disk\\trackernet-cache\\";
//const std::string ModelTubeNetwork::RealTimePositionsURL =
//"http://loggerhead.casa.ucl.ac.uk/api/f/trackernet?pattern=trackernet_*.csv";
const float ModelMLTubeNetwork::LineSize = 25; //size of track - was 50
const int ModelMLTubeNetwork::LineTubeSegments = 10; //number of segments making up the tube geometry
const float ModelMLTubeNetwork::StationSize = 50.0f; //size of station geometry object
const float ModelMLTubeNetwork::TrainSize = 300.0f; // 150.0f; //size of train geometry object

/// <summary>
/// Create a Tube Network containing a model of the tube and train agents which run around the track. You have to call load() with the relevant file names to load the
/// network graph and stations location table.
/// </summary>
/// <param name="SceneGraph">
/// Reference to the scene graph that the Tube Network will be added to and which the ABM code will control.
/// </param>
ModelMLTubeNetwork::ModelMLTubeNetwork(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph) { 
}

/// <summary>Destructor</summary>
ModelMLTubeNetwork::~ModelMLTubeNetwork() {
}


/// <summary>
/// Get colour for a tube line from the line code character
/// </summary>
/// <param name="Code">Line Code</param>
/// <returns>A hexadecimal colour integer for the line</returns>
unsigned int ModelMLTubeNetwork::LineCodeToColour(char Code) {
	unsigned int lineBColour = 0xb06110,
		lineCColour = 0xef2e24,
		lineDColour = 0x008640,
		lineHColour = 0xffd203, //this is yellow!
		lineJColour = 0x959ca2,
		lineMColour = 0x98005d,
		lineNColour = 0x231f20,
		linePColour = 0x1c3f95,
		lineVColour = 0x009ddc,
		lineWColour = 0x86cebc;
	//lineY colour?
	switch (Code) {
	case 'B': return lineBColour; //Bakerloo
	case 'C': return lineCColour; //Central
	case 'D': return lineDColour; //District
	case 'H': return lineHColour; //Hammersmith and City and Circle
	case 'J': return lineJColour; //Jubilee
	case 'M': return lineMColour; //Metropolitan
	case 'N': return lineNColour; //Northern
	case 'P': return linePColour; //Piccadilly
	case 'V': return lineVColour; //Victoria
	case 'W': return lineWColour; //Waterloo and City
	default: return 0xffffff; //white
	}
};

/// <summary>
/// Get colour for a tube line from the line code character
/// </summary>
/// <param name="Code">Line Code</param>
/// <returns>A hexadecimal colour vector (0..1) for the line</returns>
glm::vec3 ModelMLTubeNetwork::LineCodeToVectorColour(char Code) {
	//return a random colour for testing
	//return glm::vec3((rand()%256)/256.0f, (rand()%256)/256.0f, (rand()%256)/256.0f);
	switch (Code) {
	case 'B': return glm::vec3(0.69f, 0.38f, 0.06f); //Bakerloo
	case 'C': return glm::vec3(0.94f, 0.18f, 0.14f); //Central
	case 'D': return glm::vec3(0.0f, 0.53f, 0.25f); //District
	case 'H': return glm::vec3(1.0f, 0.82f, 0.01f); //Hammersmith and City and Circle (this is yellow!)
	case 'J': return glm::vec3(0.58f, 0.61f, 0.64f); //Jubilee
	case 'M': return glm::vec3(0.6f, 0.0f, 0.36f); //Metropolitan
	case 'N': return glm::vec3(0.14f, 0.12f, 0.13f); //Northern
	case 'P': return glm::vec3(0.11f, 0.24f, 0.58f); //Piccadilly
	case 'V': return glm::vec3(0.0f, 0.62f, 0.86f); //Victoria
	case 'W': return glm::vec3(0.53f, 0.81f, 0.74f); //Waterloo and City
	default: return glm::vec3(1.0f, 1.0f, 1.0f); //white
	}
}

void ModelMLTubeNetwork::LoadStations(std::string Filename)
{
	int count = 0; //using count to make a unique station id number for the probability matrix column and row
	LoadAgentsCSV(Filename, 1, [this,&count](std::vector<std::string> items) {
		//#code,NPTGCode,lines,lon,lat,name
		//ACT,9400ZZLUACT1,DP,-0.28025120353611,51.50274977300050,Acton Town
		ABM::Agent *a = nullptr;
		if ((items.size() == 0) || (items[0][0] == '#')) return a; //blank line or comment line
		if (items.size() >= 6) {
			std::string code1;
			float lon, lat;
			std::string LineCodes;
			code1 = items[0];
			lon = std::stof(items[3]);
			lat = std::stof(items[4]);
			LineCodes = items[2];
			GraphNameXYZ pos; //TODO: you could remove this at tube_stations has now been removed
			pos.Name = code1; pos.P = _pEllipsoid->toVector(glm::radians(lon), glm::radians(lat), 0);

			//and create the node agent
			a = _agents.Hatch("node"); //alternative method to sprout

			a->SetXYZ(pos.P.x, pos.P.y, pos.P.z);
			a->Name = code1;
			a->Set("LineCodes", LineCodes);
			a->Set("id", count); //numeric id set on the station node agent for matrix probs row/col
			std::cout << a->Name<<std::endl;
			++count;
		}
		return a;
	});
	NumStations = count;
};

/// <summary>
/// Load train positions from CSV file and create agents for them as necessary by merging with existing agents
/// Used to log O/D information and station arrival points for network and timetable creation
/// </summary>
void ModelMLTubeNetwork::LoadPositions(std::string Filename) {
	std::set<std::string> LiveAgents;
	LoadAgentsCSV(Filename, 1, [this,&LiveAgents](std::vector<std::string> items) {
		//linecode,tripnumber,setnumber,lat,lon,east,north,timetostation(secs),location,stationcode,stationname,platform,platformdirectioncode,destination,destinationcode
		//"B","0","201","51.54392","-0.2754093","519577.9","184243.3","0","At Platform","SPK","Stonebridge Park","Southbound - Platform 1","1","Check Front of Train","728"
		ABM::Agent *a = nullptr;
		if (items.size() == 15) {
			//line,trip,set,lat,lon,east,north,timetostation,location,stationcode,stationname,platform,platformdirectioncode,destination,destinationcode
			std::string lineCode = items[0];
			std::string tripcode = items[1];
			std::string setcode = items[2];
			std::string stationcode = items[9];
			int timetostation = std::atoi(items[7].c_str());
			int dir = std::atoi(items[12].c_str());
	if (dir != 0) return a; //HACK! only track the main direction
			std::string name = lineCode + "_" + tripcode + "_" + setcode; //unique name to match up to next data download
			std::vector<ABM::Agent*> alist = _agents.With("name", name);
			if (alist.size() > 0) a = alist.front(); //found agent
			if (a==nullptr) {
				//agent doesn't exist, so we need to create a new one
				
				//log the creation point
				//std::cout << "Create," << name << std::endl;

				a = _agents.Hatch("driver");
				a->Name = name;
				//a->Face(*a->Get<ABM::Agent*>("toNode"));
				a->Set<float>("v", 0); //velocity not used
				a->Set<int>("direction", dir);
				a->Set<std::string>("lineCode", lineCode);
				a->SetColour(LineCodeToVectorColour(lineCode[0]));
				//set up the to and from nodes so all the properties are defined at creation time, even if they're null
				std::vector<ABM::Agent*> agent_d = _agents.With("name", stationcode);
				if (agent_d.size()>0)
					a->Set<ABM::Agent*>("toNode", agent_d.front());
				else
					a->Set<ABM::Agent*>("toNode", nullptr); //guard case for when station not found
				a->Set<ABM::Agent*>("fromNode", nullptr);
				a->Set<int>("time",AnimationDataDT._DT); //NOTE: a time_t is secs since 1 Jan 1970, which will fit in a single precision int
				a->Set<int>("eta", AnimationDataDT._DT + timetostation);
			}
			//now look at the O/D links - if this was a new agent, then there won't be a fromnode and we can't do much until we see it again with a different tonode
			ABM::Agent* fromNode = a->Get<ABM::Agent*>("fromNode");
			ABM::Agent* toNode = a->Get<ABM::Agent*>("toNode");
			std::vector<ABM::Agent*> agent_d = _agents.With("name", stationcode); //destination node station - do you need the breed name as well?
			if (agent_d.size() == 0) return a; //guard case - stationcode not found, so exit
			
			//picked up existing tube with this name, so check for destination node changes i.e. it's arrived somewhere - note you can have zero in time to station, so we have an exact station hit
			if (timetostation == 0) {
				//arrived at "stationcode" at this point in time (now), which we need to log - NOTE: station code might not be equal to tonode if it jumped a station between sample points
				//std::cout << "Arrive," << name << "," << stationcode << std::endl;
				a->Set<ABM::Agent*>("fromNode", agent_d[0]); //set the from node for next time we arrive somewhere
				a->Set<int>("time", AnimationDataDT._DT);
			}
			else if ((toNode)&&(toNode->Name!=stationcode)) {
				//we've been through a station between now and the last sample point, so we need to log this fact and update the agent's data
				//NOTE: we only update when the get past the toNode station i.e. null->MYB when you arrive at MYB becomes MYB->PAD and we have a link, but we wait until arrival at PAD
				//as we can then potentially log the time taken to get there

				//update agent with new from and to node information
				a->Set<ABM::Agent*>("fromNode", toNode); //yes, the node the agent has come from was the last one it was going to
				a->Set<ABM::Agent*>("toNode", agent_d[0]); //and the new tonode comes from looking up the "stationcode" in the list of station nodes

				//get the difference between the last timestamp and now, then update the timestamp
				AgentTime dt,eta;
				dt._DT = a->Get<int>("time");
				eta._DT = a->Get<int>("eta");
				int runtime = (int)AgentTime::DifferenceSeconds(eta, dt); //in seconds
				a->Set<int>("time", AnimationDataDT._DT);
				a->Set<int>("eta", AnimationDataDT._DT + timetostation);
				
				if ((fromNode)&&(fromNode!=toNode)) //so we have valid from and to nodes, with a changed to node this time around, meaning he's traversed from->to and is now heading towards a different station
				{
					//NOTE: this is key: fromNode!=toNode NEVER create self-referential links, it messes up a lot of things and never happens
					//NOTE: if you logged the time to station, then you could use this to estimate the tonode arrival time which is <now
					//std::cout << "Link," << name << "," << fromNode->Name << "," << toNode->Name << std::endl;

					//update link probability table
					//LinkProbTablesUpdate(a->Get<std::string>("lineCode")[0], fromNode, toNode);
					//TODO: remove this in favour of the routes update code below

					//check that this link doesn't already exist!!!
					std::vector<ABM::Link*> outLinks = fromNode->OutLinks();
					auto found=std::find_if(outLinks.begin(), outLinks.end(),
						[&toNode,&lineCode,&dir](ABM::Link* const& item)
						{
							return (item->_BreedName==lineCode) && (item->Get<int>("direction")==dir) && (item->end2 == toNode); //test line, direction and end node
						}
					);
					if (found==outLinks.end()) //OK, not found, so it's a new link
					{
						//but, first check that it's not masking an origin/destination that is already reachable
						std::string BreedName = a->Get<std::string>("lineCode"); //e.g. V for Victoria in any direction
						int dist = Reachable(fromNode, toNode, BreedName[0], dir, 10); //NOTE: 10 is a big fan out!!!!
						if (dist == -1)
						{
							LinkProbTablesUpdate(a->Get<std::string>("lineCode")[0], fromNode, toNode);
							//create a link on the graph between these two places
							ABM::Link* L = _links.CreateLink(BreedName, fromNode, toNode); //use line code as breed name i.e. V for Victoria Northbound or Southbound (links are uni directional, so need two)
							//this is where you set the velocity, linecode, direction
							L->Set<std::string>("lineCode", BreedName);
							L->Set<int>("direction", dir);
							L->Set<float>("runlink", 0); //Don't have this here - need timing
							L->colour = a->GetColour(); // LineCodeToVectorColour(BreedName[0]); //NOTE: the colour for links comes from the default breed colour
							if (BreedName=="V")
								std::cout << "CreateLink," << name << "," << fromNode->Name << "," << toNode->Name << std::endl;
						}
						else {
							//if link is already reachable, then run along the link via the route it should have taken to increase the probabilities along that route
							//we use the longest route (not necessarily the same one as from Reachable), as we want the longer routes to persist over the shorter ones
							/*std::vector<ABM::Agent*> route = GetLongestRoute(toNode, fromNode, BreedName[0], dir, 5);
							if (route.size()>0) //should always be true if Reachable worked properly above
							{
								std::cout << "LONGROUTE,"<<fromNode->Name<<","<<toNode->Name<<"-> ";
								for (int i=0; i<route.size()-1; ++i)
								{
									LinkProbTablesUpdate(a->Get<std::string>("lineCode")[0], route[i], route[i+1]);
									std::cout << route[i]->Name << " ";
								}
								std::cout << route.back()->Name << std::endl;
							}*/
						}
					}
				}
			}
		}
		LiveAgents.insert(a->Name);
		return a;
	});

	//now do a check for agents which are missing from the data here
	std::vector<ABM::Agent*> drivers = _agents.Ask("driver");
	int livecount = 0;
	for (auto aIT = drivers.begin(); aIT != drivers.end(); ++aIT) {
		if (LiveAgents.find((*aIT)->Name) == LiveAgents.end())
		{
			//TODO: need to log this
			//std::cout<<"Die: "<<(*aIT)->Name<<std::endl;
			(*aIT)->Die();
		}
		else {
			//std::cout<<"Die: "<<(*aIT)->Name<<" OK"<<std::endl;
			++livecount;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Network methods

/// <summary>
/// Go through all the links looking for ones of the form AB, BC -> A,B,C.
/// Algorithm:
/// For all nodes (A):
///   For all of A's Outlinks (B)
///     Find an outlink of A (called C) such that there exists A->C and B->C
///     Remove A->C, keeping A->B and B->C
///
/// Formally, this is finding a first neighbour of A and a second neighbour of A, both with the same destination node.
/// What you do then is to remove the shorter path and leave the second neighbour path.
/// NOTE: this is a brute force approach, you could improve efficiency using sorted maps
/// TODO: is there any clever way of doing this with spanning trees? It must be related to shortest path algorithms?
/// </summary>
/// <returns>The number of links that were removed from the graph.</returns>
int ModelMLTubeNetwork::CoalesceLinks()
{
	int NumLinksRemoved = 0;
	//Weirdly, the best method is to go through all the nodes, despite it being about links, as it's the nodes that have the outlinks on them.
	//It works better with the node iterator too, so you can delete the links without messing up the iteration.
	std::vector<ABM::Agent*> nodes = _agents.Ask("node");
	for (auto it = nodes.begin(); it != nodes.end(); ++it)
	{
		std::set<ABM::Link*> DeleteLinks; //we need to keep a list of links which need deleting at the end, otherwise it messes up the iterators

		ABM::Agent* node_A = *it;
		std::vector<ABM::Link*> AOutLinks = node_A->OutLinks();
		for (auto itFirst = AOutLinks.begin(); itFirst != AOutLinks.end(); ++itFirst) //first neighbours of A
		{
			ABM::Link* link_AB = *itFirst; //first neighbour link of A, so node A to node B
			ABM::Agent* node_B = link_AB->end2; //first neighbour destination, B
			std::vector<ABM::Link*> BOutLinks = node_B->OutLinks();
			for (auto itSecond = BOutLinks.begin(); itSecond != BOutLinks.end(); ++itSecond) //first neighbours of B, second neighbours of A
			{
				ABM::Link* link_BC = *itSecond; //first neighbour of B, second neighbour of A, node B to node C
				if (link_AB->_BreedName != link_BC->_BreedName) continue; //reject BC if it's not on the same line as AB
				if (link_AB->Get<int>("direction") != link_BC->Get<int>("direction")) continue; //reject BC if it's not in the same direction as AB
				ABM::Agent* node_C = link_BC->end2; //first neighbour destination, C
				//now do the test - if there's a direct link A->C on A's outlinks, then we can remove A->C in favour of the A->B, B->C pair we've just found
				for (auto itA = AOutLinks.begin(); itA != AOutLinks.end(); ++itA)
				{
					ABM::Link* link_AX = *itA; //A->X as we're testing for X==C here
					if (link_AB->_BreedName != link_AX->_BreedName) continue; //reject AX if it's not on the same line as AB (and BC by implication earlier)
					if (link_AB->Get<int>("direction") != link_AX->Get<int>("direction")) continue; //reject AX if it's not in the same direction as AB (and BC by implication earlier)
					if (link_AX->end2 == node_C) {
						//we've found a pattern of A->C, A->B and B->C, which allows us to remove A->C (A->X)
						//BUT, we can only do this by adding the link to remove to a list, so we can do it after the outer A loop completion, otherwise we end up looking at dead links
						std::cout << "DELETE," <<
							link_AX->end1->Name << "->" << link_AX->end2->Name << "  " <<
							link_AB->end1->Name << "->" << link_AB->end2->Name << "  " <<
							link_BC->end1->Name << "->"<<link_BC->end2->Name << std::endl;
						DeleteLinks.insert(link_AX);
						LinkProbTablesUpdateDirect(link_AB->_BreedName[0], link_AX->end1, link_AX->end2, -100000); //kill it forever...HACK!
					}
				}
			}
		}
		//now do the delete
		for (auto itd = DeleteLinks.begin(); itd != DeleteLinks.end(); ++itd) {
			_links.DeleteLink(*itd);
			++NumLinksRemoved;
		}
	}
	return NumLinksRemoved;
}

/// <summary>
/// Is agent D reachable on the network from agent O within limit links? Based on line and direction constraints.
/// NOTE: this returns the FIRST possible route, NOT the closest.
/// </summary>
/// <returns>The number of network links to reach D from O i.e. neighbour distance. Negative means it can't be reached within limit</returns>
int ModelMLTubeNetwork::Reachable(ABM::Agent* o, ABM::Agent* d, char lineCode, int direction, int limit)
{
	if (o == d) return 0; //guard case - we're there, so return zero hops
	if (limit == 0) return -1; //guard case, we've run out of hops
	std::vector<ABM::Link*> OutLinks = o->OutLinks();
	for (auto link : OutLinks) {
		if ((link->_BreedName[0]==lineCode) && (link->Get<int>("direction") == direction)) {
			int dist = Reachable(link->end2, d, lineCode, direction, limit - 1);
			if (dist >= 0) return dist + 1; //found a route on this link, so return 1 for this hop, plus however far this link to d was recursively
		}
	}
	return -1; //not reachable
}

/// <summary>
/// returns the longest list of nodes connecting origin to destination
/// TODO: it's not exactly elegant
/// </summary>
std::vector<ABM::Agent*> ModelMLTubeNetwork::GetLongestRoute(ABM::Agent* o, ABM::Agent* d, char lineCode, int direction, int limit)
{
	std::vector<ABM::Agent*> route;

	if (o == d) {
		route.push_back(d);
		return route; //guard case - we're there, so return zero hops
	}
	if (limit == 0)
		return route; //guard case, we've run out of hops
	std::vector<ABM::Link*> OutLinks = o->OutLinks();
	std::vector<ABM::Agent*> longestroute;
	for (auto link : OutLinks) {
		if ((link->_BreedName[0] == lineCode) && (link->Get<int>("direction") == direction)) {
			std::vector<ABM::Agent*> nodes = GetLongestRoute(link->end2, d, lineCode, direction, limit - 1);
			if (nodes.size() > longestroute.size()) {
				longestroute.clear();
				longestroute.push_back(o);
				for (auto node : nodes) longestroute.push_back(node);
			}
		}
	}
	if (longestroute.size() > 0) {
		for (auto node : longestroute) {
			route.push_back(node);
//			std::cout << node->Name << " "; //debug
		}
//		std::cout << std::endl; //debug
	}
	return route;
}

/// <summary>
/// Go through each node's outlinks and check for duplicates based on reachability and shortest hops.
/// This is a Super Prune as it should remove a lot of dead (duplicate) links based on geometry.
/// </summary>
/// <returns>the number deleted</returns>
int ModelMLTubeNetwork::NodePrune()
{
	int NumLinksRemoved = 0;

	std::vector<ABM::Agent*> nodes = _agents.Ask("node");
	for (auto it = nodes.begin(); it != nodes.end(); ++it)
	{
		ABM::Agent* node = *it;

		std::set<ABM::Link*> DeleteLinks; //we need to keep a list of links which need deleting at the end, otherwise it messes up the iterators

		
		std::vector<ABM::Link*> OutLinks = node->OutLinks();
		for (auto link : OutLinks)
		{
			std::string lineCode = link->_BreedName;
			int direction = link->Get<int>("direction");
			std::vector<ABM::Agent*> longestroute = GetLongestRoute(link->end1, link->end2, lineCode[0], direction, 8);
			if (longestroute.size() > 2) {
				DeleteLinks.insert(link);
				if (lineCode == "V") {
					std::cout << "Prune Link: " << lineCode << " " << link->end1->Name << " " << link->end2->Name << " " << longestroute.size() << std::endl;
					for (auto node : longestroute) std::cout << node->Name << " ";
					std::cout << std::endl;
				}

			}
			else {
				if (lineCode=="V")
					std::cout << "Keep Link: " << lineCode << " " << link->end1->Name << " " << link->end2->Name << " " << longestroute.size() << std::endl;
			}
		}

		//now do the delete
		for (auto itd = DeleteLinks.begin(); itd != DeleteLinks.end(); ++itd) {
			_links.DeleteLink(*itd);
			++NumLinksRemoved;
		}
	}
	return NumLinksRemoved;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Link probabilities

void ModelMLTubeNetwork::InitialiseLinkProbTables()
{
	int NumStations = _agents.Ask("node").size();
	//TODO: this is a bit of a hack - you could use the graph breeds
	//NOTE: these are shared pointers, so reference counted
	LinkProbTables['B'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['C'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['D'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['H'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['J'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['M'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['N'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['P'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['V'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	LinkProbTables['W'] = std::shared_ptr<float[]>(new float[NumStations, NumStations]);
	for (auto it : LinkProbTables) {
		shared_ptr<float[]> counts = it.second;
		for (int i = 0; i < NumStations; i++) {
			for (int j = 0; j < NumStations; j++) {
				counts[i, j] = 0;
			}
		}
	}
}

void ModelMLTubeNetwork::LinkProbTablesUpdate(char LineCode, ABM::Agent* o, ABM::Agent* d)
{
	int o_id = o->Get<int>("id");
	int d_id = d->Get<int>("id");
	std::shared_ptr<float[]> counts = LinkProbTables[LineCode];
	counts[o_id, d_id] = counts[o_id, d_id] + 1;
}

//allow writing a count directly into the table so we can manually force links that are removed
void ModelMLTubeNetwork::LinkProbTablesUpdateDirect(char LineCode, ABM::Agent* o, ABM::Agent* d,int count)
{
	int o_id = o->Get<int>("id");
	int d_id = d->Get<int>("id");
	std::shared_ptr<float[]> counts = LinkProbTables[LineCode];
	counts[o_id, d_id] = count;
}

void ModelMLTubeNetwork::WriteLinkProbTables()
{
	//write out to disk
	//TODO:
}

int ModelMLTubeNetwork::CullLinkProbTables()
{
	int deletecount = 0;
	//first, make a lookup table to link the node id number to its name
	std::map<int, ABM::Agent*> IdNameLookup;
	std::vector<ABM::Agent*> nodes = _agents.Ask("node");
	for (auto it : nodes) {
		IdNameLookup[it->Get<int>("id")] = it;
	}

	//remove low probability links - two pass algorithm, first count up the total count, then use this to work out probabilities and cull below threshold
	for (auto it = LinkProbTables.begin(); it != LinkProbTables.end(); ++it)
	{
		char lineCode = it->first;
		shared_ptr<float[]> counts = it->second;
		float sum = 0, max=0;
		for (int i = 0; i < NumStations; ++i) {
			for (int j = 0; j < NumStations; ++j) {
				sum += counts[i, j];
				if (counts[i, j] > max) max = counts[i, j];
			}
		}

		//guard case - if there are no links you'll get a divide by zero on the probability, so short circuit this (NOTE: you can write a negative sum directly)
		if (sum <= 0) continue;

		float threshold = max / sum * 0.5f;
		std::cout << "threshold=" << threshold << " max=" << max << std::endl;

		//now removal of links
		for (int i = 0; i < NumStations; ++i) {
			for (int j = 0; j < NumStations; ++j) {
				float prob = counts[i, j] / sum;
				if ((counts[i, j] >0)&&(prob<threshold)) { //no point looking for a link to delete if the count is zero	
					//find the station nodes with the ids i and j, then use node i's outlinks to help find the ij link
					//NOTE: there might not actually be one
					ABM::Agent* from = IdNameLookup[i];
					ABM::Agent* to = IdNameLookup[j];
					std::vector<ABM::Link*> OutLinks = from->OutLinks();
					for (auto link : OutLinks) {
						if (link->end2 == to) _links.DeleteLink(link);
						++deletecount;
					}
				}
			}
		}
	}
	return deletecount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Logging

/*void ModelMLTubeNetwork::NetworkTransitionLog()
{
//log all link transitions so we can analyse offline

}*/

/// <summary>
/// Log the state of the network to file for machine learning purposes.
/// Write out the state of the network as of AnimationDataDT time and 0..29s and 30..59s
/// </summary>
void ModelMLTubeNetwork::MLLog(AgentTime& DataDT)
{
	std::ofstream out_log("ml_tubenetwork.txt", std::ios::app);

	//dayofweek,hour,minute,30sec -> [300 stations]*(10 lines?)

	//write out data here as one hot
	int dayofweek = DataDT.GetDayOfWeek();
	int hour, minute, second;
	DataDT.GetTimeOfDay(hour, minute, second);
	int s30 = 0;
	if (second >= 30) s30 = 1; //[1,0] means 0..29 seconds, [0,1] means 30..59 seconds
	//work out time range that we're looking for (seconds)
	float minsecs = s30 * 30, maxsecs = minsecs + 30; //so [0,30] and [30,60] for 0..29 and 30..59
	float onehot_time[93]; //7+24+60+2 = 93
	for (int i = 0; i < 93; ++i) onehot_time[i] = 0;
	onehot_time[dayofweek] = 1.0;
	onehot_time[7 + hour] = 1.0;
	onehot_time[7 + 24 + minute] = 1.0;
	onehot_time[7 + 24 + 60] = 0.0; //set both 30s slots to zero, then set the correct one hot
	onehot_time[7 + 24 + 60 + 1] = 0.0;
	onehot_time[7 + 24 + 60 + s30] = 1.0;

	//this is our output data array
	float* stationstate = new float[NumStations];
	for (int i = 0; i < NumStations; ++i) stationstate[i] = 0.0; //zero all the station states

	//go through all the agents and any eta between now and the next 30s increment counts as at station and records a passing point for this time
	int NumTubes = 0;
	std::vector<ABM::Agent*> drivers = _agents.Ask("driver");
	for (auto it : drivers)
	{
		AgentTime eta;
		eta._DT = (time_t)it->Get<int>("eta");
		eta._fraction = 0;
		float deltasecs = AgentTime::DifferenceSeconds(eta, DataDT);
		if ((deltasecs >= minsecs) && (deltasecs < maxsecs)) {
			//log the station as having a train present
			ABM::Agent* toNode = it->Get<ABM::Agent*>("toNode");
			int id = toNode->Get<int>("id");
			stationstate[id] = 1.0;
			++NumTubes;
		}
	}

	//now log the data
	out_log << AgentTime::ToStringYYYYMMDD_hhmmss(DataDT) << "," << drivers.size()<<","<<NumTubes << ",";
	for (int i = 0; i < 93; ++i) out_log << onehot_time[i] << ",";

	out_log << "  "; //fiddle so I can see where the stations data start in the file

	for (int i = 0; i < NumStations; ++i) out_log << stationstate[i] << ",";

	delete[] stationstate;

	out_log << std::endl;
	out_log.close();

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ABM

void ModelMLTubeNetwork::Setup()
{
	//AgentScript: agentBreeds "nodes" "drivers"
	//My version, closer to NetLogo: Breed("node","nodes"); and Breed("driver","drivers");
	SetDefaultShape("node", "sphere"); //or cube?
	SetDefaultColour("node", glm::vec3(1.0f, 1.0f, 1.0f));
	SetDefaultSize("node", StationSize);
	SetDefaultShape("driver", "none" /*"turtle"*/);
	SetDefaultSize("driver", TrainSize);
	//need to create breed default colours here as these are used for the links - the actual colour on the ink isn't used
	SetDefaultColour("B", LineCodeToVectorColour('B'));
	SetDefaultColour("C", LineCodeToVectorColour('C'));
	SetDefaultColour("D", LineCodeToVectorColour('D'));
	SetDefaultColour("H", LineCodeToVectorColour('H'));
	SetDefaultColour("J", LineCodeToVectorColour('J'));
	SetDefaultColour("M", LineCodeToVectorColour('M'));
	SetDefaultColour("N", LineCodeToVectorColour('N'));
	SetDefaultColour("P", LineCodeToVectorColour('P'));
	SetDefaultColour("V", LineCodeToVectorColour('V'));
	SetDefaultColour("W", LineCodeToVectorColour('W'));

	LoadStations(Filename_StationCodes); //station locations
	InitialiseLinkProbTables(); //tables of link probabilities - must be done after the stations have been created as need the number of stations

	//Setup time
	//AnimationDataDT = AgentTime::FromString("20170101_000000");
	//AnimationDataDT = AgentTime::FromString("20170201_000000");
	//AnimationDataDT = AgentTime::FromString("20170301_000000");
	//AnimationDataDT = AgentTime::FromString("20170401_000000");
	AnimationDataDT = AgentTime::FromString("20170501_000000");
	
	//Setup training epoch number
	epoch = 0;

	//ABM method of creating links in the 3D scene graph
	_links.Create3D(_links._pSceneRoot); //TODO: need more elegant way of calling this

	//TEST
	/*ABM::Agent* VUX = _agents.With("name", "VUX")[0];
	ABM::Agent* PIM = _agents.With("name", "PIM")[0];
	ABM::Agent* VIC = _agents.With("name", "VIC")[0];
	ABM::Agent* GPK = _agents.With("name", "GPK")[0];
	ABM::Agent* OXC = _agents.With("name", "OXC")[0];
	ABM::Link* L1 = _links.CreateLink("V", VUX, PIM);
	L1->Set<std::string>("lineCode", "V");
	L1->Set<int>("direction", 0);
	ABM::Link* L2 = _links.CreateLink("V", PIM, VIC);
	L2->Set<std::string>("lineCode", "V");
	L2->Set<int>("direction", 0);
	ABM::Link* L3 = _links.CreateLink("V", VIC, GPK);
	L3->Set<std::string>("lineCode", "V");
	L3->Set<int>("direction", 0);
	ABM::Link* L4 = _links.CreateLink("V", GPK, OXC);
	L4->Set<std::string>("lineCode", "V");
	L4->Set<int>("direction", 0);
	std::vector<ABM::Agent*> route = GetLongestRoute(VUX, OXC, 'V', 0, 10);
	for (auto node : route) {
		std::cout << node->Name << " ";
	}
	std::cout << endl;*/
	//END TEST
};

void ModelMLTubeNetwork::Step(double ticks)
{
	++epoch;
	std::cout << "Epoch=" << epoch << std::endl;

	std::cout<<AgentTime::ToStringYYYYMMDD_hhmmss(AnimationDataDT)<<std::endl;
	std::string Filename = ModelMLTubeNetwork::Filename_AnimationDir + AgentTime::ToFilePath(AnimationDataDT) +"/trackernet_"+ AgentTime::ToStringYYYYMMDD_hhmmss(AnimationDataDT)+".csv";
	LoadPositions(Filename);
	
	/*if (epoch%50==0) {
		//int count = CoalesceLinks(); //fuse missed connections down
		//while (count > 0) count = CoalesceLinks();
		//int ccount = CullLinkProbTables();
		int ncount = NodePrune();
	}*/

	//log 3 mins of train/station state date in 30s intervals before adding 3 mins to the counter to get the next data frame
	AgentTime DataDT = AnimationDataDT;
	for (int i = 0; i < 6; ++i)
	{
		MLLog(DataDT);
		DataDT.Add(30);
	}

	AnimationDataDT.Add(3 * 60);

};
