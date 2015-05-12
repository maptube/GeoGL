/*
 * model_busnetwork.cpp
 *
 *  Created on: 4 May 2015
 *      Author: richard
 */

#include "model_busnetwork.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "json/json.h"
#include "json/reader.h"

#include "ellipsoid.h"
#include "netgraphgeometry.h"

#include "Link.h"
#include "Agent.h"



//define locations of configuration files
const std::string ModelBusNetwork::Filename_StopCodes =
		"../data/countdown/instant_V1.json"; //bus stop locations
const std::string ModelBusNetwork::Filename_BusRoutesNetwork =
		"../data/countdown/TfL_Bus_routes_stream_20150504.csv"; //network of bus routes based on lists of stops
//const std::string ModelTubeNetwork::Filename_TrackernetPositions =
//		/*"data/trackernet_20140127_154200.csv";*/
//		"../data/trackernet_20140127_154200.csv"; //train positions
//const std::string ModelTubeNetwork::Filename_AnimationDir =
//		"../data/tube-anim-strike/28";
//const float ModelTubeNetwork::LineSize = 25; //size of track - was 50
//const int ModelTubeNetwork::LineTubeSegments = 10; //number of segments making up the tube geometry
//const float ModelTubeNetwork::StationSize = 100.0f; //size of station geometry object
//const float ModelTubeNetwork::TrainSize = 300.0f; //size of train geometry object

ModelBusNetwork::ModelBusNetwork(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph) {
	// TODO Auto-generated constructor stub

}

ModelBusNetwork::~ModelBusNetwork() {
	// TODO Auto-generated destructor stub
}

void ModelBusNetwork::Setup() {
	//from ABM::Model
	SetDefaultShape("node","cube");
	SetDefaultSize("node",100.0f/*StationSize*//*0.001f*/);
	SetDefaultShape("driver","turtle");
	//SetDefaultSize("driver",TrainSize/*0.005f*/);

	LoadBusStops(Filename_StopCodes); //station locations
	LoadLinks(Filename_BusRoutesNetwork); //network from CSV lists of stops making up each route

	//now create the 3D representation from the data just loaded
	_links.Create3D(_links._pSceneRoot); //TODO: need more elegant way of calling this
}

void ModelBusNetwork::Step(double Ticks) {
	//from ABM::Model
}

/// <summary>
/// Load the bus stop location and code information from the stream json file
/// </summary>
/// <param name="Filename">The local fully qualified path of the json file to load instant_V1.json from.</param>
void ModelBusNetwork::LoadBusStops(const std::string& Filename)
{
	Json::Value root;   // will contain the root value after parsing.
	Json::Reader reader;

	//[4,"1.0",1430768485304]
	//[0,"Gospel Oak Station","34506","72579","490001119W","STBC",242,"GP",0,51.55475,-0.151763]
	//[0,"Scarborough Road","33493","47987","490011941N","STBR",341,null,0,51.637767,-0.046232]
	//Index positions in the file (above)
	const Json::UInt PosID=0, PosName=1, PosCode1=2, PosCode2=3, PosLat=9, PosLon=10; //bearing=6?

	int AgentCount=0; //count how many we create
	//OK, we all know it's not valid json, so read line by line and use the json parser on each individually
	std::ifstream in_json(Filename.c_str());
	if (in_json.is_open()) {
		std::string line;
		while (!in_json.eof()) {
			line.clear();
			std::getline(in_json,line);
			if (line.length()==0) continue; //blank line
			if (reader.parse(line,root)) {
				const Json::Value& id = root[PosID];
				if (id==0) { //id==4 is the header record
					//take the line apart here and create the bus stop nodes
					//NOTE: might need to create my own lookup table of codes here as the tube model does?
					//NOTE2: might need the bearing?
					std::string Name = root[PosName].asString();
					std::string Code = root[PosCode1].asString();
					float Lat = (float)root[PosLat].asDouble();
					float Lon = (float)root[PosLon].asDouble();
					glm::vec3 P = _pEllipsoid->toVector(glm::radians(Lon),glm::radians(Lat),0);
					//create the node agent
					std::vector<ABM::Agent*> AList = PatchesPatchXYSprout((int)P.x,(int)P.y,1,"node");
					//get our newly created agent and set his properties
					ABM::Agent *a = AList.front();
					a->SetXYZ(P.x,P.y,P.z);
					a->Name=Code;
					++AgentCount;
					//std::cout<<"ModelBusNetwork::LoadBusStops created agent: "<<Code<<std::endl;
				}
			}
			//else failure to read line
		}
	}
	else {
		std::cerr  << "ModelBusNetwork::LoadBusStops failed to parse file: " << Filename <<std::endl;
	}
	std::cout<<"ModelBusNetwork::LoadBusStops created "<<AgentCount<<" new agents"<<std::endl;
}

void ModelBusNetwork::LoadLinks(const std::string& RoutesFilename)
{
	//todo: this is the csv file containing the stops on each bus route
	std::ifstream in_csv(RoutesFilename.c_str());
	//Route,Run,Sequence,Stop_Code_LBSL,Bus_Stop_Code,Naptan_Atco,Stop_Name,Location_Easting,Location_Northing,Heading,Virtual_Bus_Stop
	//1,1,1,14456,53369,490000235Z,NEW OXFORD STREET,529998,181428,74,0
	int CountLinks = 0; //counter for how many links we have created
	int CountMissingNodes = 0; //counter for how many stop codes we find that are missing
	if (in_csv.is_open()) {
		std::string line;
		std::getline(in_csv,line); //skip the header line
		ABM::Agent* agent_o = nullptr; //origin agent (once it gets set)
		std::string LastRoute="";
		int LastRun=-1;
		while (!in_csv.eof()) {
			//if (CountLinks>5000) break; //HACK!!!!! Limit number of links loaded
			line.clear();
			std::getline(in_csv,line);
			if (line.length()==0) continue; //blank line
			//parse the line as a string stream separated by ,
			std::stringstream ss(line);
			std::vector<std::string> items;
			while (ss.good()) {
				std::string elem;
				std::getline(ss,elem,',');
				items.push_back(elem);
			}
			if (items.size()>=11) {
				//need route, (run), (sequence), Stop_Code or BusStopCode?, Heading
				//create the new link here
				std::string Route = items[0];
				int Run = std::stoi(items[1]); //run is generally 1 or 2, but you do get very occasional 3 and 4
				int Sequence = std::stoi(items[2]); //1,2,3... stop number in sequence
				std::string Code1 = items[3]; //both code 1 and code 2 appear in the instant_V1.json file, but the agents are created using Code1
				int Bearing = std::stoi(items[9]);
				//lookup the destination link using its stop code name
				std::vector<ABM::Agent*> agents_d = _agents.With("name",Code1);
				if (agents_d.size()==0) {
					//agent_d not found, so skip the link and report it
					std::cerr<<"ModelBusNetwork::LoadLinks agent name "<<Code1<<" not found"<<std::endl;
					++CountMissingNodes;
				}
				else {
					ABM::Agent* agent_d = agents_d.front();
					//test for this being a different Route or Run (NOTE: could also test Sequence!=Sequence+1)
					if ((Route!=LastRoute)||(Run!=LastRun)) {
						//different route or run, so start a new chain of connections
						agent_o=nullptr;
					}
					else {
						//continue chain from previous link if agent_o is defined, otherwise just make the new agent_d to start a new chain
						if (agent_o!=nullptr) { //null if this is the first node of a new chain and we can't create a link until we have two nodes
							ABM::Link* L = _links.CreateLink(LastRoute,agent_o,agent_d);
							++CountLinks;
							//this is where you set the velocity, linecode, direction etc
							L->Set<std::string>("route",Route);
							L->Set<int>("bearing",Bearing);
							L->colour=glm::vec3(1.0f,0.0f,0.0f); //red, obviously TODO: check what the TfL official red is
							//Don't know runlinks for buses, but you get the information from the stream
							//TODO: probably need to create a dummy velocity here?
							//L->Set<float>("runlink",(float)r);
							//now add a pre-created velocity for this link based on distance and runlink in seconds
							//double dx = L->end2->xcor()-L->end1->xcor();
							//double dy = L->end2->ycor()-L->end1->ycor();
							//double dz = L->end2->zcor()-L->end1->zcor();
							//double dist = sqrt(dx*dx+dy*dy+dz*dz);
							//L->Set<float>("velocity",(float)(dist/(float)r));
							//std::cout<<"Created Link: "<<Route<<" "<<Run<<" "<<Sequence<<" "<<CountLinks<<std::endl;
						}
					}
					//move along the OD chain
					LastRoute=Route;
					LastRun=Run;
					agent_o=agent_d;
				}
			}
		}
		in_csv.close();
		//std::cout<<"Number of Links: "<<_links.NumLinks()<<std::endl;
	}
	std::cout<<"ModelBusNetwork::LoadLinks created "<<CountLinks<<" new links with "<<CountMissingNodes<<" missing nodes"<<std::endl;
}


