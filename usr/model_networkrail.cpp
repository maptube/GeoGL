/*
 * model_networkrail.cpp
 *
 *  Created on: 6 Jun 2015
 *      Author: richard
 */

#include "model_networkrail.h"

#include <iostream>
#include <fstream>
//#include <sstream>
#include <vector>
//#include <list>
//#include <iomanip>
//#include <chrono>
#include <ctime>
#include <unordered_map>

#include "abm/Model.h"
#include "abm/Link.h"
#include "abm/Agent.h"
#include "abm/agenttime.h"

//define locations of configuration files - these are the root dirs with data in the as /YYYY/MM/DD/trackernet_YYYYMMDD_HHMMSS.csv (and similarly for countdown)
const std::string ModelNetworkRail::Filename_TrainPositions =
		"/run/media/richard/SAMSUNG2/nationalrail-cache/";

/*
 * TOD_ID codes
 * 6 Eurostar (UK) Ltd
 * 21 National Express East Anglia
 * 24 Heathrow Connect
 * 25 First Great Western
 * 26 First Capital Connect
 * 29 London Midland
 * 30 London Overground
 * 55 First Hull Trains
 * 61 National Express East Coast
 * 65 Virgin West Coast Trains
 * 74 The Chiltern Railway Co. Ltd
 * 79 c2c
 * 80 Southeastern
 * 82 Southern
 * 84 Stagecoach South Western Trains Ltd
 * 86 Heathrow Express Ltd
 */

ModelNetworkRail::ModelNetworkRail(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph)
{
	// TODO Auto-generated constructor stub

}

ModelNetworkRail::~ModelNetworkRail()
{
	// TODO Auto-generated destructor stub
}

void ModelNetworkRail::Setup()
{
	//SetDefaultShape("train","turtle");
	SetDefaultShape("train","none");
	SetDefaultSize("train",100);
	AnimationDT = AnimationDT.FromString("20131231_235000"); //so by adding 10m you get midnight on 1/1/2014
//TODO: need rail network from shapefile
}

void ModelNetworkRail::Step(double Ticks)
{
	//ticks are seconds, so *60*10 means each one is a data file
	AnimationDT.Add(600);

	//kill off all the agents
	//using while loop to kill the first agent on the list as the list gets changed every time an agent is killed, so an iterator won't work
	while (_agents.NumAgents>0) {
		_agents._Agents[0]->Die();
	}

	struct tm *tmp = localtime(&AnimationDT._DT); //still need this for the timepoint and dayofweek

	std::string DirPart = AgentTime::ToFilePath(AnimationDT);
	std::string TimeCode = AgentTime::ToStringYYYYMMDD_hhmmss(AnimationDT);

	//these two calculate the 3 minute sample point in the day and the day of week number for handling weekends and weekdays separately
	int TimePoint = (tmp->tm_hour*60+tmp->tm_min)/3; //0..6*24
	int DayOfWeek = tmp->tm_wday; //only need this when you write the data out

	std::string Filename = Filename_TrainPositions+DirPart+"/nationalrail_"+TimeCode+".csv";
	LoadTrainPositions(Filename);
	if (TimePoint==0) std::cout<<Filename<<std::endl;

	//count the new ones and write out the data
	std::vector<ABM::Agent*> trains = _agents.Ask("train");
	int Number = trains.size();
	int WithPosition=0;
	int NumberLondon=0;
	int NumberLate=0;
	std::unordered_map<int,int> TocTotalLateMins;
	std::unordered_map<int,int> TocNumberLate;
	//TODO: what about counts of selected toc_ids?
	float minlat = 51.286f, minlon=-0.51f, maxlat=51.692f, maxlon=0.34f; //London box coords (from ANTS)
	for (std::vector<ABM::Agent*>::iterator it=trains.begin(); it!=trains.end(); it++) {
		ABM::Agent* a=(*it);
		if (a->Get<bool>("positiondefined")) {
			++WithPosition;
			float Lat = a->Get<float>("lat");
			float Lon = a->Get<float>("lon");
			if ((Lat>=minlat)&&(Lat<=maxlat)&&(Lon>=minlon)&&(Lon<=maxlon)) //inside London box
				++NumberLondon;
		}
		//now calculate average late minutes per train for all tocs
		int tocid = a->Get<int>("toc_id");
		int LateMins = a->Get<int>("lateminutes");
		if (LateMins>0) {
			std::unordered_map<int,int>::iterator it = TocTotalLateMins.find(tocid);
			if (it==TocTotalLateMins.end()) {
				//need to insert new toc_id for both the late mins and count maps
				TocTotalLateMins.insert(std::pair<int,int>(tocid, LateMins));
				TocNumberLate.insert(std::pair<int,int>(tocid, 1)); //i.e. the one we added in the line above
			}
			else {
				//toc_id already exists in both maps (we assume as it's guaranteed in the first one), so accumulate counts
				TocTotalLateMins[tocid]+=LateMins;
				TocNumberLate[tocid]+=1;
			}
		}
	}


	std::ofstream out_log("/home/richard/NetworkRail.txt",std::ios::app);
	out_log<<TimeCode<<","<<TimePoint<<","<<DayOfWeek<<","<<Number<<","<<NumberLondon<<","<<WithPosition<<","<<NumberLate<<std::endl;
	out_log.close();

	//write out toc late mintues file here - in order to make sure that the columns come out in the same order, we need to hard code it
	//columns are: TimeCode, TimePoint, DayOfWeek, 6, 21, 24, 25, 26, 29, 30, 55, 61, 65, 74, 79, 80, 82, 84, 86 (all with triples of number, total mins, average)
	int tocs[] = {6, 21, 24, 25, 26, 29, 30, 55, 61, 65, 74, 79, 80, 82, 84, 86};
	std::ofstream out_toclate("/home/richard/NetworkRail_tocaveragelate.txt",std::ios::app);
	out_toclate<<TimeCode<<","<<TimePoint<<","<<DayOfWeek;
	for (int i=0; i<16; i++) {
		int Mins = TocTotalLateMins[tocs[i]];
		int Num = TocNumberLate[tocs[i]];
		float Average=0;
		if (Num>0) Average = (float)Mins/(float)Num;
		out_toclate<<","<<Mins<<","<<Num<<","<<Average;
	}
	out_toclate<<std::endl;
	out_toclate.close();

}

void ModelNetworkRail::LoadTrainPositions(const std::string& Filename)
{
	//train,servicecode,toc_id,lat,lon,runtime(mins),lateminutes,stanox,crs,stationname,toc_name
	//"521Y461931","21939001","21","51.84021","0.7041008","1","-1","50113","KEL","Kelvedon Rail Station","National Express East Anglia"

	LoadAgentsCSV(Filename,1,[this](std::vector<std::string> items) {
				ABM::Agent* a = _agents.Hatch("train");
				a->Name = items[0]; //unique name to match up to next data download
				a->Set<std::string>("tocid",items[2]);
				a->Set<std::string>("stanox",items[7]);
				a->Set<std::string>("crs",items[8]);
				if ((items[3]!="NaN")&&(items[4]!="NaN")) {
					double Lat = std::stof(items[3]);
					double Lon = std::stof(items[4]);
					//SetGeodeticPosition(a,Lat,Lon,0);
					a->Set<bool>("positiondefined",true);
					a->Set<float>("lat",(float)Lat);
					a->Set<float>("lon",(float)Lon);
				}
				else a->Set<bool>("positiondefined",false);
				a->Set<int>("toc_id",stoi(items[2]));
				a->Set<int>("runtime",std::stoi(items[5]));
				a->Set<int>("lateminutes",std::stoi(items[6]));
				return a;
			});

}

