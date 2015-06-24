#pragma once

//this is good on #includes
//http://www.cplusplus.com/forum/articles/10627/

#include "scenegraph.h"
#include "netgraphgeometry.h"

#include "Agents.h"
#include "Links.h"


#include <string>
#include <vector>
#include <functional>

//TODO:
//an animation record is a set of agent properties for a specific time
//the idea is to be able to update the properties as the time changes and do it for any derived model easily
//i.e. the step can take the new data for the next keyframe and interpolate using agent logic which the user provides

//Forward declarations
class Ellipsoid;

namespace gengine {
	class Shader;
};

namespace ABM {

	//Forward declarations
	class Agent;

	//things we might want to do:
	//ABM.agents.with("o.name=='VIC');

	//to go
	//  move-turtles
	//  tick
	//end
	//to move-turtles
	//  ask turtles [
	//    right random 360
	//    forward 1
	//  ]
	//end

	//to swap-colors [turtle1 turtle2]
	//  let temp [color] of turtle1
	//  ask turtle1 [ set color [color] of turtle2 ]
	//  ask turtle2 [ set color temp ]
	//end

	//need to find elegant way of doing "ask drivers ..."
	//where this works because drivers is an agentset breed


	//default shapes:
	//First row: default, airplane, arrow, box, bug, butterfly, car
	//Second row: circle, circle 2, cow, cylinder, dot, face happy, face neutral
	//Third row: face sad, fish, flag, flower, house, leaf, line
	//Fourth row: line half, pentagon, person, plant, sheep, square, square 2
	//Fifth row: star, target, tree, triangle, triangle 2, truck, turtle
	//Sixth row: wheel, x

	//REPORTERS i.e. count etc

	///	<summary>
	/// Model is the base for the ABM. Create one of these and add agents and other stuff as necessary.
	/// Override Setup to construct the initial state of the model.
	/// Override Step to program the simulation step code.
	/// UpdateScene keeps the scene graph in step with the simulation.
	/// </summary>
	class Model
	{
	public:
		Model(SceneGraphType* pSceneGraph);
		~Model(void);
		Agents _agents; //class controlling the list of agents
		Links _links; //class controlling list of links between agents
		//std::vector<Link> _links; //all links
		//Graph _graph; //this is the graph structure behind the links in NetLogo
		//NetGraphGeometry* _graph; //physical rendering of graph structure
		//Patches?

		SceneGraphType* _pSceneGraph;

		void SetAgentShader(gengine::Shader* pShader);

		virtual void Setup();
		virtual void Step(double Ticks);
		void UpdateScene();

		void CreateTurtles(int Number);
		std::vector<Agent*> PatchesPatchXYSprout(int X,int Y,int N,std::string BreedName); //HACK! patches.patchxy(X,Y).sprout, you can't create an agent directly

		//defaults
		void SetDefaultShape(std::string BreedName, std::string ShapeName);
		void SetDefaultSize(std::string BreedName, float Size);

		//breeds (Agents)
		void Breed(std::string singular, std::string plural); //TODO: create a breed with the given singular and plural names e.g. "node", "nodes" - not really necessary
		//breeds (Links)
		void DirectedLinkBreed(std::string singular, std::string plural);
		void UndirectedLinkBreed(std::string singular, std::string plural);

		//Extension Methods not in NetLogo (and maybe LoadTurtles would be better?)
		void LoadAgentsCSV(const std::string& Filename, const int SkipLines, std::function<Agent* (std::vector<std::string>)> func);
		void SetGeodeticPosition(Agent* agent, double Lat, double Lon, double Height);

	protected:
		Ellipsoid* _pEllipsoid;
	};

}

