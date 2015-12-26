
#include "Model.h"
#include "ellipsoid.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "sensor/geofence.h"

#include "text/TextUtilities.h"

namespace ABM {

	/// <summary>Construct a model which controls a scene graph</summary>
	/// <param name="pSceneGraph">The scene graph containing the object which this model controls</param>
	Model::Model(SceneGraphType* pSceneGraph)
	{
		_pSceneGraph=pSceneGraph;
		_pEllipsoid = new Ellipsoid(); //bit naughty, but it gets us the default WGS84 ellipsoid
	
		//create an object 3d in the scene as a placeholder for the agents
		Object3D* AgentsSceneRoot = new Object3D();
		AgentsSceneRoot->Name="_AGENTS_";
		_pSceneGraph->push_back(AgentsSceneRoot);
		_agents._pSceneRoot=AgentsSceneRoot; //I really want to get rid of this - shouldn't have to copy pointers to scene graph around
	
		//call setup for user defined model initialisation
		//NO! this does nothing
		//Setup();

		//User defined initialisation complete, now build the graphic representation of the links as a single mesh so we have something to look at
		Object3D* LinksSceneRoot = new Object3D();
		LinksSceneRoot->Name="_LINKS_"; //TODO: can we make this user defined as a scene can have multiple models? (And _AGENTS_?)
		//NO! need child model to call this? _links.Create3D(LinksSceneRoot);
		_pSceneGraph->push_back(LinksSceneRoot);
		_links._pSceneRoot=LinksSceneRoot; //Again, I would like to get rid of this
	}

	/// <summary>Destructor</summary>
	Model::~Model(void)
	{
		delete _pEllipsoid;
	}

	/// <summary>
	/// Sets the shader which is attached to any new agents that are created.
	/// TODO: you could make this an agent breed default if you want to be really clever?
	/// </summary>
	void Model::SetAgentShader(gengine::Shader* pShader) {
		_agents.agentShader=pShader;
	}

	/// <summary>Initialisation of model - probably override this</summary>
	void Model::Setup()
	{
	}

	/// <summary>Step the model forward Ticks units of time - probably override this</summary>
	/// <param name="Ticks">Number of time ticks to simulate for, so pass in this_time - last_time for real-time simulation</param>
	void Model::Step(double Ticks)
	{
	}

	/// <summary>Draw the model in the scene graph. Well, not draw the model exactly, but move all the agents around ready to be drawn.</summary>
	void Model::UpdateScene()
	{
		//maybe "Prepare" would be a better description?
		//this is going to be so interesting....
	}

	/// <summary>Create more turtles. Should probably return an AgentSet?</summary>
	/// <param name="Number"></param>
	void Model::CreateTurtles(int Number)
	{
	}

	/// <summary>
	/// Hack for Patches.PatchXY(X,Y).Sprout
	/// We don't have any patches yet and agents can't be created directly, so this is a hack for sprouting them from (non-existent) patches
	/// </summary>
	std::vector<Agent*> Model::PatchesPatchXYSprout(int X,int Y,int N,std::string BreedName)
	{
		std::vector<Agent*> Results;
		for (int i=0; i<N; i++) {
			Agent* a = _agents.Hatch(BreedName);
			//a->SetXYZ((float)X,(float)Y,0); //TODO: need a way of getting the patch coords here... casting ints won't necessarily work, need patch coord system - CHECK BOUNDING BOXES
			Results.push_back(a);
		}
		return Results;
	}

	/// <summary>
	/// Set the default shape to be used for any new agents of this breed
	/// </summary>
	/// <param name="BreedName">The breed to set the default shape for</param>
	/// <param name="ShapeName">The plain text name of the shape e.g. turtle, cube, sphere</param>
	//TODO: need to formalise the mapping between shape names and 3D geometry
	void Model::SetDefaultShape(std::string BreedName, std::string ShapeName)
	{
		//this mimics:
		//set-default-shape turtles [string]
		//set-default-shape links [string]
		//set-default-shape [breed] [string]

		//how do you do this? you really need to intercept BreedName=turtle of links and send the request to the correct one.
		//for now, just pass it on to the _agents map
		_agents.SetDefaultShape(BreedName,ShapeName);
	}

	void Model::SetDefaultSize(std::string BreedName, float Size)
	{
		//this mimicks:
		//set-default-size turtles [string]
		//set-default-size links [string]
		//set-default-size [breed] [string]
		_agents.SetDefaultSize(BreedName,Size);

	}

	void Model::Breed(std::string singular, std::string plural)
	{
		//TODO: create a breed with the given singular and plural names e.g. "node", "nodes" - not really necessary
	}

	void Model::DirectedLinkBreed(std::string singular, std::string plural)
	{
		//TODO: create directed graph breed type
	}

	void Model::UndirectedLinkBreed(std::string singular, std::string plural)
	{
		//TODO: create undirected graph breed type
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Extension methods not in NetLogo
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//need utility csv split function here (private)
	std::vector<std::string> SplitCSV(const std::string& Line) {
		std::vector<std::string> Fields;
		std::string Text="";
		bool InQuote=false;
		for (std::string::const_iterator it = Line.begin(); it!=Line.end(); ++it) {
			char ch = *it;
			switch (ch) {
			case '\"' :
				InQuote=!InQuote; //what happens with this: "abc"d,"xyz" at the moment you get [abcd,xyz]
				break;
			case ',' :
				if (InQuote) Text+=ch;
				else { Fields.push_back(Text); Text=""; }
				break;
			case ' ':
				if ((InQuote)||(Text.length()>0)) Text+=ch; //don't allow whitespace at start of field
				break;
			default :
				Text+=ch;
			}
		}
		if (Text.length()>0) Fields.push_back(Text);
		return Fields;
	}

	/// <summary>
	/// Extension method to load agents from a csv file.
	/// Load single agent time frame from data file. Create agents and initialise properties.
	/// </summary>
	void Model::LoadAgentsCSV(const std::string& Filename, const int SkipLines, std::function<Agent* (std::vector<std::string>)> func)
	{
		//TODO: this needs to be able to handle commas inside quotes
		int AgentCount=0;
		std::ifstream in_csv(Filename.c_str());
		if (in_csv.is_open()) {
			std::string line;
			for (int i=0; i<SkipLines; i++)
				std::getline(in_csv,line); //skip the header lines

			while (!in_csv.eof()) {
				line.clear();
				std::getline(in_csv,line);
				if (line.length()==0) continue; //blank line
				//parse the line as a string stream separated by ,
				/*std::stringstream ss(line);
				std::vector<std::string> items;
				while (ss.good()) {
					std::string elem;
					std::getline(ss,elem,',');

					//remove start and end quotes
					if (elem.length()>0) {
						if ((elem.at(0)=='\"')&&(elem.at(elem.length()-1)=='\"')) {
							elem=elem.substr(1,elem.length()-2);
						}
					}
					elem=geogl::text::TextUtilities::trim(elem);

					items.push_back(elem);
				}*/
				//use custom line parsing function that understands quoted strings
				std::vector<std::string> items = SplitCSV(line);
				//call processing function here (NOTE: you could just not return anything from this function, we're not using it)
				Agent* a = func(items);
				++AgentCount; //we don't really know how many were created, but probably one!
			}
		}
		//std::cout<<"Model::LoadAgentsCSV created "<<AgentCount<<" agents from "<<Filename<<std::endl;
	}

	/// <summary>
	/// EXTENSION - Not in regular NetLogo
	/// Set the lat lon position of an agent, using the ellipsoid stored by the model.
	/// Height is in metres.
	/// Note Lat, Lon in Degrees.
	/// </summary>
	void Model::SetGeodeticPosition(Agent* agent, double Lat, double Lon, double Height)
	{
		glm::vec3 P = _pEllipsoid->toVector(glm::radians(Lon),glm::radians(Lat),Height);
		agent->SetXYZ(P.x,P.y,P.z);
	}

	//added
	/// <summary>
	/// Kill all turtles
	/// </summary>
	void Model::ClearTurtles()
	{
		//NOTE: clear-turtles kills all the turtles in NetLogo

		//using while loop to kill the first agent on the list as the list gets changed every time an agent is killed, so an iterator won't work
		while (_agents.NumAgents>0) {
			_agents._Agents[0]->Die();
		}
	}

	/// <summary>
	/// Kill all turtles of a specific breed
	/// </summary>
	void Model::ClearTurtles(std::string BreedName)
	{
		//As the agent list gets changed as we're deleting agents, the method is to only delete from the head of the list, which then shrinks.
		//If the agent at the head of the list isn't the right breed, then the head counter gets moved to the next one on the list.
		int Head=0;
		while (Head<_agents.NumAgents) {
			if (_agents._Agents[Head]->_BreedName==BreedName) {
				_agents._Agents[Head]->Die();
			}
			else ++Head;
		}
	}

	/// <summary>
	/// Extension
	/// Test all sensors attached to this model.
	/// TODO: need better way of the model being able to run this transparently - at the moment it's down to the user implementation
	/// </summary>
	void Model::SensorTests()
	{
		//OK, so for all agents, for all sensors OR for all sensors, for all agents - which is better?
		//and do you run a single sensor test for every agent individually, or pass it a reference to all the agents.
		//I'm going for passing all agents to a sensor (allows for density sensors)
		for (std::vector<sensor::GeoFence*>::iterator it = _sensors.begin(); it!=_sensors.end(); ++it) {
			_agents.SensorTests(*it);
		}
	}

	/// <summary>
	/// Extension
	/// Clear the counters of all sensors hooked up to this model.
	/// </summary>
	void Model::ClearAllSensors()
	{
		for (std::vector<sensor::GeoFence*>::iterator it = _sensors.begin(); it!=_sensors.end(); ++it) {
			(*it)->Clear();
		}
	}

} //namespace ABM
