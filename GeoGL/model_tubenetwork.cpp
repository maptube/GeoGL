//Tube Network functions
//Specialisation of functions required to load and build a London Underground network
#include "model_tubenetwork.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iterator>
//#include <Windows.h> //why on earth is this here?
#include <math.h>
#include <ctime>

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

#include <dirent.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std;

#include "json/json.h"
#include "json/reader.h"

#include "ellipsoid.h"
#include "netgraphgeometry.h"

#include "Link.h"
#include "Agent.h"
//#include "Model.h"

//define locations of configuration files
const std::string ModelTubeNetwork::Filename_StationCodes = /*"data/station-codes.csv";*/ "..\\data\\station-codes.csv"; //station locations
const std::string ModelTubeNetwork::Filename_TubeODNetwork = /*"data/tube-network.json";*/ "..\\data\\tube-network.json"; //network from JSON origin destination file
const std::string ModelTubeNetwork::Filename_TrackernetPositions = /*"data/trackernet_20140127_154200.csv";*/ "..\\data\\trackernet_20140127_154200.csv"; //train positions
const float ModelTubeNetwork::LineSize = 50; //size of track
const int ModelTubeNetwork::LineTubeSegments = 10; //number of segments making up the tube geometry
const float ModelTubeNetwork::StationSize = 100.0f; //size of station geometry object
const float ModelTubeNetwork::TrainSize = 300.0f; //size of train geometry object

/// <summary>
/// Create a Tube Network containing a model of the tube and train agents which run around the track. You have to call load() with the relevant file names to load the
/// network graph and stations location table.
/// </summary>
/// <param name="SceneGraph">
/// Reference to the scene graph that the Tube Network will be added to and which the ABM code will control.
/// </param>
ModelTubeNetwork::ModelTubeNetwork(SceneGraphType* SceneGraph) : ABM::Model(SceneGraph) {
	//tube_graph = new Graph(true);
	//tube_stations = new map<string,struct TubeStationWGS84>();
	tube_stations = new unordered_map<string,struct GraphNameXYZ>();
}

/// <summary>Destructor</summary>
ModelTubeNetwork::~ModelTubeNetwork() {
	//delete tube_graph;
	for (std::unordered_map<char,Graph*>::iterator it=tube_graphs.begin(); it!=tube_graphs.end(); ++it) {
		delete (it->second);
	}
	delete tube_stations;
}


/// <summary>
/// Get colour for a tube line from the line code character
/// </summary>
/// <param name="Code">Line Code</param>
/// <returns>A hexadecimal colour integer for the line</returns>
unsigned int ModelTubeNetwork::LineCodeToColour(char Code) {
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
glm::vec3 ModelTubeNetwork::LineCodeToVectorColour(char Code) {
	switch (Code) {
	case 'B': return glm::vec3(0.69f,0.38f,0.06f); //Bakerloo
	case 'C': return glm::vec3(0.94f,0.18f,0.14f); //Central
	case 'D': return glm::vec3(0.0f,0.53f,0.25f); //District
	case 'H': return glm::vec3(1.0f,0.82f,0.01f); //Hammersmith and City and Circle (this is yellow!)
	case 'J': return glm::vec3(0.58f,0.61f,0.64f); //Jubilee
	case 'M': return glm::vec3(0.6f,0.0f,0.36f); //Metropolitan
	case 'N': return glm::vec3(0.14f,0.12f,0.13f); //Northern
	case 'P': return glm::vec3(0.11f,0.24f,0.58f); //Piccadilly
	case 'V': return glm::vec3(0.0f,0.62f,0.86f); //Victoria
	case 'W': return glm::vec3(0.53f,0.81f,0.74f); //Waterloo and City
	default: return glm::vec3(1.0f,1.0f,1.0f); //white
	}
}


/// <summary>
/// Load the station names and positions from the CSV file. Create an agent of Breed "node" and name=stationcode at each station.
/// This allows us to look up stations when creating the network graph.
/// Call this first, before loadLinks.
/// </summary>
/// <param name="Filename">The CSV file containing the stations data</param>
void ModelTubeNetwork::loadStations(std::string Filename) {
	//OK, now on to loading part two - load the stations file containing the lat/lon of all the tube stations which form vertices in the graph
	//ifstream in_csv("C:\\richard\\projects\\VC++\\GeoGL\\GeoGL\\data\\station-codes.csv");
	ifstream in_csv(Filename.c_str());
	//#code,NPTGCode,lines,lon,lat,name
	//ACT,9400ZZLUACT1,DP,-0.28025120353611,51.50274977300050,Acton Town
	if (in_csv.is_open()) {
		string line;
		while (!in_csv.eof()) {
			line.clear();
			std::getline(in_csv,line);
			if ((line.length()==0)||(line[0]=='#')) continue; //blank line or comment line
			//parse the line as a string stream separated by ,
			stringstream ss(line);
			std::vector<string> items;
			while (ss.good()) {
				string elem;
				std::getline(ss,elem,',');
				items.push_back(elem);
			}
			if (items.size()>=6) {
				string code1;
				float lon,lat;
				code1 = items[0];
				lon = std::stof(items[3]);
				lat = std::stof(items[4]);
				//TubeStationWGS84 pos = {lon,lat};
				GraphNameXYZ pos;
				//pos.Name=code1; pos.P.x=lon; pos.P.y=lat; pos.P.z=0; 
				pos.Name=code1; pos.P=_pEllipsoid->toVector(glm::radians(lon),glm::radians(lat));
				tube_stations->insert(make_pair/*<string,struct GraphNameXYZ>*/(code1,pos));
				
				//and create the node agent
				vector<ABM::Agent*> AList = PatchesPatchXYSprout((int)pos.P.x,(int)pos.P.y,1,"node");
				ABM::Agent *a = AList.front();
				
				//ABM::Agent* a = _agents.Hatch("node"); //alternative method to sprout
				
				a->SetXYZ(pos.P.x,pos.P.y,pos.P.z);
				a->Name=code1;
				//std::cout<<"agent "<<code1<<" "<<pos.P.x<<","<<pos.P.y<<","<<pos.P.z<<std::endl;
				//and set him to a sphere?
			}

			//std::istringstream(line)>>code1>>code2>>tubelines>>lon>>lat;
			//sscanf(line.c_str(),"%s,%s,%s,%f,%f",code1,code2,tubelines,lon,lat);
		}
		in_csv.close();
	}
}

/// <summary>
/// Load the links between stations into the graph/links structure.
/// MUST have called loadStations first.
/// </summary>
/// <param name="Filename">Name of the graph file</param>
void ModelTubeNetwork::loadLinks(std::string NetworkJSONFilename) {
	stringstream ss;
	ifstream in_json(NetworkJSONFilename.c_str());

	Json::Value root;   // will contain the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(in_json, root );
	in_json.close();
	if ( !parsingSuccessful )
	{
		// report to the user the failure and their locations in the document.
		std::cout  << "Failed to parse configuration\n" << reader.getFormatedErrorMessages();
		return;
	}

	//why on earth did I do it like this?
	const string linecodes[] = { string("B"), string("C"), string("D"), string("H"), string("J"), string("M"), string("N"), string("P"), string("V"), string("W") };
	const string dirs[] = { string("0"), string("1") }; //bi-directional
	//const string dirs[] = { string("0") }; //uni-directional

	for (int i=0; i<10; i++) {
		Graph *G = new Graph(true);
		std::map<std::string,int> VertexNames; //lookup between vertex names and ids in the network graph (need a new lookup each time a new graph is started)
		const Json::Value& jsLine = root[linecodes[i]];
		for (int dir=0; dir<=1; dir++) { //HACK for unidirectional
			const Json::Value& jsLineDir = jsLine[dirs[dir]];
			std::string Label = linecodes[i]+"_"+dirs[dir]; //e.g. V_0 for Victoria Northbound or V_1 for Southbound
			for (unsigned int v = 0; v < jsLineDir.size(); ++v ) {
				//cout<<"data:"<<jsLineDir[v]<<endl;
				//OutputDebugStringW(L"data:");
				//OutputDebugStringW(jsLineDir[v].asString().c_str()); //how? _T _W
				//OutputDebugStringA(jsLineDir[v].asString().c_str());
				// "o", "d", "r"
				//velocity = dist/runlink
				const Json::Value& node = jsLineDir[v];
				string o = node["o"].asString();
				string d = node["d"].asString();
				int r = node["r"].asInt();
				//OutputDebugStringA("Node: ");
				//OutputDebugStringA(o.c_str());
				//OutputDebugStringA("\n");
				//add the vertices to the graph and connect them (in a directional sense)
				//need to lookup the vertex id from the 3 letter station code string which we keep in VertexNames map as the graph is built
				//TODO: need to handle lines and directions properly (label edges?)
				Vertex *VOrigin, *VDestination;
				std::map<std::string,int>::const_iterator it;
				it = VertexNames.find(o);
				if (it==VertexNames.end()) {
					//not found, so create a new vertex
					VOrigin = G->AddVertex();
					VOrigin->_Name=o; //station 3 letter code
					//VertexNames.insert(pair<string,int>(o,VOrigin->_VertexId)); //push name and vertex id onto hash
					VertexNames[o]=VOrigin->_VertexId; //push name and vertex id onto hash
				}
				else VOrigin = G->_Vertices[it->second];
				//destination
				it = VertexNames.find(d);
				if (it==VertexNames.end()) {
					//not found, so create a new vertex
					VDestination = G->AddVertex();
					VDestination->_Name=d; //station 3 letter code
					//VertexNames.insert(pair<string,int>(d,VDestination->_VertexId)); //push name and vertex id onto hash
					VertexNames[d]=VDestination->_VertexId; //push name and vertex id onto hash
				}
				else VDestination = G->_Vertices[it->second];
				//now connect the pair - THIS RETURNS THE EDGE IF YOU NEED IT
				G->ConnectVertices(VOrigin,VDestination,Label,(float)r); //node weight is the runlink in seconds
				
				//Everything up to this point is direct manipulation of an internal graph structure. The following is another method
				//using the ABM structure i.e. this creates a second, separate graph - user should only use ABM interface.
				ABM::Agent* agent_o = _agents.With("name",o).front();
				ABM::Agent* agent_d = _agents.With("name",d).front();
				ABM::Link* L = _links.Create(agent_o,agent_d);
				//this is where you set the velocity, linecode, direction
				L->Set<std::string>("lineCode",string(linecodes[i]));
				L->Set<int>("direction",dir);
				L->Set<float>("runlink",(float)r);
				L->colour=LineCodeToVectorColour(linecodes[i][0]);
				//now add a pre-created velocity for this link based on distance and runlink in seconds
				double dx = L->end2->xcor()-L->end1->xcor();
				double dy = L->end2->ycor()-L->end1->ycor();
				double dz = L->end2->zcor()-L->end1->zcor();
				double dist = sqrt(dx*dx+dy*dy+dz*dz);
				L->Set<float>("velocity",(float)(dist/(float)r));
			}
		}
		tube_graphs.insert(pair<char,Graph*>(linecodes[i].c_str()[0],G));
	}
}

/// <summary>
/// Load train positions from CSV file (or web service?) and create agents for them
/// </summary>
void ModelTubeNetwork::loadPositions(std::string Filename) {
	ifstream in_csv(Filename.c_str());
	//linecode,tripnumber,setnumber,lat,lon,east,north,timetostation(secs),location,stationcode,stationname,platform,platformdirectioncode,destination,destinationcode
	//"B","0","201","51.54392","-0.2754093","519577.9","184243.3","0","At Platform","SPK","Stonebridge Park","Southbound - Platform 1","1","Check Front of Train","728"
	if (in_csv.is_open()) {
		string line;
		while (!in_csv.eof()) {
			line.clear();
			std::getline(in_csv,line);
			if ((line.length()==0)||(line[0]=='#')) continue; //blank line or comment line
			//parse the line as a string stream separated by ,
			stringstream ss(line);
			std::vector<string> items;
			while (ss.good()) {
				string elem;
				std::getline(ss,elem,',');
				items.push_back(elem);
			}
			if (items.size()==15) {
				//do stuff here
				//line,trip,set,lat,lon,east,north,timetostation,location,stationcode,stationname,platform,platformdirectioncode,destination,destinationcode
				for (std::vector<std::string>::iterator it=items.begin(); it!=items.end(); ++it) {
					//data[j]=data[j].replace(/\"/g,'') #remove quotes from all columns
					//you need std::tr1::regex
					//std::regex r("/\"/g");
					//*it=r.
					//std::string str = *it;
					//std::string s("test");
					//std::regex re("");
					//cout<<regex_replace(s,re,"",regex_constants::match_default);
					//cout<<std::regex_replace(s,"",re);

					//ALL quotes
					std::string str=*it;
					str.erase(remove( str.begin(), str.end(), '\"' ),str.end());
					*it=str;
				}
				std::string lineCode = items[0];
				std::string tripcode = items[1];
				std::string setcode = items[2];
				std::string stationcode = items[9];
				int dir = std::atoi(items[12].c_str());
				//ABM::Agent* agent_d = @nodes.with("o.name=='"+stationcode+"'") //destination node station
				std::vector<ABM::Agent*> agent_d = _agents.With("name",stationcode); //destination node station - do you need the breed name as well?
				//find a link with the correct linecode that connects o to d
				//console.log(agent_d)
				if (agent_d.size()>0) {
					//console.log("found: ",agent_d[0])
					std::vector<ABM::Link*> links = agent_d.front()->InLinks();
					for (std::vector<ABM::Link*>::iterator itLinks = links.begin(); itLinks!=links.end(); ++itLinks)
					{
						//console.log("l: ",l)
						ABM::Link* l = *itLinks;

						if ((l->Get<std::string>("lineCode")==lineCode) && (l->Get<int>("direction")==dir))
						{
							//OK, so l is the link that this tube is on and we just have to position between end1 and end2
							//console.log("found: ",stationcode,lineCode,dir)
							//console.log("found2: ",l)
							//now hatch a new agent driver from this node and place in correct location
							//nominally, the link direction is end1 to end2
							std::vector<ABM::Agent*> a_list = l->end1->Hatch(1,"driver");
							ABM::Agent* a = a_list[0];
							a->Name = lineCode+"_"+tripcode+"_"+setcode; //unique name to match up to next data download
							a->Set<ABM::Agent*>("fromNode", l->end1);
							a->Set<ABM::Agent*>("toNode", l->end2);
							a->Face(*a->Get<ABM::Agent*>("toNode"));
//TODO: the velocity isn't right due to the frame rate (also, it's speed anyway)
							a->Set<float>("v", l->Get<float>("velocity")/*/10.0f*/); //use pre-created velocity for this link
							//dx=l.end2.x-l.end1.x
							//dy=l.end2.y-l.end1.y
							//dist = Math.sqrt(dx*dx+dy*dy)
							//a.v = dist/l.runlink #should really make sure runlink>0, but it's my data originally
							//a.v = 0.05
							a->Set<int>("direction", l->Get<int>("direction"));
							a->Set<std::string>("lineCode", l->Get<std::string>("lineCode"));
							//TODO: a->color=@lineColours[l.lineCode];
							a->SetColour(LineCodeToVectorColour(lineCode[0]));
							//cout<<"created agent "<<a->Name<<endl;
							
						}

					}

				}

			}

			//std::istringstream(line)>>code1>>code2>>tubelines>>lon>>lat;
			//sscanf(line.c_str(),"%s,%s,%s,%f,%f",code1,code2,tubelines,lon,lat);
		}
		in_csv.close();
	}
}

/// <summary>
/// assuming the animation records have been loaded, create tubes and initialise the animation from the first animation frame
/// Must call LoadAnimation first
/// </summary>
void ModelTubeNetwork::LoadAnimatePositions() {
	//time_t Frame0DT = (*tube_anim_frames.begin()).first;
	time_t Frame0DT = GetFirstAnimationTime();
	map<string,struct tube_anim_record> Frame0 = tube_anim_frames[Frame0DT];
	cout<<"initialising animation with first frame time of "<<AgentTime::ToString(Frame0DT)<<" and "<<Frame0.size()<<" agents"<<endl;
	
	//now go through all the vehicles in frame zero and create them in the correct positions and orientations
//	for (map<string,struct tube_anim_record>::iterator it = Frame0.begin(); it!=Frame0.end(); ++it)
//	{
//		string UniqueName = it->first;
//		tube_anim_record rec = it->second;
//		string strLineCode = "X";
//		strLineCode[0]=rec.LineCode; //how else are you supposed to create a string from a char?
//		std::vector<ABM::Agent*> agent_d = _agents.With("name",rec.StationCode); //destination node station - do you need the breed name as well?
//		//find a link with the correct linecode that connects o to d
//		if (agent_d.size()>0) {
//			std::vector<ABM::Link*> links = agent_d.front()->InLinks();
//			for (std::vector<ABM::Link*>::iterator itLinks = links.begin(); itLinks!=links.end(); ++itLinks)
//			{
//				ABM::Link* l = *itLinks;
//				
//				if ((l->Get<std::string>("lineCode")==strLineCode) && (l->Get<int>("direction")==rec.Direction))
//				{
//					//OK, so l is the link that this tube is on and we just have to position between end1 and end2
//					//now hatch a new agent driver from this node and place in correct location
//					//nominally, the link direction is end1 to end2
//					std::vector<ABM::Agent*> a_list = l->end1->Hatch(1,"driver");
//					ABM::Agent* a = a_list[0];
//					a->Name = UniqueName; //unique name to match up to next data download
//					a->Set<ABM::Agent*>("fromNode", l->end1);
//					a->Set<ABM::Agent*>("toNode", l->end2);
//					a->Face(*a->Get<ABM::Agent*>("toNode"));
////what am I doing with the velocity?
//					a->Set<float>("v", l->Get<float>("velocity")); //use pre-created velocity for this link
//					a->Set<int>("direction", l->Get<int>("direction"));
//					a->Set<std::string>("lineCode", l->Get<std::string>("lineCode"));
//					a->SetColour(LineCodeToVectorColour(rec.LineCode));
//					//cout<<"created agent "<<a->Name<<endl;
//			//TODO: this hasn't actually interpolated the positions!
//				}	
//			}
//		}
//	}

	for (map<string,struct tube_anim_record>::iterator it = Frame0.begin(); it!=Frame0.end(); ++it)
	{
		string UniqueName = it->first;
		tube_anim_record rec = it->second;
		//string strLineCode = "X";
		//strLineCode[0]=rec.LineCode; //how else are you supposed to create a string from a char?
		ABM::Agent* a = _agents.Hatch("driver");
		a->Name = UniqueName; //unique name to match up to next data download
		a->SetColour(LineCodeToVectorColour(rec.LineCode));
		if (!PositionAgent(a,rec.LineCode,rec.TimeToStation,rec.StationCode,(int)rec.Direction))
			if (!PositionAgent(a,rec.LineCode,rec.TimeToStation,rec.StationCode,1-(int)rec.Direction)) //try other direction - shouldn't happen, but it does
				cerr<<"Error hatching agent "<<UniqueName<<endl;
	}

}

/// <summary>
/// Load a directory of csv files containing tube positions for the animation.
/// </summary>
/// <param name="DirectoryName">Root folder containing csv files which are timestamped</param>
void ModelTubeNetwork::LoadAnimation(const std::string& DirectoryName) {
	ifstream fin;
	string filepath;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;
	
	cout<<"Processing tube animation frames from: "<<DirectoryName<<endl;

	dp = opendir( DirectoryName.c_str() );
	if (dp == NULL)
	{
		cout << "Error(" << errno << ") opening " << DirectoryName << endl;
		return; // errno;
	}
	
	int count=0;
	while ((dirp = readdir(dp)))
	{
		if (++count>10) break;
		filepath = DirectoryName + "/" + dirp->d_name;
		
		// If the file is a directory (or is in some way invalid) we'll skip it 
		if (stat( filepath.c_str(), &filestat )) continue;
		if (S_ISDIR( filestat.st_mode ))         continue;

		//load data in csv file and store it as a hash of YYYYMMDD_HHMMSS and Unique name
		//cout<<"Processing: "<<filepath<<endl;
		fin.open(filepath.c_str());

		string line;
		std::getline(fin,line); //skip header line
		while (!fin.eof()) {
			line.clear();
			std::getline(fin,line);
			if ((line.length()==0)||(line[0]=='#')) continue; //blank line or comment line
			//parse the line as a string stream separated by ,
			stringstream ss(line);
			std::vector<string> items;
			//TODO: I think this bit here needs optimising and also the quote removal?
			while (ss.good()) {
				string elem;
				std::getline(ss,elem,',');
				items.push_back(elem);
			}
			if (items.size()==15) {
				//line,trip,set,lat,lon,east,north,timetostation,location,stationcode,stationname,platform,platformdirectioncode,destination,destinationcode
				for (std::vector<std::string>::iterator it=items.begin(); it!=items.end(); ++it) {
					//remove ALL quotes
					std::string str=*it;
					str.erase(remove( str.begin(), str.end(), '\"' ),str.end());
					*it=str;
				}
				std::string DTCode = string(dirp->d_name).substr(11,15); //assuming they're all of the format trackernet_20140119_082400.csv
				//std::tm t = {0};
				//stringstream ss(DTCode);
				//stringstream ss("201406");
				//ss>>std::get_time(&t,"%Y%m"); //TODO: check leading zeroes
				//strptime(DTCode, "%Y%m%d_%H%M%S", &t); //UNDEFINED
				//OK, none of the above functions for parsing time work, so do it the hard way
				//TODO: check I've got all of this right
				//t.tm_year = std::atoi(DTCode.substr(0,4).c_str())-1900; //2014 should come out as 114
				//t.tm_mon = std::atoi(DTCode.substr(4,2).c_str())-1; //january is zero
				//t.tm_mday = std::atoi(DTCode.substr(6,2).c_str());
				//space
				//t.tm_hour = std::atoi(DTCode.substr(9,2).c_str());
				//t.tm_min = std::atoi(DTCode.substr(11,2).c_str());
				//t.tm_sec = std::atoi(DTCode.substr(13,2).c_str());
				//time_t DTTime = std::mktime(&t);
				AgentTime atime = AgentTime::FromString(DTCode);
				time_t DTTime = atime._DT;
				std::string lineCode = items[0];
				std::string tripcode = items[1];
				std::string setcode = items[2];
				std::string stationcode = items[9];
				float timetostation = atof(items[7].c_str());
				int dir = std::atoi(items[12].c_str());
				std::string UniqueName = lineCode+"_"+tripcode+"_"+setcode;

				//now write the hash record here, so it's hashed by time, then vehicle id, then comes the record
				tube_anim_record anim_rec;
				anim_rec.LineCode=lineCode[0];
				anim_rec.StationCode=stationcode;
				anim_rec.TimeToStation=timetostation;
				anim_rec.Direction=dir;
				tube_anim_frames[DTTime][UniqueName] = anim_rec;
				//cout<<"Written: "<<DTCode<<" "<<UniqueName<<" "<<stationcode<<endl;
			}
		}
		fin.close();
	}
	
	closedir( dp );

	cout<<"Finished processing animation frames. Found "<<tube_anim_frames.size()<<" animation frames."<<endl;
}

/// <summary>Generate a tube (spline) geometry for a specific Underground line as defined by the LineCode.</summary>
/// <param name="LineCode"></param>
/// <returns></returns>
NetGraphGeometry* ModelTubeNetwork::GenerateLineMesh(char LineCode) {
	//get the graph for the requested linecode using the map
	std::unordered_map<char,Graph*>::iterator itG = tube_graphs.find(LineCode);
	Graph* G = itG->second;
	//find its colour
	glm::vec3 LineColour = LineCodeToVectorColour(LineCode);
	//now build the geometry
	NetGraphGeometry* geom = new NetGraphGeometry(G,tube_stations,LineSize/*0.00025f*/,LineTubeSegments,LineColour);
	geom->Name="LINE_"+LineCode; //might as well name the object in case we need to find it in the scene graph

	return geom;
}


/// <summary>Generate one object for all the lines in the correct colours</summary>
/// <returns></returns>
Object3D* ModelTubeNetwork::GenerateMesh() {
	Object3D* o3d = new Object3D();
	o3d->Name="LondonUnderground";
	for (std::unordered_map<char,Graph*>::iterator it=tube_graphs.begin(); it!=tube_graphs.end(); ++it) {
		NetGraphGeometry* geom = GenerateLineMesh(it->first);
		o3d->AddChild(geom);
	}
	return o3d;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ABM

/// <summary>ABM initialisation</summary>
void ModelTubeNetwork::Setup() {
	//TODO: need to create a breed called nodes and drivers and set default shape to sphere and turtle
	//Check how NetLogo does this

	//AgentScript: agentBreeds "nodes" "drivers"
	//My version, closer to NetLogo: Breed("node","nodes"); and Breed("driver","drivers");
	//SetDefaultShape("node","sphere"); //takes too long to create 300 of them
	SetDefaultShape("node","cube");
	SetDefaultSize("node",StationSize/*0.001f*/);
	SetDefaultShape("driver","turtle");
	SetDefaultSize("driver",TrainSize/*0.005f*/);

	////////////Set animation mode
	AnimateMode=true;
	AnimationTimeMultiplier=10; //run at ten times normal speed

	loadStations(Filename_StationCodes); //station locations
	loadLinks(Filename_TubeODNetwork); //network from JSON origin destination file
	
	/////this needs to depend on the animate settings
	//loadPositions(Filename_TrackernetPositions); //train positions
	LoadAnimation("..\\data\\tube-anim");
	LoadAnimatePositions();
	//set current animation time to the first time in the data sample
	AnimationDT._DT=GetFirstAnimationTime();
	AnimationDT._fraction=0;
	FrameTimeN=AnimationDT._DT;
	/////

	Object3D* lu = GenerateMesh();
	lu->Name="LondonUnderground";
	//you shouldn't manipulate the scene graph directly, but instead create the equivalent of an ABM.Links object
	_pSceneGraph->push_back(lu); //this is bad, but only until ABM can represent the network
	_links._pSceneRoot = lu; //set the root scene graph object for the links to this network object
	

	//now create some tube trains (=drivers in netlogo and agentscript speak)
	//the AgentScript code did this by hatching them from the station node (vertex) that they are heading towards
	//..\\data\\trackernet_20140127_154200.csv
	//ABM::Agent* a = _agents.Hatch("driver");
	//a->SetXYZ(0.11464,51.46258,0); //not Brixton


	//debug
	//std::vector<ABM::Agent*> nodes = _agents.Ask("node");
	//for (std::vector<ABM::Agent*>::iterator it = nodes.begin(); it!=nodes.end(); ++it) {
	//	ABM::Agent* A = *it;
	//	if (A->Name=="WDN") {
	//		cout<<"Wimbledon"<<endl;
	//		std::vector<ABM::Link*> lnks = A->OutLinks();
	//		for (std::vector<ABM::Link*>::iterator it2 = lnks.begin(); it2!=lnks.end(); ++it2) {
	//			ABM::Agent* end1 = (*it2)->end1;
	//			ABM::Agent* end2 = (*it2)->end2;
	//			cout<<end1->Name<<"->"<<end2->Name<<" ";
	//		}
	//		cout<<endl;
	//	}
	//}
	//end debug

}

/// <summary>
/// Animation step virtual from ABM::Model. This calls one of two animation step functions based on whether we're animating from an archive, or displaying real data.
/// </summary>
void ModelTubeNetwork::Step(double Ticks) {
	if (AnimateMode) StepAnimation(Ticks);
	else StepRealTime(Ticks);
}

//visitor callback for filtering links by line and direction code
//bool is_line_dir() {
//	return false;
//}

//functor - check line and direction for driver agent, note logic is reversed as it's used in a remove_if to remove anything that's the wrong line or direction
struct checkLineDirection {
  const string lineCode;
  const int direction;
  checkLineDirection(const string& lineCode, const int direction) : lineCode(lineCode),direction(direction) {}
  bool operator()(ABM::Link* x) { return (x->Get<std::string>("lineCode")!=lineCode) || (x->Get<int>("direction")!=direction); } //note application of De Morgan's
};

//functor - check line for driver agent, logic reversed as above
struct checkLine {
  const string lineCode;
  checkLine(const string& lineCode) : lineCode(lineCode) {}
  bool operator()(ABM::Link* x) { return x->Get<std::string>("lineCode")!=lineCode; }
};

/// <summary>ABM simulation step code</summary>
/// <param name="Ticks">In this instance, ticks is the time interval since the last simulation step</param>
void ModelTubeNetwork::StepRealTime(double Ticks) {
	std::vector<ABM::Agent*> drivers = _agents.Ask("driver"); //should be drivers really, but haven't implemented plural breeds yet
	for (std::vector<ABM::Agent*>::iterator dIT = drivers.begin(); dIT!=drivers.end(); ++dIT)
	{
		ABM::Agent* d = *dIT;

		ABM::Agent* toNode = d->Get<ABM::Agent*>("toNode");
		d->Face(*toNode); //d.face d.toNode
		d->Forward(min(d->Get<float>("v") * (float)Ticks,(float)d->Distance(*toNode))); //d.forward Math.min d.v, d.distance d.toNode
		if (.00001f > d->Distance(*toNode)) //# or (d.distance d.toNode) < .01
		{
			ABM::Agent* pTestAgent0 = d->Get<ABM::Agent*>("toNode");
			ABM::Agent* pTestAgent1 = d->Get<ABM::Agent*>("fromNode");
			//cout<<"Agent: "<<d->Name<<" reached "<<d->Get<ABM::Agent*>("toNode")->Name<<" ";
			ABM::Agent* toNode = d->Get<ABM::Agent*>("toNode"); //not sure why, but if you join this with the line below it doesn't set d.fromNode
			d->Set<ABM::Agent*>("fromNode",toNode); // d.fromNode = d.toNode
			ABM::Agent* pTestAgent2 = d->Get<ABM::Agent*>("fromNode");
			//############################################
			//#pick a random one of the outlinks from this node
			//#also, you can't use "with" as it returns an array
			//lnks = (lnk for lnk in d.fromNode.myOutLinks() when lnk.lineCode==d.lineCode and lnk.direction==d.direction)
			std::vector<ABM::Link*> lnks = d->Get<ABM::Agent*>("fromNode")->OutLinks();
			std::string lineCode = d->Get<std::string>("lineCode");
			int dir = d->Get<int>("direction");
			std::vector<ABM::Link*>::iterator end = std::remove_if(lnks.begin(),lnks.end(),checkLineDirection(lineCode,dir)); //note end iterator
			//#console.log("LINKS: ",lnks)
			size_t length = end-lnks.begin();
			if (length>0)
			{
				//coffeescript - pick a new link randomly
				//l = lnks[u.randomInt lnks.length]
				//d.toNode = l.end2
				//d.v = l.velocity
				
				//hack, I'm just going to take the first
				//cout<<"(CONTINUE) ";
				//debug
				//for (vector<ABM::Link*>::iterator it=lnks.begin(); it!=end; ++it) {
				//	cout<<"["<<(*it)->end1->Name<<" "<<(*it)->end2->Name<<"] ";
				//}
				//end of debug
				//ABM::Link* l = lnks[0];
				ABM::Link* l = lnks.at(0);
				ABM::Agent* toNode = l->end2;
				d->Set<ABM::Agent*>("toNode",toNode);
				d->Set<float>("v",l->Get<float>("velocity"));
			}
			else
			{
				//#condition when we've got to the end of the line and need to change direction - drop the direction constraint
				//lnks = (lnk for lnk in d.fromNode.myOutLinks() when lnk.lineCode==d.lineCode)
				std::vector<ABM::Link*>::iterator end = std::remove_if(lnks.begin(),lnks.end(),checkLine(lineCode)); //note end iterator
				size_t length = end-lnks.begin();
				if (length>0)
				{
					//cout<<"(NEW DIR) ";
					//l = lnks[0]
					//d.direction=l.direction //#don't forget to change the direction - otherwise everybody gets stuck on the last link
					//d.toNode = l.end2
					//d.v = l.velocity
					ABM::Link* l = lnks[0];
					d->Set<int>("direction",l->Get<int>("direction")); //don't forget to change the direction - otherwise everybody gets stuck on the last link
					ABM::Agent* toNode = l->end2;
					d->Set<ABM::Agent*>("toNode",toNode);
					d->Set<float>("v",l->Get<float>("velocity"));
				}
				else
				{
					//#should never happen
					//console.log("ERROR: no end of line choice for driver: ",d)
					//#d.die ?
					ABM::Agent* toNode=d->Get<ABM::Agent*>("toNode");
					int dir=d->Get<int>("direction");
					//cout<<"Agent "<<d->Name<<" no options "<<toNode->Name<<" dir="<<dir<<endl;
					
					//debug
					//for (vector<ABM::Link*>::iterator it=lnks.begin(); it!=lnks.end(); ++it) {
					//	cout<<"["<<(*it)->end1->Name<<" "<<(*it)->end2->Name<<"] ";
					//}
					//cout<<endl;
					//end of debug
				}
			}
			//string fromNodeName = d->Get<ABM::Agent*>("fromNode")->Name;
			//string toNodeName = d->Get<ABM::Agent*>("toNode")->Name;
			//cout<<" now travelling from "<<fromNodeName<<" to "<<toNodeName<<endl;
		}
	}
}

/// <summary>
/// return the time of the first animation frame that we have data for
/// </summary>
time_t ModelTubeNetwork::GetFirstAnimationTime()
{
	time_t DT = tube_anim_frames.begin()->first;
	return DT;
}

/// <summary>
/// return the next animation frame time which is >=Now
/// If Now is past the end of the animation, then it returns the first frame time i.e. it loops
/// </summary>
//time_t ModelTubeNetwork::GetNextAnimationTimeFrom(const time_t Now)
//{
//}

/// <summary>
/// return the next animation time period from the current one and wraps around to the beginning if necessary i.e. it loops
//TODO: the looping is broken and it could do with some improvement
/// </summary>
time_t ModelTubeNetwork::GetNextAnimationTime(const time_t Current)
{
	/*time_t TimeN;
	map<time_t,std::map<std::string,struct tube_anim_record>>::iterator it = tube_anim_frames.find(Current);
	if (it==tube_anim_frames.end())
		TimeN = tube_anim_frames.begin()->first; //Current not found, so return first anim frame time
	else {
		++it;
		if (it==tube_anim_frames.end())
			TimeN = tube_anim_frames.begin()->first; //Current+1 not found, so return first anim frame time
		else
			TimeN = it->first; //return Current+1 time
	}*/
	time_t TimeN;
	for (map<time_t,std::map<std::string,struct tube_anim_record>>::iterator it = tube_anim_frames.begin(); it!=tube_anim_frames.end(); ++it) {
		TimeN = it->first;
		if (TimeN>Current) break;
	}
	return TimeN;
}

/// <summary>
/// position an agent with a position expressed as 30s from [StationName] on line B in direction [D]
/// </summary>
/// <returns>returns true if agent position was found and the agent data was updated</returns>
bool ModelTubeNetwork::PositionAgent(ABM::Agent* agent,char LineCode, float TimeToStation, std::string NextStation, int Direction) {
	bool Success = false;
	string strLineCode(1,LineCode); //string= 1 repetition of LineCode
	std::vector<ABM::Agent*> agent_d = _agents.With("name",NextStation); //destination node station
	if (agent_d.size()>0) {
		std::vector<ABM::Link*> links = agent_d.front()->InLinks();
		for (std::vector<ABM::Link*>::iterator itLinks = links.begin(); itLinks!=links.end(); ++itLinks)
		{
			ABM::Link* l = *itLinks;
			if ((l->Get<std::string>("lineCode")==strLineCode) && (l->Get<int>("direction")==Direction))
			{
				agent->Set<ABM::Agent*>("fromNode", l->end1);
				agent->Set<ABM::Agent*>("toNode", l->end2);
				agent->Set<float>("v", l->Get<float>("velocity")); //use pre-created velocity for this link
				agent->Set<int>("direction", l->Get<int>("direction"));
				agent->Set<std::string>("lineCode", strLineCode);
				agent->SetColour(LineCodeToVectorColour(LineCode));
				//interpolate position based on runlink and time to station
				float dist = l->end2->Distance(*(l->end1));
				float runlink = l->Get<float>("runlink");
				//TODO: here
				glm::dvec3 delta = l->end1->GetXYZ() - l->end2->GetXYZ(); //note this is the reverse direction end2 to end1
				double scale = TimeToStation/runlink;
				glm::dvec3 Pend = l->end2->GetXYZ();
				agent->SetXYZ(Pend.x+scale*delta.x,Pend.y+scale*delta.y,Pend.z+scale*delta.z); //linear interpolation X seconds back from target node based on runlink
				agent->Face(*agent->Get<ABM::Agent*>("toNode")); //put the face last as we need the position
				Success=true;
				break;
			}
		}
		//if !Success, then do it again, but relax the direction? Would cover situation where agent has got to the end of the line and turned around
		//you could always do this yourself though
	}
	return Success;
}

//internal function used by NextNodeOnPath
void Traverse(const std::string& LineCode, ABM::Agent* Begin, ABM::Agent* End, vector<ABM::Agent*> list)
{
	//TODO: not checking for cycles!!!!
	list.push_back(Begin);
	if (Begin==End) return; //guard case
	if (list.size()>5) return; //other guard case - any more than five stations depth and this is going to take a lot of time

	std::vector<ABM::Link*> lnks = Begin->OutLinks();
	std::vector<ABM::Link*>::iterator end = std::remove_if(lnks.begin(),lnks.end(),checkLine(LineCode)); //note end iterator
	for (std::vector<ABM::Link*>::iterator itLnks = lnks.begin(); itLnks!=end; ++itLnks) {
		ABM::Link* lnk = *itLnks;
		Traverse(LineCode,lnk->end2,End,list);
	}
}

/// <summary>
/// Return the next node after begin along a route between begin and end.
/// You need a line code!
/// </summary>
ABM::Agent* ModelTubeNetwork::NextNodeOnPath(const std::string& LineCode, ABM::Agent* Begin, ABM::Agent* End)
{
	if (Begin==End) return End; //trivial and shouldn't really happen

	//working on the basis that the tube network isn't that complicated, let's do it the brute force way
	
	vector<ABM::Agent*> list;
	Traverse(LineCode,Begin,End,list);
	if (list.back()==End) return list.at(1); //not the first, as that's where we are, but the second
	return nullptr;
}

/// <summary>
/// Version of Step to be called when animating using the tube_anim_frames store of positions from a set of CSV files.
/// The intention is just to call this from Step(Ticks) instead of the real-time version.
/// </summary>
void ModelTubeNetwork::StepAnimation(double Ticks)
{
	bool NewData = false; //TODO: check animation time and frames
	//AnimationDT=AnimationDT+0.5
	AnimationDT += 0.25f; // Ticks; ///10; //this is the time now, which is the last update time plus the ticks delta since then
	if (AnimationDT>=FrameTimeN) {
		//new data is available, so move the frame time ahead
		NewData = true;
		FrameTimeN = GetNextAnimationTime(AnimationDT._DT); //this is the next animation time >current i.e. the keyframe we're working towards
	}
	//FrameN is always ahead of the agents in time, so it contains the next data needed to make a decision on
	map<string,tube_anim_record> FrameN = tube_anim_frames[FrameTimeN];
	float FrameNFutureSecs = FrameTimeN - AnimationDT._DT; //this is how many seconds in to the future the FrameN time is - needed for timing offset and velocity
	FrameNFutureSecs-=AnimationDT._fraction; //and the fraction
	
	cout<<"Animation time: "<<AgentTime::ToString(AnimationDT)<<" Next Frame: "<<AgentTime::ToString(FrameTimeN)<<endl;

	std::vector<ABM::Agent*> drivers = _agents.Ask("driver"); //should be drivers really, but haven't implemented plural breeds yet
	for (std::vector<ABM::Agent*>::iterator dIT = drivers.begin(); dIT!=drivers.end(); ++dIT)
	{
		ABM::Agent* d = *dIT;
		string temp = d->Get<std::string>("Name");
		if (temp!="V_26_230") continue;

		ABM::Agent* toNode = d->Get<ABM::Agent*>("toNode");
		d->Face(*toNode);
		//the distance you move is the velocity times the simulation step - in this case hard coded to 1 frame = 0.1s - MAKE SURE this matches what you add to AnimationDT at the top
		//d->Forward(min(d->Get<float>("v") * (float)Ticks,(float)d->Distance(*toNode)));
		d->Forward(min(d->Get<float>("v") * 0.25f,(float)d->Distance(*toNode)));
		if (.00001f > d->Distance(*toNode))
		{
			//decision point - we've reached the next toNode, so we need to choose a new direction
			
			//AName, ADir, ALineCode and ANextStation are the current values stored on the agent
			string AName = d->Get<std::string>("Name"); //e.g. V_1_123
			//int ADir = d->Get<int>("direction");
			string ALineCode = d->Get<std::string>("lineCode");
			string ANextStation = toNode->Name;


			//the anim rec is the new data used to make the decision
			if (FrameN.count(AName)==0) {
				//this agent doesn't exist in the next data frame, so get rid of him TODO: check whether this is too early - he should get to his final station
				//cout<<"Die: agent "<<AName<<endl;
				d->Die();
				continue;
			}
			tube_anim_record anim_rec = FrameN[AName];

			
			//Frame next station, dir and timetostation is the data from the frame record
			string FrameNextStation = anim_rec.StationCode;
			double FrameTimeToStation = (double)anim_rec.TimeToStation;
			
			if (FrameNextStation!=ANextStation) { //if agent and frame stations equal, then all we can do is sit at this node and wait for new data
				std::vector<ABM::Link*> lnks = toNode->OutLinks();
				std::vector<ABM::Link*>::iterator end = std::remove_if(lnks.begin(),lnks.end(),checkLine(ALineCode)); //note end iterator
				bool Found=false;
				for (std::vector<ABM::Link*>::iterator itLnks = lnks.begin(); itLnks!=end; ++itLnks) {
					ABM::Link* lnk = *itLnks;
					if (lnk->end2->Name==FrameNextStation) {
						//update fromnode, tonode, and velocity
						d->Set<ABM::Agent*>("fromNode",toNode); //set agent's from node to the current to node that he's just reached
						toNode = lnk->end2; //switch the tonode to the new next station node that we've just found
						d->Set<ABM::Agent*>("toNode",toNode);
						double dist = d->Distance(*toNode);
						d->Set<float>("v",dist/(FrameNFutureSecs + FrameTimeToStation)); //make sure denominator isn't zero!!!!
						Found=true;
						break;
					}
				}
				//final catch if we can't find the next station any other way - look it up in the global node list and position tube manually (computationally expensive)
				if (!Found)
				{
					//We need a route from the current node to the next toNode on the frame data.
					//It's obviously skipped at least one station, so we need to find a route ourselves.
					//A dirty hack would be to check if there's only one route and take it, or skip if there are choice.

					//next station from FrameN not found on current next station's out links,so going to need to "jump" the tube's position
					//cout<<"Jump tube: "<<AName<<" station code "<<anim_rec.StationCode<<endl;
					//interpolate the position between the new next station and the last one
					if (!PositionAgent(d,anim_rec.LineCode,anim_rec.TimeToStation,anim_rec.StationCode,(int)anim_rec.Direction))
						if (!PositionAgent(d,anim_rec.LineCode,anim_rec.TimeToStation,anim_rec.StationCode,1-(int)anim_rec.Direction)) //try alternate direction (feed errors? agent dir != frame dir?)
							cerr<<"Error: station "<<FrameNextStation<<" not found for agent "<<AName<<endl;
				}
			}
		}
		else if (NewData) {
////////////////////////////////
			//covers the case where a new data frame causes an update to the velocity e.g. a tube was 3 mins from station in the last frame, but 3 mins later it's still 1 min away
/*			if (FrameNextStation == ANextStation) {
				//frame and agent's next station hasn't changed, so not there yet, update velocity (we implicitly assume direction hasn't changed as it can't have)
				double dist = d->Distance(*toNode); //this is how far I am from the next station based on my current position
				//use: l->Get<float>("velocity") to get the velocity for this link based on the official runlinks
				//we know how long they're telling us it's going to take to get to this station, so calculate a velocity based on v=dist/time
				//and the fact that the frame time is in the future i.e. 60 seconds from NOW, the tube will be 30 seconds from the station: 60+30=90s away now
				d->Set<float>("v",dist/(FrameNFutureSecs + FrameTimeToStation)); //make sure denominator isn't zero!!!!
			}
			else {
				bool Found=false;
				//different next station
				//check it hasn't changed direction and look at current next station's out links based on dir
				if (ADir==FrameDir) //still going the same way, so should be able to find the next station easily from the current next station's out links
				{
					std::vector<ABM::Link*> lnks = toNode->OutLinks();
					std::vector<ABM::Link*>::iterator end = std::remove_if(lnks.begin(),lnks.end(),checkLineDirection(ALineCode,ADir)); //note end iterator
					for (std::vector<ABM::Link*>::iterator itLnks = lnks.begin(); itLnks!=end; ++itLnks) {
						ABM::Link* lnk = *itLnks;
						if (lnk->end2->Name==FrameNextStation) {
							//update fromnode, tonode, and velocity
							d->Set<ABM::Agent*>("fromNode",toNode); //set agent's from node to the current to node that he's just reached
							toNode = lnk->end2; //switch the tonode to the new next station node that we've just found
							d->Set<ABM::Agent*>("toNode",toNode);
							double dist = d->Distance(*toNode);
							d->Set<float>("v",dist/(FrameNFutureSecs + FrameTimeToStation)); //make sure denominator isn't zero!!!!
							Found=true;
							break;
						}
					}
				}
*/
////////////////////////////////////////////////
		}

	}
	//how to you pick up any extra tubes on the frame list which aren't currently created as agents?

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
