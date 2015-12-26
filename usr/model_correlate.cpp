/*
 * model_correlate.cpp
 *
 *  Created on: 19 May 2015
 *      Author: richard
 */

#include "model_correlate.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "abm/Model.h"
#include "abm/Link.h"
#include "abm/Agent.h"
#include "abm/agenttime.h"

//define locations of configuration files - these are the root dirs with data in the as /YYYY/MM/DD/trackernet_YYYYMMDD_HHMMSS.csv (and similarly for countdown)
const std::string ModelCorrelate::Filename_BusPositions =
		//"../data/countdown/";
		"/run/media/richard/SAMSUNG2/countdown-cache/";
const std::string ModelCorrelate::Filename_TubePositions =
		//"../data/trackernet/";
		"/run/media/richard/SAMSUNG2/trackernet-cache/";

ModelCorrelate::ModelCorrelate(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph) {
	// TODO Auto-generated constructor stub

}

ModelCorrelate::~ModelCorrelate() {
	// TODO Auto-generated destructor stub
}

void ModelCorrelate::Setup() {
	//SetDefaultShape("tube","turtle");
	SetDefaultShape("tube","none");
	SetDefaultSize("tube",100);
	//SetDefaultShape("bus","turtle");
	SetDefaultShape("bus","none");
	SetDefaultSize("bus",100);
	//AnimationDT = AnimationDT.FromString("20131231_235700"); //so by adding 3m you get midnight on 1/1/2014
	AnimationDT = AnimationDT.FromString("20141231_235700");
}

////////////////////
//General replaceAll string function
//replace from string with to string
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty()) return;
	size_t start_pos=0;
	while((start_pos=str.find(from,start_pos))!=std::string::npos) {
		str.replace(start_pos,from.length(),to);
		start_pos+=to.length();
	}
}

//TODO: write a general padding function
std::string Pad4(int n) {
	std::stringstream ss;
	ss<<n;
	std::string s;
	ss>>s;
	if (n>999) return s;
	else if (n>99) return "0"+s;
	else if (n>9) return "00"+s;
	return "000"+s;
}

std::string Pad2(int n) {
	std::stringstream ss;
	ss<<n;
	std::string s;
	ss>>s;
	if (n>9) return s;
	return "0"+s;
}

std::string Pad0(int n) {
	//OK, this is a hack - no padding, just conversion to string
	std::stringstream ss;
	ss<<n;
	std::string s;
	ss>>s;
	return s;
}

/// <summary>
/// Cross correlate two time signals and return the resulting waveform
/// </summary>
std::list<float> Correlate(const int X[], const int Y[]) {
	std::list<float> Rxy;
	for (int Tor=0; Tor<480; Tor++) {
		int Sum=0;
		for (int t=0; t<480; t++) {
			//int tMTor = t-Tor; //this is a fiddle - use mod operator
			//if (tMTor<0) tMTor+=480;
			int tMTor = t+Tor; //this is a fiddle - use mod operator
			if (tMTor>=480) tMTor-=480;
			Sum += X[t]*Y[tMTor];
		}
		Rxy.push_back((float)Sum/480.0f);
	}
	return Rxy;
}

void ModelCorrelate::Step(double Ticks) {
	//ticks are seconds, so *60*3 means each one is a data file
	AnimationDT.Add(180);

	//kill off all the agents
	//using while loop to kill the first agent on the list as the list gets changed every time an agent is killed, so an iterator won't work
	while (_agents.NumAgents>0) {
		_agents._Agents[0]->Die();
	}
//TODO: you can use the agent time functions to do this now
	struct tm *tmp = localtime(&AnimationDT._DT); //still need this for the timepoint and dayofweek
//	std::stringstream ss;
//	//dir part (note dirs don't have a leading zero)
//	ss<<(tmp->tm_year+1900)<<"/"<<(tmp->tm_mon+1)<<"/"<<(tmp->tm_mday);
//	std::string DirPart;
//	ss>>DirPart;
//	//filename part (note use of same stream)
//	ss.str(""); //need to do these three to reset stream
//	ss.clear();
//	ss.seekg(0);
//	//this is a rubbish way of formatting text
//	ss<<(tmp->tm_year+1900)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_mon+1)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_mday)
//			<<"_"<<std::setfill('0')<<std::setw(2)<<(tmp->tm_hour)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_min)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_sec);
//	std::string TimeCode;
//	ss>>TimeCode;
//
	std::string DirPart = AgentTime::ToFilePath(AnimationDT);
	std::string TimeCode = AgentTime::ToStringYYYYMMDD_hhmmss(AnimationDT);

	//these two calculate the 3 minute sample point in the day and the day of week number for handling weekends and weekdays separately
	int TimePoint = (tmp->tm_hour*60+tmp->tm_min)/3; //0..479
	int DayOfWeek = tmp->tm_wday; //only need this when you write the data out

	std::string Filename = Filename_TubePositions+DirPart+"/trackernet_"+TimeCode+".csv";
	LoadTubePositions(Filename);

	Filename = Filename_BusPositions+DirPart+"/countdown_"+TimeCode+".csv";
	LoadBusPositions(Filename);


	//count the new ones and write out the data
	std::vector<ABM::Agent*> tubes = _agents.Ask("tube");
	std::vector<ABM::Agent*> buses = _agents.Ask("bus");
	TubeNumbers[TimePoint]=tubes.size();
	BusNumbers[TimePoint]=buses.size();
	//added individual tube lines
	int TempTubeNumbersLine[] = { 0,0,0,0,0,0,0,0,0,0 };
	std::string TubeLookup = "BCDHJMNPVW"; //this is the important lookup between the index number and the line code character (should be a constant?)
	for (std::vector<ABM::Agent*>::iterator tubeIT=tubes.begin(); tubeIT!=tubes.end(); ++tubeIT) {
		std::string LineCode = (*tubeIT)->Get<std::string>("LineCode");
		int Pos = TubeLookup.find_first_of(LineCode.at(0));
		++TempTubeNumbersLine[Pos];
	}
	for (int i=0; i<10; i++) {
		char LineCode = TubeLookup[i];
		TubeNumbersLine[LineCode][TimePoint]=TempTubeNumbersLine[i];
	}

	//fill in any missing data by duplicating the previous value
	if (TimePoint>0) {
		if (TubeNumbers[TimePoint]==0) TubeNumbers[TimePoint]=TubeNumbers[TimePoint-1];
		if (BusNumbers[TimePoint]==0) BusNumbers[TimePoint]=BusNumbers[TimePoint-1];
		for (int i=0; i<10; i++) {
			char LineCode = TubeLookup.at(i);
			if (TubeNumbersLine[LineCode][TimePoint]==0) TubeNumbersLine[LineCode][TimePoint]=TubeNumbersLine[LineCode][TimePoint-1];
		}
	}

	//this writes out the raw count for each 3 min period
	std::cout<<AnimationDT.ToString(AnimationDT)<<std::endl;
	std::ofstream out_lograw("/home/richard/Correlate.txt",std::ios::app);
	out_lograw<<"RawCounts,"<<AnimationDT.ToString(AnimationDT)<<","<<TimePoint<<",Tubes=,"<<tubes.size()<<",Buses=,"<<buses.size()<<std::endl;
	out_lograw.close();

	//this is the complex correlation data
	/*if (TimePoint==479) { //last timepoint before a new day starts - we can do this as we're controlling the time which is guaranteed monotonic
		std::ofstream out_log("/home/richard/Correlate.txt",std::ios::app);

		std::cout<<TimeCode<<" Tube: "<<TubeNumbers[180]<<" Bus: "<<BusNumbers[180]<<std::endl; //180 is 9am

		//correlate and find maximum correlation lag
		std::list<float> Rxy = Correlate(TubeNumbers,BusNumbers);
		float Max=0;
		int i=0, Maxi=0;
		for (std::list<float>::iterator it = Rxy.begin(); it!=Rxy.end(); it++) {
			if (*it>Max) { Max=*it; Maxi=i; }
			i++;
		}
		//now find the morning and evening peak times and write them out for a comparison
		//apply hamming window to each and repeat correlation

		//AM
		int HamAMTubeNumbers[480], HamAMBusNumbers[480];
		for (int i=0; i<480; i++) {
			float Hamming = 0.54f+0.46f*cos((i-160)*M_PI/240.0f);
			HamAMTubeNumbers[i]=TubeNumbers[i]*Hamming;
			HamAMBusNumbers[i]=BusNumbers[i]*Hamming;
		}
		std::list<float> RxyHamAM = Correlate(HamAMTubeNumbers,HamAMBusNumbers);
		float MaxHamAM=0;
		int iHamAM=0, MaxiHamAM=0;
		for (std::list<float>::iterator it = RxyHamAM.begin(); it!=RxyHamAM.end(); it++) {
			if (*it>MaxHamAM) { MaxHamAM=*it; MaxiHamAM=iHamAM; }
			iHamAM++;
		}

		//PM
		int HamPMTubeNumbers[480], HamPMBusNumbers[480];
		for (int i=0; i<480; i++) {
			float Hamming = 0.54f+0.46f*cos((i-340)*M_PI/300.0f);
			HamPMTubeNumbers[i]=TubeNumbers[i]*Hamming;
			HamPMBusNumbers[i]=BusNumbers[i]*Hamming;
		}
		std::list<float> RxyHamPM = Correlate(HamPMTubeNumbers,HamPMBusNumbers);
		float MaxHamPM=0;
		int iHamPM=0, MaxiHamPM=0;
		for (std::list<float>::iterator it = RxyHamPM.begin(); it!=RxyHamPM.end(); it++) {
			if (*it>MaxHamPM) { MaxHamPM=*it; MaxiHamPM=iHamPM; }
			iHamPM++;
		}

		//tube numbers correlation - every line against every line
		int RxyTubeTube[10][10];
		for (int i=0; i<10; i++) {
			char LineCodei = TubeLookup.at(i);
			for (int j=0; j<10; j++) {
				char LineCodej = TubeLookup.at(j);
				std::list<float> RxyTT = Correlate(TubeNumbersLine[LineCodei],TubeNumbersLine[LineCodej]);
				//find max
				float MaxRxyTT=0;
				int iRxyTT=0, MaxiRxyTT=0;
				for (std::list<float>::iterator it = RxyTT.begin(); it!=RxyTT.end(); it++) {
					if (*it>MaxRxyTT) { MaxRxyTT=*it; MaxiRxyTT=iRxyTT; }
					iRxyTT++;
				}
				RxyTubeTube[i][j]=MaxiRxyTT;
			}
		}


		//surely there is a better way of writing out the system clock?
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		std::time_t nowtt = std::chrono::system_clock::to_time_t(now);
		out_log<<"Timestamp: "<<std::ctime(&nowtt)<<std::endl;

		//write out tube numbers for the day
		out_log<<"Tube,"<<TimeCode<<","<<DayOfWeek;
		for (int i=0; i<480; i++) out_log<<","<<TubeNumbers[i];
		out_log<<std::endl;
		//write out tube numbers for each line for the day
		for (int i=0; i<10; i++) {
			char LineCode = TubeLookup.at(i);
			out_log<<"Tube"<<LineCode<<","<<TimeCode<<","<<DayOfWeek;
			for (int j=0; j<480; j++) out_log<<","<<TubeNumbersLine[LineCode][j];
			out_log<<std::endl;
		}
		//write out bus numbers for the day
		out_log<<"Bus,"<<TimeCode<<","<<DayOfWeek;
		for (int i=0; i<480; i++) out_log<<","<<BusNumbers[i];
		out_log<<std::endl;
		//write out Hamming AM tube numbers for the day
		out_log<<"TubeHamAM,"<<TimeCode<<","<<DayOfWeek;
		for (int i=0; i<480; i++) out_log<<","<<HamAMTubeNumbers[i];
		out_log<<std::endl;
		//write out Hamming AM bus numbers for the day
		out_log<<"BusHamAM,"<<TimeCode<<","<<DayOfWeek;
		for (int i=0; i<480; i++) out_log<<","<<HamAMBusNumbers[i];
		out_log<<std::endl;
		//write out Hamming PM tube numbers for the day
		out_log<<"TubeHamPM,"<<TimeCode<<","<<DayOfWeek;
		for (int i=0; i<480; i++) out_log<<","<<HamPMTubeNumbers[i];
		out_log<<std::endl;
		//write out Hamming PM bus numbers for the day
		out_log<<"BusHamPM,"<<TimeCode<<","<<DayOfWeek;
		for (int i=0; i<480; i++) out_log<<","<<HamPMBusNumbers[i];
		out_log<<std::endl;

		//write out correlation data
		out_log<<"Correlate,"<<TimeCode<<","<<DayOfWeek;
		for (std::list<float>::iterator it = Rxy.begin(); it!=Rxy.end(); it++) out_log<<","<<*it;
		out_log<<std::endl;
		out_log<<"Correlate Max,"<<TimeCode<<","<<DayOfWeek<<","<<Maxi<<","<<Max<<std::endl;
		//Hamming AM Correlation
		out_log<<"CorrelateHamAM,"<<TimeCode<<","<<DayOfWeek;
		for (std::list<float>::iterator it = RxyHamAM.begin(); it!=RxyHamAM.end(); it++) out_log<<","<<*it;
		out_log<<std::endl;
		out_log<<"CorrelateHamAM Max,"<<TimeCode<<","<<DayOfWeek<<","<<MaxiHamAM<<","<<MaxHamAM<<std::endl;
		//Hamming PM Correlation
		out_log<<"CorrelateHamPM,"<<TimeCode<<","<<DayOfWeek;
		for (std::list<float>::iterator it = RxyHamPM.begin(); it!=RxyHamPM.end(); it++) out_log<<","<<*it;
		out_log<<std::endl;
		out_log<<"CorrelateHamPM Max,"<<TimeCode<<","<<DayOfWeek<<","<<MaxiHamPM<<","<<MaxHamPM<<std::endl;

		//write out Bus/Tube ratio
		out_log<<"BusTubeRatio,"<<TimeCode<<","<<DayOfWeek;
		for (int i=0; i<480; i++) {
			float ratio=0;
			if (TubeNumbers[i]>0) ratio=BusNumbers[i]/TubeNumbers[i];
			out_log<<","<<ratio;
		}
		out_log<<std::endl;

		//finally, write out tube line to tube line correlation for every one
		//so RxyTubeTube_B is Bakerloo against all ten, then RxyTubeTube_C is central against ten etc.
		for (int i=0; i<10; i++) {
			char LineCodei = TubeLookup.at(i);
			out_log<<"RxyTubeTube_"<<LineCodei<<","<<TimeCode<<","<<DayOfWeek;
			for (int j=0; j<10; j++) {
				out_log<<","<<RxyTubeTube[i][j];
			}
			out_log<<std::endl;
		}


		//clear data for next day - not strictly necessary
		for (int i=0; i<480; i++) {
			TubeNumbers[i]=0;
			BusNumbers[i]=0;
		}

		out_log.close();
	}*/

	//this is a hack to write out the bus numbers to a simple file that I can make the year's data from:
	std::ofstream out_log("/home/richard/BusNumbers.txt",std::ios::app);
	out_log<<TimeCode<<","<<buses.size()<<std::endl;
	out_log.close();
}


//////////////////////
//General file exists function - needs to go into GeoGL.io
bool FileExists(std::string Filename) {
	std::ifstream infile(Filename);
	return infile.good();
}

void ModelCorrelate::LoadBusPositions(const std::string& Filename) {
	LoadAgentsCSV(Filename,1,[this](std::vector<std::string> items) {
			ABM::Agent* a = _agents.Hatch("bus");
			a->Name = items[3]; //unique name to match up to next data download (licence number) (or vehicle id or trip id?)
			a->Set<std::string>("route",items[0]);
			//double Lat = std::stof(items[5]);
			//double Lon = std::stof(items[6]);
			//SetGeodeticPosition(a,Lat,Lon,0);
			return a;
		});

}

void ModelCorrelate::LoadTubePositions(const std::string& Filename) {
	LoadAgentsCSV(Filename,1,[this](std::vector<std::string> items) {
		//linecode,tripnumber,setnumber,lat,lon,east,north,timetostation(secs),location,stationcode,stationname,platform,platformdirectioncode,destination,destinationcode
		//"B","0","201","51.54392","-0.2754093","519577.9","184243.3","0","At Platform","SPK","Stonebridge Park","Southbound - Platform 1","1","Check Front of Train","728"
				ABM::Agent* a = _agents.Hatch("tube");
				a->Name = items[0]+"_"+items[1]+"_"+items[2]; //unique name to match up to next data download
				a->Set<std::string>("LineCode",items[0]);
				//double Lat = std::stof(items[3]);
				//double Lon = std::stof(items[4]);
				//SetGeodeticPosition(a,Lat,Lon,0);
				return a;
			});
}

