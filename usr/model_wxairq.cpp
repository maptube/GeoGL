/*
 * model_wxairq.cpp
 *
 *  Created on: 21 Jul 2015
 *      Author: richard
 */

#include "model_wxairq.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

//#include <dirent.h>
////#include <unistd.h>
//#include <sys/stat.h>
//#include <sys/types.h>

//#include <math.h>

#include "json/json.h"
//#include "json/reader.h"

#include "ellipsoid.h"
#include "netgraphgeometry.h"

#include "abm/Link.h"
#include "abm/Agent.h"
#include "abm/agenttime.h"
#include "abm/utils.h"

#include "io/directory.h"
//#include "io/wildcardmatch.h"

//define locations of configuration files
const std::string ModelWXAirQ::Filename_WXData =
		//"../data/wxairq/metodatapoint/";
		"/run/media/richard/SAMSUNG2/metodatapoint-cache/";
const std::string ModelWXAirQ::Filename_AirQData =
		//"../data/wxairq/airquality/";
		"/run/media/richard/SAMSUNG2/airquality-cache/";
const std::string ModelWXAirQ::Filename_AirQStationList =
		"../data/wxairq/aurn-defra-2.csv";
const std::string ModelWXAirQ::LogFilename =
		"/home/richard/wxairq.txt";

ModelWXAirQ::ModelWXAirQ(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph) {
	// TODO Auto-generated constructor stub

}

ModelWXAirQ::~ModelWXAirQ() {
	// TODO Auto-generated destructor stub
}

void ModelWXAirQ::Setup() {
	//from ABM::Model
	//nodes are the route stop points
	//SetDefaultShape("node","none"); //was cube
	//SetDefaultSize("node",10.0f/*StationSize*//*0.001f*/);
	//drivers are the buses
	//SetDefaultShape("driver","turtle");
	//SetDefaultSize("driver",400.0f/*0.005f*/);
	//TODO: SetDefaultColour("driver",glm::dvec3(1.0,0,0));
	//TODO: need to be able to set the links to no mesh as well.

	//breeds are: wx and airq
	SetDefaultShape("wx","none"); //was pyramid4
	SetDefaultSize("wx",12000.0f); //400.0f is a better size
	SetDefaultShape("airq","sphere"); //was cube
	SetDefaultSize("airq",50.0f); //400.0f is a better size (was 12000)

	AnimationDT = AgentTime::FromString("20131231_230000");
	//AnimationDT = AgentTime::FromString("20141231_230000"); //for the 2015 bus strike data
	//NextTimeDT = AgentTime::FromString("20140101_000000");

	//load air q stations here
	//LoadBusStops(Filename_StopCodes); //station locations
	//LoadLinks(Filename_BusRoutesNetwork); //network from CSV lists of stops making up each route

	//load airquality sensor locations
	LoadAgentsCSV(Filename_AirQStationList,1,[&](std::vector<std::string> items) { //capture was [this]
		//latitude,longitude,site_id,uka_id,site_name,country_id,environment_id,overall_index,network_id,network_name
		ABM::Agent* a = _agents.Hatch("airq");
		float Lat = std::stof(items[0]);
		float Lon = std::stof(items[1]);
		std::string SiteID = items[2];
		a->Name = SiteID;
		//glm::vec3 P = _pEllipsoid->toVector(glm::radians(Lon),glm::radians(Lat),0);
		//a->SetXYZ(P.x,P.y,P.z);
		SetGeodeticPosition(a,Lat,Lon,0);
		a->Set<float>("lat",Lat);
		a->Set<float>("lon",Lon);
		a->Set<bool>("isdatavalid",false);
		return a;
	});

	//start the log file for WriteLogWXHRLHIL
	//StartLogWXHRLHIL();


	//now create the 3D representation from the data just loaded
	_links.Create3D(_links._pSceneRoot); //TODO: need more elegant way of calling this
}

//TODO: this needs to go in a special text processing class - HOW ABOUT A CONVERT CLASS?
bool SafeFloat(std::string Text,float& Result) {
	try {
		Result = std::stof(Text);
		return true;
	}
	catch (...) {
		//error in conversion
	}
	return false;
}
bool SafeInt(std::string Text,int& Result) {
	try {
		Result = std::stoi(Text);
		return true;
	}
	catch (...) {
		//error in conversion
	}
	return false;
}

//start the log for WriteLogWXHRLHIL
void ModelWXAirQ::StartLogWXHRLHIL() {
	std::ofstream out_log(LogFilename,std::ios::app);
	//old: out_log<<"timecode,wxname,wmo,reportutc,WxType,Visibility,AirTemp,WindSpeed,WindDir,WindGust,MSLP";
	out_log<<"timecode,wxname,wmo,reportutc,reportlocal,wxtype,visibility,temperature,dewpoint,relhumidity,wind_speed,wind_direction,wind_gust,pressure,pressure_tend";
	out_log<<",airqname,O3,NO2,SO2,PM2p5,PM10";
	out_log<<std::endl;
	out_log.close();
}

/// <summary>
/// Write out the weather data for Heathrow linked to the HRL and HIL airq sites (as two lines)
/// </summary>
void ModelWXAirQ::WriteLogWXHRLHIL(std::string TimeCode) {
	//then go through all the data and write out a file - how? interpolation? pick some close samples in London?
	//count the new ones and write out the data
	std::vector<ABM::Agent*> wx = _agents.Ask("wx");
	std::vector<ABM::Agent*> airq = _agents.Ask("airq");
	//std::cout<<"WX Agents: "<<wx.size()<<" AirQ Agents: "<<airq.size()<<std::endl;
	//TODO: what do you want in the file?
	//Heathrow,city, gatwick, wind speed, temp, pressure, all airq for london? OR INTERPOLATE?
	//NOTE: there is no NetLogo way of telling whether a property is present on an Agent - they use agent-owns [ a, b, c] to define properties a,b,c on all agents of that breed
	//TODO: need hasVariable for an agent?

	//TODO: how about writing a write CSV function that you can pass agent variables to?

	//first run of data, write out Heathrow WX with Harlington and Hillingdon air quality as they're very close
	std::vector<ABM::Agent*> a_wxHeathrow = _agents.With("name","3772");
	std::vector<ABM::Agent*> a_airqHRL = _agents.With("name","HRL");
	std::vector<ABM::Agent*> a_airqHIL = _agents.With("name","HIL");
	std::string HeathrowCSVChunk = "";
	std::string HRLCSVChunk = "";
	std::string HILCSVChunk = "";
	//make Heathrow data chunk
	if (a_wxHeathrow.size()>0) {
		//NOTE: missing agent properties are all returned as zero! Need to fix this!
		std::stringstream ss;
		ABM::Agent* a = a_wxHeathrow[0];
		std::string reportutc = a->Get<std::string>("reportutc");
		std::cout<<"reportutc="<<reportutc<<std::endl;
		int WxType = a->Get<int>("wxtype");
		int Visibility = a->Get<int>("visibility");
		float AirTemp = a->Get<float>("airtemp");
		float DewPoint = a->Get<float>("dewpoint");
		float RelHumidity = a->Get<float>("relhumidity");
		float WindSpeed = a->Get<float>("windspeed");
		float WindDir = a->Get<float>("winddir");
		float WindGust = a->Get<float>("windgust");
		float MSLP = a->Get<float>("mslp");
		std::string PressureTend = a->Get<std::string>("pressuretend");
		std::string reportlocal = a->Get<std::string>("reportlocal");
		//old:ss<<"Heathrow,03772,\""<<reportutc<<"\""<<","<<WxType<<","<<Visibility<<","<<AirTemp<<","<<WindSpeed<<","<<WindDir<<","<<WindGust<<","<<MSLP;
		//timecode,wxname,wmo,reportutc,reportlocal,wxtype,visibility,temperature,dewpoint,relhumidity,wind_speed,wind_direction,wind_gust,pressure,pressure_tend"
		ss<<"Heathrow,03772,\""<<reportutc<<"\""<<","<<"\""<<reportlocal<<"\""<<","<<WxType<<","<<Visibility<<","<<AirTemp<<","<<DewPoint<<","<<RelHumidity<<","
				<<WindSpeed<<","<<WindDir<<","<<WindGust<<","<<MSLP<<","<<PressureTend;
		HeathrowCSVChunk = ss.str();
	}
	else HeathrowCSVChunk = "Heathrow,03772,\"\",\"\",,,,,,,,,,"; //missing data file line

	//OK, now on to the HRL chunk
	if (a_airqHRL.size()>0) { //there should always be one as they don't get destroyed
		ABM::Agent* a = a_airqHRL[0];
		bool isValid = a->Get<bool>("isdatavalid");
		if (isValid) {
			std::stringstream ss;
			float O3 = a->Get<float>("O3");
			float NO2 = a->Get<float>("NO2");
			float SO2 = a->Get<float>("SO2");
			float PM2p5 = a->Get<float>("PM2p5");
			float PM10 = a->Get<float>("PM10");
			ss<<"HRL,"<<O3<<","<<NO2<<","<<SO2<<","<<PM2p5<<","<<PM10;
			HRLCSVChunk=ss.str();
		}
		else HRLCSVChunk = "HRL,,,,,"; //missing data file line
	}
	//and the HIL chunk
	if (a_airqHIL.size()>0) { //there should always be one as they don't get destroyed
		ABM::Agent* a = a_airqHIL[0];
		bool isValid = a->Get<bool>("isdatavalid");
		if (isValid) {
			std::stringstream ss;
			float O3 = a->Get<float>("O3");
			float NO2 = a->Get<float>("NO2");
			float SO2 = a->Get<float>("SO2");
			float PM2p5 = a->Get<float>("PM2p5");
			float PM10 = a->Get<float>("PM10");
			ss<<"HIL,"<<O3<<","<<NO2<<","<<SO2<<","<<PM2p5<<","<<PM10;
			HILCSVChunk=ss.str();
		}
		else HILCSVChunk = "HIL,,,,,";
	}

	//finally, write out the two data lines
	std::ofstream out_log(ModelWXAirQ::LogFilename,std::ios::app);
	out_log<<TimeCode<<"0000,"; //it had the mmss stripped earlier - also, should you do anything about the actual times of each data source?
	out_log<<HeathrowCSVChunk<<","<<HRLCSVChunk<<std::endl;
	out_log<<TimeCode<<"0000,";
	out_log<<HeathrowCSVChunk<<","<<HILCSVChunk<<std::endl;
	out_log.close();
}

/// <summary>
/// This writes out averages for each stations within the L
/// </summary>
void ModelWXAirQ::WriteLogLondonBoxNO2Hourly(std::string TimeCode) {
	//NO2 averages for sites and hour of day
	//this is aggregate data that gets written after all the data has been processed
	const float MinLat=51.286, MinLon=-0.51, MaxLat=51.692, MaxLon=0.34; //London box

	std::ofstream out_log(ModelWXAirQ::LogFilename,std::ios::app);

	std::vector<ABM::Agent*> airq = _agents.Ask("airq");
	//first step, write out the header line containing the names of the sites in London
	out_log<<"HH";
	for (std::vector<ABM::Agent*>::iterator it = airq.begin(); it!=airq.end(); it++) {
		ABM::Agent* a = (*it);
		float Lat = a->Get<float>("lat");
		float Lon = a->Get<float>("lon");
		if ((Lat>=MinLat)&&(Lat<=MaxLat)&&(Lon>=MinLon)&&(Lon<=MaxLon)) {
			out_log<<","<<a->Name<<"_TotalNO2,"<<a->Name<<"_Count,"<<a->Name<<"_Average";
		}
	}
	out_log<<std::endl;

	//second step, write out the actual data
	for (int Hour=0; Hour<24; ++Hour) {
		std::string strHH = std::to_string(Hour);
		if (Hour<10) strHH="0"+strHH;
		out_log<<strHH;
		for (std::vector<ABM::Agent*>::iterator it = airq.begin(); it!=airq.end(); it++) {
			ABM::Agent* a = (*it);
			float Lat = a->Get<float>("lat");
			float Lon = a->Get<float>("lon");
			if ((Lat>=MinLat)&&(Lat<=MaxLat)&&(Lon>=MinLon)&&(Lon<=MaxLon)) {
				float total = a->Get<float>("NO2Total_"+strHH);
				float count = a->Get<float>("NO2Count_"+strHH); //yes, really, count is a float
				float mean = 0;
				if (count>0) mean=total/count;
				out_log<<","<<total<<","<<count<<","<<mean;
			}
		}
		out_log<<std::endl;
	}
	out_log.close();
}

void ModelWXAirQ::WriteLogLondonBoxNO2(std::string TimeCode) {
	//this one just writes out the current data hour by hour for all sites within the London box. Used to generate the
	//mean values for the bus strike data in Jan/Feb 2015
	//NOTE: ONLY BANK HOLIDAYS FOR 2014 are handled correctly.
	const float MinLat=51.286, MinLon=-0.51, MaxLat=51.692, MaxLon=0.34; //London box
	//const AgentTime Holidays[] = { AgentTime::FromString("2014") };

	int DayOfWeek = this->AnimationDT.GetDayOfWeek();
	int WeekDay=1; //1=normal working day, 0=sat, sun or bank holiday
	if ((DayOfWeek==0)||(DayOfWeek==6)) WeekDay=0; //Sun or Sat
	//Bank Holidays in 2014: 1 Jan, 18,21 April, 5,26 May, 25 Aug, 25, 26 Dec
	//TODO: put in Bank Holidays here...
	int Year,Month,Day;
	this->AnimationDT.GetYearMonthDay(Year,Month,Day);
	if (Year==2014) {
		if ((Month==1)&&(Day==1)) WeekDay=0; //Jan 1st
		if ((Month==4)&&((Day==18)||(Day==21))) WeekDay=0; //April 18 and 21, Good Friday, Easter Monday
		if ((Month==5)&&((Day==5)||(Day==26))) WeekDay=0; //May 5 and 26 Bank Holidays
		if ((Month==8)&&(Day==25)) WeekDay=0; //Aug 25 Bank Holiday
		if ((Month==12)&&((Day==25)||(Day==26))) WeekDay=0; //Christmas
	}

	std::ofstream out_log(ModelWXAirQ::LogFilename,std::ios::app);

	out_log<<TimeCode<<"0000"; //it had mmss stripped off earlier
	out_log<<","<<DayOfWeek<<","<<WeekDay; //write day of week (0..6) followed by whether it's a normal weekday (0|1)
	std::vector<ABM::Agent*> airq = _agents.Ask("airq");
	for (std::vector<ABM::Agent*>::iterator it = airq.begin(); it!=airq.end(); it++) {
		ABM::Agent* a = (*it);
		float Lat = a->Get<float>("lat");
		float Lon = a->Get<float>("lon");
		if ((Lat>=MinLat)&&(Lat<=MaxLat)&&(Lon>=MinLon)&&(Lon<=MaxLon)) {
			std::string Name = a->Name;
			float NO2 = a->Get<float>("NO2");
			out_log<<","<<Name<<","<<NO2;
		}
	}
	out_log<<std::endl;
}


void ModelWXAirQ::Step(double Ticks) {
	AnimationDT.Add(3600); //it's hourly data
	std::cout<<AgentTime::ToString(AnimationDT)<<std::endl;
	std::string DirPart = AgentTime::ToFilePath(AnimationDT);
	std::string TimeCode = AgentTime::ToStringYYYYMMDD_hhmmss(AnimationDT);
	TimeCode = TimeCode.substr(0,11); //strip the mmss
	//patterns for matching relevant files for each data source
	std::string PatternWX = "r_metodatapoint_" + TimeCode + "????.csv"; //NOTE r_metodatapoint for reprocessed files on zero min boundaries and filenames correct local times
	std::string PatternAirQ = "airquality_" + TimeCode + "????.csv";
	//scan the directory for a filename matching the regex

	//old
	//std::string MatchedFilenameWX = MatchFilePattern(Filename_WXData+DirPart,PatternWX);
	//std::string MatchedFilenameAirQ = MatchFilePattern(Filename_AirQData+DirPart,PatternAirQ);
	//new
	std::string MatchedFilenameWX = geogl::io::Directory::MatchFilePattern(Filename_WXData+DirPart,PatternWX);
	std::string MatchedFilenameAirQ = geogl::io::Directory::MatchFilePattern(Filename_AirQData+DirPart,PatternAirQ);

	//make up two fully qualified paths from the matched filename patterns and directory tree
	std::string FilenameWX = Filename_WXData+DirPart+"/"+MatchedFilenameWX;
	std::string FilenameAirQ = Filename_AirQData+DirPart+"/"+MatchedFilenameAirQ;
	std::cout<<"ModelWXAirQ::Step : "<<FilenameWX<<" "<<FilenameAirQ<<std::endl;

	//kill off all the agents
	//using while loop to kill the first agent on the list as the list gets changed every time an agent is killed, so an iterator won't work
	//NOTE: clear-turtles kills all the turtles in NetLogo
	//while (_agents.NumAgents>0) {
	//	_agents._Agents[0]->Die();
	//}
	ClearTurtles("wx"); //delete all the weather turtles
	//and set isdatavalid=false on all airq turtles
	std::vector<ABM::Agent*> turtles = _agents.Ask("airq");
	for (std::vector<ABM::Agent*>::iterator it = turtles.begin(); it!=turtles.end(); it++)
		(*it)->Set<bool>("isdatavalid",false);

	//load weather data
	if (MatchedFilenameWX.length()>0)
	{
		LoadAgentsCSV(FilenameWX,1,[&](std::vector<std::string> items) { //capture was [this]
			//OLD:"datetime_utc","wxtype","visibility","temperature","wind_speed","wind_direction","wind_gust","pressure","station_name","station_number","lat","lon","report_datetime_utc","datetime_local"
			//NEW:datetime_utc,wxtype,visibility,temperature,dewpoint,relhumidity,wind_speed,wind_direction,wind_gust,pressure,pressure_tend,station_name,station_number,lat,lon,report_datetime_utc,datetime_local
			int WxType, Visibility, WMONumber;
			float AirTemp, DewPoint, RelHumidity, WindSpeed, WindDir, WindGust, MSLP, Lat, Lon;
			ABM::Agent* a = _agents.Hatch("wx");
			a->Name=items[12]; //string copy of WMO number
			if (SafeInt(items[1],WxType)) a->Set<int>("wxtype",WxType);
			if (SafeInt(items[2],Visibility)) a->Set<int>("visibility",Visibility);
			if (SafeFloat(items[3],AirTemp)) a->Set<float>("airtemp",AirTemp);
			if (SafeFloat(items[4],DewPoint)) a->Set<float>("dewpoint",DewPoint);
			if (SafeFloat(items[5],RelHumidity)) a->Set<float>("relhumidity",RelHumidity);
			if (SafeFloat(items[6],WindSpeed)) a->Set<float>("windspeed",WindSpeed);
			if (SafeFloat(items[7],WindDir)) a->Set<float>("winddir",WindDir);
			if (SafeFloat(items[8],WindGust)) a->Set<float>("windgust",WindGust);
			if (SafeFloat(items[9],MSLP)) a->Set<float>("mslp",MSLP);
			a->Set<std::string>("pressuretend",items[10]); //it's a string i.e. F=falling
			//station name
			if (SafeInt(items[12],WMONumber)) a->Set<int>("wmonumber",WMONumber);
			if (SafeFloat(items[13],Lat)) a->Set<float>("lat",Lat);
			if (SafeFloat(items[14],Lon)) a->Set<float>("lon",Lon);
			a->Set<std::string>("reportutc",items[15]);
			a->Set<std::string>("reportlocal",items[16]);
			//datetime local - NOTE: there was an error in this in the original code
			//YOU COULD go through the existing UK stations and just bind data without destroying agents - need missing data value though
			//it's a join on the WMO number, or you could track LIVE agents
			SetGeodeticPosition(a,Lat,Lon,0);

			return a;
		});
	}

	//load air quality data
	if (MatchedFilenameAirQ.length()>0)
	{
		LoadAgentsCSV(FilenameAirQ,1,[&](std::vector<std::string> items) { //capture was [this]
			//"datetime_utc","site_id","O3","O3_index","NO2","NO2_index","SO2","SO2_index","PM2p5","PM2p5_index","PM10","PM10_index","lastupdate","O3_text","NO2_text","SO2_text","PM2p5_text","PM10_text","datetime_local"
			ABM::Agent* a = nullptr;

			std::string SiteID = items[1];
			std::vector<ABM::Agent*> agents = _agents.With("name",SiteID); //assuming wx and airq site id names are unique
			if (agents.size()==0) {
				std::cerr<<"Error: Air Quality agent "<<SiteID<<" not found"<<std::endl;
			}
			else { //I'm assuming you can't get more than one!
				float O3, NO2, SO2, PM2p5, PM10;
				//ABM::Agent* a = _agents.Hatch("airq");
				a=agents[0];
				a->Name=items[1]; //it must already have been set to this anyway
				a->Set<std::string>("datetimeutc",items[0]);
				//need to blank any existing data
				a->Set<float>("O3",-1); a->Set<float>("NO2",-1); a->Set<float>("PM2p5",-1); a->Set<float>("PM10",-1);
				//now set the real data
				if (SafeFloat(items[2],O3)) a->Set<float>("O3",O3);
				//O3 index
				if (SafeFloat(items[4],NO2)) {
					a->Set<float>("NO2",NO2);
					//update NO2 hourly based on the type of day i.e. weekday or Sunday
					int DayOfWeek = AnimationDT.GetDayOfWeek();
					if (DayOfWeek==0) {
						//and update NO2 hourly total and counters, used to compute hourly average
						std::string HH = TimeCode.substr(9,2); //OK, it's a bit of a hack, but it gives you a 2 digit hour string
						float total = 0, count = 0;
						if (a->OwnsVariable("NO2Total_"+HH)) total = a->Get<float>("NO2Total_"+HH);
						if (a->OwnsVariable("NO2Count_"+HH)) count = a->Get<float>("NO2Count_"+HH);
						total+=NO2;
						count+=1;
						a->Set<float>("NO2Total_"+HH,total);
						a->Set<float>("NO2Count_"+HH,count);
					}
				}
				//NO2 index
				if (SafeFloat(items[6],SO2)) a->Set<float>("SO2",SO2);
				//SO2 index
				if (SafeFloat(items[8],PM2p5)) a->Set<float>("PM2p5",PM2p5);
				//PM2.5 index
				if (SafeFloat(items[10],PM10)) a->Set<float>("PM10",PM10);
				//PM10 index
				//last update
				//O3 text
				//NO2 text
				//SO2 text
				//PM2.5 text
				//PM10 text
				//datetime local
				a->Set<bool>("isdatavalid",true);
			}

			return a;
		});
	}

	//then go through all the data and write out a file - how? interpolation? pick some close samples in London?
	//count the new ones and write out the data
	std::vector<ABM::Agent*> wx = _agents.Ask("wx");
	std::vector<ABM::Agent*> airq = _agents.Ask("airq");
	std::cout<<"WX Agents: "<<wx.size()<<" AirQ Agents: "<<airq.size()<<std::endl;
	//TODO: what do you want in the file?
	//Heathrow,city, gatwick, wind speed, temp, pressure, all airq for london? OR INTERPOLATE?
	//NOTE: there is no NetLogo way of telling whether a property is present on an Agent - they use agent-owns [ a, b, c] to define properties a,b,c on all agents of that breed
	//TODO: need hasVariable for an agent?

	//TODO: how about writing a write CSV function that you can pass agent variables to?

	//This writes out the Heathrow wx data linked to HRL and HIL on separate lines
	//WriteLogWXHRLHIL(std::string TimeCode);

	//Second data logging - this writes Hour, sideid1, siteid2...siteidn for
	//NOTE: we only do this once after all the data has been processed
	if (TimeCode=="20141231_23") WriteLogLondonBoxNO2Hourly(TimeCode);

	//Third data logging - this write hourly data for all London sites to allow a running mean to be calculated for the London box data
	//WriteLogLondonBoxNO2(TimeCode);
}

//todo: this needs to be a library function
/*std::string ModelWXAirQ::MatchFilePattern(std::string BaseDir, std::string pattern) {
	//todo: could do with a C# style Directory class
	std::string filepath, Result;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;
	geogl::io::WildcardMatch* match = new geogl::io::WildcardMatch();

	dp = opendir(BaseDir.c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening " << BaseDir << std::endl;
		return ""; // errno;
	}

	Result = "";
	while ((dirp = readdir(dp)))
	{
		std::string name = dirp->d_name;
		filepath = BaseDir + "/" + name;

		// If the file is a directory (or is in some way invalid) we'll skip it
		if (stat( filepath.c_str(), &filestat )) continue;
		if (S_ISDIR( filestat.st_mode ))         continue;

		//check match on dirp->d_name and pattern
		//std::tr1::smatch matches;
		//bool found = regex_match(name,matches,pattern);
		bool found = match->match(pattern,name);
		if (found) {
			Result=name;
			std::cout<<"Match Found: "<<name<<std::endl;
			break;
		}
		else {
			std::cout<<"Match not found: "<<name<<std::endl;
		}
	}
	closedir(dp);
	delete match;

	return Result;

}
*/
