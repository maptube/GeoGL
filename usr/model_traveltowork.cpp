/*
 * model_traveltowork.cpp
 *
 *  Created on: 23 Jan 2016
 *      Author: richard
 */

#include "model_traveltowork.h"

#include <random>

#include "abm/Link.h"
#include "abm/Agent.h"
//#include "Model.h"
#include "agentobject.h"

//define locations of configuration files
const std::string ModelTravelToWork::Filename_MSOANodes =
		"../data/census/MSOA_2011_EW_PWC_COORD_V2.CSV";
const std::string ModelTravelToWork::Filename_TravelToWorkOD =
		"../data/census/wu03ew_msoa.csv";

ModelTravelToWork::ModelTravelToWork(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph) {
	// TODO Auto-generated constructor stub
}

ModelTravelToWork::~ModelTravelToWork() {
	// TODO Auto-generated destructor stub
}

void ModelTravelToWork::Setup() {
	SetDefaultShape("worker","turtle");
	SetDefaultSize("worker",5000);
	SetDefaultShape("msoa","none"); //was cube
	SetDefaultSize("msoa",100);
	NumberOfAgents=5000;


	//now load the data - msoa node positions and origin destination data
	LoadAgentsCSV(ModelTravelToWork::Filename_MSOANodes,1,[this](std::vector<std::string> items) {
		//MSOA11CD	MSOA11NM	BNGEAST	BNGNORTH	LONGITUDE	LATITUDE
		//E02002536	Stockton-on-Tees 002	445583	524174	-1.29575861	54.61067865
				ABM::Agent* a = _agents.Hatch("msoa");
				a->Name = items[0];
				double Lat = std::stof(items[5]);
				double Lon = std::stof(items[4]);
				SetGeodeticPosition(a,Lat,Lon,0);
				return a;
			});

	//need to compile a list of unique trips here, each with a number which becomes a probability
	//Area of residence	Area of workplace	All categories: Method of travel to work	Work mainly at or from home	Underground, metro, light rail, tram	Train	Bus, minibus or coach	Taxi	Motorcycle, scooter or moped	Driving a car or van	Passenger in a car or van	Bicycle	On foot	Other method of travel to work
	//E02000001	E02000001	1506	0	73	41	32	9	1	8	1	33	1304	4
	TotalTripCount = 0;
	LoadAgentsCSV(ModelTravelToWork::Filename_TravelToWorkOD,1,[this](std::vector<std::string> items) {
					//ABM::Agent* a = _agents.Hatch("worker");
					//a->Name = items[3];
					//a->Set<std::string>("route",items[0]);
					//double Lat = std::stof(items[5]);
					//double Lon = std::stof(items[6]);
					//SetGeodeticPosition(a,Lat,Lon,0);
					//return a;
		std::string Origin = items[0];
		std::string Destination = items[1];
		//reject any trips that are OD... etc, which aren't MSOAs e.g. Scotland
		if ((Origin[0]!='S')&&(Destination[0]!='S')&&(Origin[0]!='N')&&(Destination[0]!='N')&&(Origin[0]!='O')&&(Destination[0]!='O')) {
			int AllTrips = std::stoi(items[2]);
			TotalTripCount+=AllTrips;
			ODData odd(Origin,Destination,AllTrips);
			trips.push_back(odd);
		}
		return nullptr;
				});

	std::cout<<"TotalTripCount="<<TotalTripCount<<std::endl;


}

/// <summary>
/// Create a new trip by making a random selection from all the available trips in the list. The point is to maintain the correct probability of trips based on the
/// number of journeys made from the origin destination travel to work file from the Census.
/// </summary>
void ModelTravelToWork::CreateRandomJourney() {
	//pick a random number between 0 and the total number of summed trips
	//iterate through the trips until you get to the right one
	//this could do with some optimisation
	//TODO: WRITE THIS!!!!!
	//std::uniform_real_distribution<double> uniform_x {-1.0,1.0}; //uniform distribution of doubles in [-1..1]
	//std::uniform_real_distribution<double> uniform_y {-1.0,1.0}; //uniform distribution of doubles in [-1..1]
	//std::default_random_engine e;
	//double x = uniform_x(e);

	//std::uniform_int_distribution<int> uniform_trips {0,TotalTripCount};
	std::uniform_int_distribution<int> uniform_trips {0,trips.size()};
	//std::default_random_engine e;
	int x = uniform_trips(e);

	//TODO: this is really not very efficient on several million trips
	int count=0;
	std::string Origin="";
	std::string Destination="";
	/*for (std::vector<ODData>::iterator it = trips.begin(); it!=trips.end(); ++it) {
		if ((x>=count)&&(x<=(it->Count+count))) {
			Origin = it->Origin;
			Destination = it->Destination;
			break;
		}
		else count+=it->Count+count;
	}*/
	//you should drop out of here on the break condition as there's no possible way it should ever fail (!)
	ODData odd = trips[x];
	Origin = odd.Origin;
	Destination = odd.Destination;
	//std::cout<<"Hatch: "<<Origin<<" "<<Destination<<" "<<x<<std::endl;

	//create a journey - TODO: you could hatch it from the msoa node
	//ABM::Agent* a = _agents.Hatch("worker");
	//a->Name = "worker";
	//need a to node and a from node
	std::vector<ABM::Agent*> origin_agents = _agents.With("name",Origin);
	if (origin_agents.size()==0) {
		std::cerr<<"Error: agent origin node "<<Origin<<" not found"<<std::endl;
		return;
	}
	ABM::Agent* fromNode = origin_agents.front();
	//hatch a worker from the origin node
	std::vector<ABM::Agent*> workers = fromNode->Hatch(1,"worker");
	ABM::Agent* a = workers.front();

	std::vector<ABM::Agent*> dest_agents = _agents.With("name",Destination);
	if (dest_agents.size()==0) {
		std::cerr<<"Error: agent destination node "<<Destination<<" not found"<<std::endl;
		return;
	}
	ABM::Agent* toNode = dest_agents.front();
	a->Face(*toNode);

	a->Set<ABM::Agent*>("fromNode",fromNode);
	a->Set<ABM::Agent*>("toNode",toNode);

	//glm::dvec3 pos = fromNode->GetXYZ();
	//a->SetXYZ(pos.x,pos.y,pos.z);
}

void ModelTravelToWork::Step(double Ticks) {
	std::vector<ABM::Agent*> workers = _agents.Ask("worker");
	int num = workers.size();
	for (int i=0; i<NumberOfAgents-num; ++i) {
		CreateRandomJourney();
	}

	//movement code - this operates on the existing workers i.e. not the ones just created with CreateRandoJourney above.
	//when a worker gets to his assigned msoa centroid he gets killed off and is replaced with another random one next time around
	for (std::vector<ABM::Agent*>::iterator workersIT = workers.begin(); workersIT!=workers.end(); ++workersIT)
	{
		ABM::Agent* worker = *workersIT;

		ABM::Agent* toNode = worker->Get<ABM::Agent*>("toNode");
		//TODO: save the distance on the agent properties
		double dist = worker->Distance(*toNode);
		float speed = 250.0f*(float)Ticks;
		if (dist<speed)
		{
			worker->Die();
		}
		else {
			//worker->Face(*toNode);
			worker->Forward(speed);
		}
	}

}

