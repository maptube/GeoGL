#pragma once

#include <string>
#include <vector>
#include <set>

#include "opengl4.h"
#include "Agent.h"

namespace ABM {
	//list of Agent shapes e.g. circle, turtle, cube, tube, cone etc?

	//forward declare
	//class Agent;

	//contains all default property settings for an agent breed
	struct AgentDefaults {
		bool shape_present;
		bool size_present;

		std::string shape;
		float size;
	};

	class Agents
	{
	private:
		//storage for defaults, all maps of breed name against the variable
		//std::unordered_map<std::string,std::string> _defaultShape;
		//std::unordered_map<std::string,float> _defaultSize;
		std::map<std::string,struct AgentDefaults> _defaultProperties;

		void CreateShapeMesh(Agent& a);
	public:
		Agents(void);
		~Agents(void);

		Object3D* _pSceneRoot; //Root of agents in scene graph. I'd like to get rid of this, but we need it to keep the 3D in step

		int NumAgents; //counter for how many agents are in the model
		unsigned int Birth; //number of agents created in the last animation frame
		unsigned int Death; //number of agents destroyed in the last animation frame
		std::set<std::string> _BreedNames;
		std::vector<ABM::Agent*> _Agents; //list of all agents?

		void SetAgentDefaults(Agent& A);

		//Agent methods
		//Agent* SproutAgent(std::string Breed);
		Agent* Hatch(std::string Breed);
		void Die(Agent* A);
		std::vector<ABM::Agent*> With(std::string VariableName,std::string Value); //quick version for just one variable name
		//TODO: you could pass a function to WITH as the selector (visitor pattern)
		std::vector<ABM::Agent*> Ask(std::string BreedName); //For(breedname) ? i.e. for d in drivers
		void Owns(void);

		//defaults
		AgentDefaults GetDefaultProperties(std::string BreedName);
		void SetDefaultShape(std::string BreedName, std::string ShapeName);
		std::string GetDefaultShape(std::string BreedName);
		void SetDefaultSize(std::string BreedName, float Size);
		float GetDefaultSize(std::string BreedName);
	};

}

