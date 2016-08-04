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
#include <string>
#include <algorithm>

//#include <math.h>

#include "json/json.h"
//#include "json/reader.h"

#include "ellipsoid.h"
#include "netgraphgeometry.h"

#include "abm/Link.h"
#include "abm/Agent.h"
#include "abm/agenttime.h"
#include "abm/utils.h"
#include "abm/sensor/geofence.h"



//define locations of configuration files
const std::string ModelBusNetwork::Filename_StopCodes =
		"../data/countdown/instant_V1.json"; //bus stop locations
const std::string ModelBusNetwork::Filename_BusRoutesNetwork =
		"../data/countdown/TfL_Bus_routes_stream_20150504.csv"; //network of bus routes based on lists of stops
const std::string ModelBusNetwork::LogFilename =
		"/home/richard/modelbusnetwork.txt";
//const std::string ModelBusNetwork::Filename_Positions =
//		"../data/countdown/2014/1/1/countdown_20140101_090000.csv";
//const std::string ModelTubeNetwork::Filename_AnimationDir =
//		"../data/tube-anim-strike/28";
//const float ModelTubeNetwork::LineSize = 25; //size of track - was 50
//const int ModelTubeNetwork::LineTubeSegments = 10; //number of segments making up the tube geometry
//const float ModelTubeNetwork::StationSize = 100.0f; //size of station geometry object
//const float ModelTubeNetwork::TrainSize = 300.0f; //size of train geometry object

const std::string ModelBusNetwork::Filename_Positions =
		"../data/countdown/";
		//"/run/media/richard/SAMSUNG2/countdown-cache/";

ModelBusNetwork::ModelBusNetwork(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph) {
	// TODO Auto-generated constructor stub

}

ModelBusNetwork::~ModelBusNetwork() {
	// TODO Auto-generated destructor stub
}

void ModelBusNetwork::Setup() {
	//from ABM::Model
	//nodes are the route stop points
	SetDefaultShape("node","none"); //was cube
	SetDefaultSize("node",10.0f/*StationSize*//*0.001f*/);
	//drivers are the buses
	SetDefaultShape("driver","none"); //was turtle
	SetDefaultSize("driver",400.0f/*0.005f*/);
	//TODO: SetDefaultColour("driver",glm::dvec3(1.0,0,0));
	//TODO: need to be able to set the links to no mesh as well.

	AnimationDT = AgentTime::FromString("20140101_000000");
	NextTimeDT = AgentTime::FromString("20140101_000000");
	SensorRecordStartDT = AgentTime::FromString("20140101_000000");

	LoadBusStops(Filename_StopCodes); //station locations
	LoadLinks(Filename_BusRoutesNetwork); //network from CSV lists of stops making up each route
/*	LoadAgentsCSV(Filename_Positions,1,[this](std::vector<std::string> items) {
		//route, destination, vehicleid, registration, tripid, lat, lon, east, north, bearing, expectedtime(utc), timetostation(secs), linkruntime, detailsstopcode, fromstopcode, tostopcode
		//"452","Kensal Rise","16069","LJ56VTY","484640","51.51208","-0.202189","524741.4","180825.1","336","31/12/2013 23:59:06","90","78","11533","11531","11533"
		//so, 452 is the bus route number, buses are created as drivers
		ABM::Agent* a = _agents.Hatch("driver");
		a->SetColour(glm::vec3(1,0,0)); //it's a bus, better make it red
		//set other agent properties here
		std::string route = items[0];
		std::string registration = items[3];
		int Bearing = std::stoi(items[9]);
		std::string DetailsStopCode = items[13];

		//it seems that we have an estimated arrival time and number of seconds to station (stop). Why? In the example line above, the seconds to arrival is greater than the link runtime.
		//I'm going to use the expected arrival time and calculate a velocity from that.
		AgentTime ExpectedTime = AgentTime::FromString2(items[10]); //this is the dd/mm/yy hh:mm:ss format
		//TODO: need a function to get seconds difference from these two:
		float DeltaTime = AgentTime::DifferenceSeconds(ExpectedTime,AnimationDT); //this is how many seconds until it reaches the details stop code
		//float TimeToStop = std::stof(items[11]); //should be same as delta time?
		float LinkRuntime = std::stof(items[12]);
		float Lambda = DeltaTime/LinkRuntime; //Lambda factor along fromNode to toNode links
		if (Lambda<0) Lambda=0; //clamp to 0..1 to prevent agent from straying off link
		else if (Lambda>1) Lambda=1;
		//TODO: how about warning when the clamping has had to be used? it's probably a traffic jam issue

		a->Name = registration; //unique name to match up to next data download (licence number) (or vehicle id or trip id?)
		a->Set<std::string>("route",route);

		//stops have different numbers for each side of the road (route direction), but can be shared between routes
		std::vector<ABM::Agent*> toNodes = _agents.With("name",DetailsStopCode); //query the node that we have the details for
		if (toNodes.empty()) {
			//the details stop code doesn't exist in the database, so drop the agent and flag the error
			std::cerr<<"Agent toNode not found in database: DetailsStopCode="<<DetailsStopCode<<" Route="<<route<<" Name="<<a->Name<<std::endl;
			a->Die(); //this isn't a good thing to be doing as it messes up the numbers and the birth/death rate
			a=nullptr;
		}
		else {
			ABM::Agent* toNode = toNodes.front();
			ABM::Agent* fromNode = nullptr;
			//then look at all the inlinks for a previous node on the correct route
			std::vector<ABM::Link*> links = toNode->InLinks();
			for (std::vector<ABM::Link*>::iterator it = links.begin(); it!=links.end(); it++) {
				ABM::Link* l = *it;
				//todo: can you check that two different directions for a single route don't meet at the same point?
				if (l->Get<std::string>("route")==route) {
					fromNode = l->end1; //this is the agent node (stop) at the beginning of the link that ends at the destination stop
					break;
				}
			}

			if (fromNode!=nullptr) {
				//position between two nodes
				ABM::Utils::LinearInterpolate(a,Lambda,fromNode,toNode);
				a->Set<ABM::Agent*>("toNode",toNode);
				a->Set<ABM::Agent*>("fromNode",fromNode);
				a->Set<float>("v",0.1f);
				a->Face(*a->Get<ABM::Agent*>("toNode"));
			}
			else {
				//position at toNode - probably starting point of route?
				glm::dvec3 P = toNode->GetXYZ();
				a->SetXYZ(P.x,P.y,P.z);
				a->Set<ABM::Agent*>("toNode",toNode);
				a->Set<float>("v",0.1f);
				//TODO: need to face agent in the correct direction - how? pick out link of toNode?
				//USE THE BEARING!!!!!
			}

		}

		return a;
	});*/

	//now create the 3D representation from the data just loaded
	_links.Create3D(_links._pSceneRoot); //TODO: need more elegant way of calling this
}

void ModelBusNetwork::WriteLogBusNumbers(const AgentTime& DateTime)
{
	//write out the total number of bus agents counted at this point in time
	std::string TimeCode = AgentTime::ToStringYYYYMMDD_hhmmss(DateTime);
	std::vector<ABM::Agent*> buses = _agents.Ask("driver");
	std::ofstream out_log(ModelBusNetwork::LogFilename,std::ios::app);
	out_log<<TimeCode<<","<<buses.size()<<std::endl;
	out_log.close();
}

void ModelBusNetwork::WriteLogBusGeoFencedNumbers()
{
	//write out the current value of the geofence sensor placed at the MY1 position on Marylebone Road
	std::ofstream out_log(ModelBusNetwork::LogFilename,std::ios::app);
	//TODO: need better way of doing this - need to write out information from sensor before clearing them
	int Counter = _sensors[0]->Counter; //OK, this is a complete HACK!
	//to the screen so you can see what's happening
	std::cout<<"GeoFence:, "<<AgentTime::ToString(SensorRecordStartDT)<<", "<<AgentTime::ToString(AnimationDT)<<", "<<Counter<<std::endl;
	//and to the file log so you have some data
	out_log<<"GeoFence:, "<<AgentTime::ToString(SensorRecordStartDT)<<", "<<AgentTime::ToString(AnimationDT)<<", "<<Counter<<std::endl;
	out_log.close();
}

void ModelBusNetwork::UpdateAgents(const AgentTime& DateTime) {
	std::string DirPart = AgentTime::ToFilePath(DateTime);
	std::string TimeCode = AgentTime::ToStringYYYYMMDD_hhmmss(DateTime);
	std::string Filename = Filename_Positions+DirPart+"/countdown_"+TimeCode+".csv";
	std::cout<<"ModelBusNetwork::UpdateAgents : "<<Filename<<std::endl;
	std::set<std::string> LiveAgentNames; //TODO: why not set<Agent*> ?
	LoadAgentsCSV(Filename,1,[&](std::vector<std::string> items) { //capture was [this]
		//the general idea is to move all the agent creation code from setup and put it all here...

		////DUPLICATED CODE - originally identical to the setup code
		//route, destination, vehicleid, registration, tripid, lat, lon, east, north, bearing, expectedtime(utc), timetostation(secs), linkruntime, detailsstopcode, fromstopcode, tostopcode
		//"452","Kensal Rise","16069","LJ56VTY","484640","51.51208","-0.202189","524741.4","180825.1","336","31/12/2013 23:59:06","90","78","11533","11531","11533"
		//so, 452 is the bus route number, buses are created as drivers
		std::string route = items[0];
		std::string registration = items[3];
		int Bearing = std::stoi(items[9]);
		std::string DetailsStopCode = items[13];
		//it seems that we have an estimated arrival time and number of seconds to station (stop). Why? In the example line above, the seconds to arrival is greater than the link runtime.
		//I'm going to use the expected arrival time and calculate a velocity from that.
		AgentTime ExpectedTime = AgentTime::FromString2(items[10]); //this is the dd/mm/yy hh:mm:ss format
		float LinkRuntime = std::stof(items[12]);

		LiveAgentNames.insert(registration); //TODO: why not use the agent pointer?
		ABM::Agent* a;
		std::vector<ABM::Agent*> agents = _agents.With("name",registration);
		if (agents.size()>0) {
			//this agent already exists, so move him (later)
			a=agents.front();
		}
		else {
			//no agent with this registration, so hatch a new one
			a = _agents.Hatch("driver");
			a->Name = registration; //unique name to match up to next data download (licence number) (or vehicle id or trip id?)
			a->Set<std::string>("route",route);
			a->SetColour(glm::vec3(1,0,0)); //it's a bus, better make it red
		}
		//OK, "a" now contains our agent, so we had better position him properly


		float DeltaTime = AgentTime::DifferenceSeconds(ExpectedTime,AnimationDT); //this is how many seconds until it reaches the details stop code

		float Lambda = DeltaTime/LinkRuntime; //Lambda factor along fromNode to toNode links
		if (Lambda<0) Lambda=0; //clamp to 0..1 to prevent agent from straying off link
		else if (Lambda>1) Lambda=1;
		//TODO: how about warning when the clamping has had to be used? it's probably a traffic jam issue

		//stops have different numbers for each side of the road (route direction), but can be shared between routes
		std::vector<ABM::Agent*> toNodes = _agents.With("name",DetailsStopCode); //query the node that we have the details for
		if (toNodes.empty()) {
			//the details stop code doesn't exist in the database, so drop the agent and flag the error
//			std::cerr<<"Agent toNode not found in database: DetailsStopCode="<<DetailsStopCode<<" Route="<<route<<" Name="<<a->Name<<std::endl;
			a->Die(); //this isn't a good thing to be doing as it messes up the numbers and the birth/death rate
			a=nullptr;
		}
		else {
			ABM::Agent* toNode = toNodes.front();
			ABM::Agent* fromNode = nullptr;
			//then look at all the inlinks for a previous node on the correct route
			std::vector<ABM::Link*> links = toNode->InLinks();
			for (std::vector<ABM::Link*>::iterator it = links.begin(); it!=links.end(); it++) {
				ABM::Link* l = *it;
				//todo: can you check that two different directions for a single route don't meet at the same point?
				if (l->Get<std::string>("route")==route) {
					fromNode = l->end1; //this is the agent node (stop) at the beginning of the link that ends at the destination stop
					break;
				}
			}

			if (fromNode!=nullptr) {
				//position between two nodes
				ABM::Utils::LinearInterpolate(a,Lambda,fromNode,toNode);
				a->Set<ABM::Agent*>("toNode",toNode);
				a->Set<ABM::Agent*>("fromNode",fromNode);
				a->Set<float>("v",10.0f/*0.1f*/);
				a->Face(*a->Get<ABM::Agent*>("toNode"));
			}
			else {
				//position at toNode - probably starting point of route?
				glm::dvec3 P = toNode->GetXYZ();
				a->SetXYZ(P.x,P.y,P.z);
				a->Set<ABM::Agent*>("toNode",toNode);
				a->Set<float>("v",10.0f/*0.1f*/);
				//TODO: need to face agent in the correct direction - how? pick out link of toNode?
				//USE THE BEARING!!!!!
			}

		}

		return a;

	});

	//go through all agents and any not in the live list need to be killed off
	std::vector<ABM::Agent*> drivers = _agents.Ask("driver");
	for (std::vector<ABM::Agent*>::iterator dIT=drivers.begin(); dIT!=drivers.end(); ++dIT)
	{
		ABM::Agent* a = *dIT;
		if (LiveAgentNames.find(a->Name)==LiveAgentNames.end()) {
			//agent name not on the live list, so we need to get rid of him
			a->Die();
		}
	}
}

void ModelBusNetwork::Step(double Ticks) {
	//from ABM::Model
	//TODO: really need to check the tick speed - globe is passing time*10!
	float AnimSpeed = 0.4f; //0.4f; //ticks are the time between the last update and this one in seconds, so AnimSpeed is the real time to animation time multiplier (2= 2 times normal speed)

	//AnimationDT.Add(AnimSpeed * Ticks);
	//AnimationDT.Add(180); //HACK to step it by 3 min frames

	//additional time constraints
	AnimationDT.Add(0.1/*60*/); //need to make sure I jump it by constant steps to keep the time in sync with the +-10 mins window
	//OK, so we only want to simulate for 10 minutes either side of the hour
	int Hour, Minute, Second;
	AnimationDT.GetTimeOfDay(Hour,Minute,Second);
	if (Minute==10) {
		WriteLogBusGeoFencedNumbers();
		AnimationDT.Add(40*60); //if we're at 10 past the hour, then add 40 minutes to get to 10 to the next hour
		NextTimeDT.Add(40*60); //add 40 mins to next to keep it in step
		ClearAllSensors(); //and clear the sensors
		SensorRecordStartDT = AnimationDT; //make a nore of when we started the sensor
	}
	//end of additional time constraints section

	std::cout<<AgentTime::ToString(AnimationDT)<<" "<<AgentTime::ToString(NextTimeDT)<<std::endl;
	if (AnimationDT>=NextTimeDT) {
		//need to do an update, otherwise just let everything else keep running
		NextTimeDT.Add(180); //move next frame time up by 3 mins
		//load next csv file
		UpdateAgents(NextTimeDT);
	}
	else {

		//TODO: code here to do the animation updates
		std::vector<ABM::Agent*> drivers = _agents.Ask("driver"); //should be drivers really, but haven't implemented plural breeds yet
		for (std::vector<ABM::Agent*>::iterator dIT = drivers.begin(); dIT!=drivers.end(); ++dIT)
		{
			ABM::Agent* d = *dIT;
			std::string temp = d->Get<std::string>("Name");
			//UniqueAgents.insert(temp);

			ABM::Agent* toNode = d->Get<ABM::Agent*>("toNode");
			d->Face(*toNode);
			d->Forward(std::min(d->Get<float>("v") * AnimSpeed,(float)d->Distance(*toNode)));

			if (.00001f > d->Distance(*toNode))
			{
				//decision point - the destination node has been reached, we need to find another one
				//std::cout<<"agent reached node"<<std::endl;
				std::string route = d->Get<std::string>("route");
				std::vector<ABM::Link*> OutLinks = toNode->OutLinks();
				//TODO: need to implement a Links::With route==myroute lookup function to improve this bit
				for (std::vector<ABM::Link*>::iterator itL = OutLinks.begin(); itL!=OutLinks.end(); itL++) {
					ABM::Link* L = *itL;
					std::string LinkRoute = L->Get<std::string>("route"); //L->With("route",route)????
					if (LinkRoute==route) { //NOTE: they use the bearing to get the correct link
						d->Set<ABM::Agent*>("fromNode",toNode);
						d->Set<ABM::Agent*>("toNode",L->end2);
						//found=true???
						break;
					}
				}
				//if found not true, has the driver got to the end of the line?
				//NOTE: velocity not changed here - it should be calculated on the basis of the distance and time when a new frame is loaded
			}

		}
	}
	SensorTests(); //force any sensors to update - TODO: need to find better way of doing this
	WriteLogBusNumbers(AnimationDT);
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


