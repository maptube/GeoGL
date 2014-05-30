//Tube Network functions
//Specialisation of functions required to load and build a London Underground network
#include "model_tubenetwork.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iterator>
#include <Windows.h>
#include <math.h>

using namespace std;

#include "json/json.h"
#include "json/reader.h"

#include "ellipsoid.h"
#include "netgraphgeometry.h"

#include "Link.h"
#include "Agent.h"
//#include "Model.h"

//define locations of configuration files
const std::string ModelTubeNetwork::Filename_StationCodes = "..\\data\\station-codes.csv"; //station locations
const std::string ModelTubeNetwork::Filename_TubeODNetwork = "..\\data\\tube-network.json"; //network from JSON origin destination file
const std::string ModelTubeNetwork::Filename_TrackernetPositions = "..\\data\\trackernet_20140127_154200.csv"; //train positions

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

/**
* Load the tube network from a json file
* TODO: this is hacked to only load the zero direction so that the geometry can be created.
*/
/// <summary>
/// </summary>
/// <param name="NetworkJSONFilename"></param>
/// <param name="StationsCSVFilename"></param>
void ModelTubeNetwork::load(std::string NetworkJSONFilename, std::string StationsCSVFilename) {
//NOT USED
/*
	stringstream ss;
	//ifstream in_json("C:\\richard\\projects\\VC++\\GeoGL\\GeoGL\\data\\tube-network-simple.json"); /// *"..\\GeoGL\\data\\tube-network.json"* /
	//ifstream in_json("C:\\richard\\projects\\VC++\\GeoGL\\GeoGL\\data\\tube-network.json"); /// *"..\\GeoGL\\data\\tube-network.json"* /
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
	//const string dirs[] = { string("0"), string("1") }; //bi-directional
	const string dirs[] = { string("0") }; //uni-directional

	for (int i=0; i<10; i++) {
		Graph *G = new Graph(true);
		std::unordered_map<std::string,int> VertexNames; //lookup between vertex names and ids in the network graph (need a new lookup each time a new graph is started)
		const Json::Value jsLine = root[linecodes[i]];
		for (int dir=0; dir<=0; dir++) { //HACK for unidirectional
			const Json::Value jsLineDir = jsLine[dirs[dir]];
			std::string Label = linecodes[i]+"_"+dirs[dir]; //e.g. V_0 for Victoria Northbound or V_1 for Southbound
			for (unsigned int v = 0; v < jsLineDir.size(); ++v ) {
				//cout<<"data:"<<jsLineDir[v]<<endl;
				//OutputDebugStringW(L"data:");
				//OutputDebugStringW(jsLineDir[v].asString().c_str()); //how? _T _W
				//OutputDebugStringA(jsLineDir[v].asString().c_str());
				// "o", "d", "r"
				//velocity = dist/runlink
				Json::Value node = jsLineDir[v];
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
				std::unordered_map<std::string,int>::const_iterator it;
				it = VertexNames.find(o);
				if (it==VertexNames.end()) {
					//not found, so create a new vertex
					VOrigin = G->AddVertex();
					VOrigin->_Name=o; //station 3 letter code
					VertexNames.insert(pair<string,int>(o,VOrigin->_VertexId)); //push name and vertex id onto hash
				}
				else VOrigin = G->_Vertices[it->second];
				//destination
				it = VertexNames.find(d);
				if (it==VertexNames.end()) {
					//not found, so create a new vertex
					VDestination = G->AddVertex();
					VDestination->_Name=d; //station 3 letter code
					VertexNames.insert(pair<string,int>(d,VDestination->_VertexId)); //push name and vertex id onto hash
				}
				else VDestination = G->_Vertices[it->second];
				//now connect the pair - THIS RETURNS THE EDGE IF YOU NEED IT
				Edge* e = G->ConnectVertices(VOrigin,VDestination,Label,(float)r); //node weight is the runlink in seconds
				//create an ABM:Link to shadow the graph edge
				//TODO: really, you should create the link and the graph gets updated in the background i.e. ABM is the user interface
				//Need agents here!
				//_links.Create(
			}
		}
		tube_graphs.insert(pair<char,Graph*>(linecodes[i].c_str()[0],G));
	}

	//create station nodes for ABM - iterate through the data we've just loaded
	for (std::unordered_map<string,struct GraphNameXYZ>::iterator it = tube_stations->begin(); it!=tube_stations->end(); ++it) {
		glm::vec3 P = it->second.P;

		vector<ABM::Agent*> AList = PatchesPatchXYSprout(P.x,P.y,1,"node");
		ABM::Agent *a = AList.front();
		
		//ABM::Agent* a = _agents.Hatch("node"); //alternative method to sprout
		
		a->SetXYZ(P.x,P.y,P.z);
		//a->SetXYZ(0.11464,51.46258,0);
		a->Name=it->first;
		//std::cout<<"agent "<<P.x<<","<<P.y<<","<<P.z<<std::endl;
		//and set him to a sphere?
	}
	//OK, so we now have agent nodes sitting on all the stations
*/
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
				tube_stations->insert(make_pair<string,struct GraphNameXYZ>(code1,pos));
				
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
				float dx = L->end2->xcor()-L->end1->xcor();
				float dy = L->end2->ycor()-L->end1->ycor();
				float dz = L->end2->zcor()-L->end1->zcor();
				float dist = sqrt(dx*dx+dy*dy+dz*dz);
				L->Set<float>("velocity",dist/(float)r);
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
	NetGraphGeometry* geom = new NetGraphGeometry(G,tube_stations,100/*0.00025f*/,10,LineColour);
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
	SetDefaultSize("node",200.0f/*0.001f*/);
	SetDefaultShape("driver","turtle");
	SetDefaultSize("driver",600.0f/*0.005f*/);

	loadStations(Filename_StationCodes); //station locations
	loadLinks(Filename_TubeODNetwork); //network from JSON origin destination file
	loadPositions(Filename_TrackernetPositions); //train positions
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
void ModelTubeNetwork::Step(double Ticks) {
	std::vector<ABM::Agent*> drivers = _agents.Ask("driver"); //should be drivers really, but haven't implemented plural breeds yet
	for (std::vector<ABM::Agent*>::iterator dIT = drivers.begin(); dIT!=drivers.end(); ++dIT)
	{
		ABM::Agent* d = *dIT;

		ABM::Agent* toNode = d->Get<ABM::Agent*>("toNode");
		d->Face(*toNode); //d.face d.toNode
		d->Forward(min(d->Get<float>("v") * (float)Ticks,d->Distance(*toNode))); //d.forward Math.min d.v, d.distance d.toNode
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
