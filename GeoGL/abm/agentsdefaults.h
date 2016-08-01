#pragma once

/*

 * agentsdefaults.h
 *
 *  Created on: 6 Apr 2016
 *      Author: richard
 *
 *  This is used as a base class for links and agents in order for them to be able to store breed defaults.
 *  TODO: could do with adding things like numbers of vertices in tube rings for links and other 3D specific properties
 */

//need the gengine reference for glm
#include "gengine/gengine.h"
#include <string>
#include <map>
#include <set>


namespace ABM {

//contains all default property settings for an agent breed
	struct AgentDefaults {
		bool shape_present;
		bool size_present;
		bool colour_present;

		std::string shape;
		float size;
		glm::vec3 colour;
	};

class AgentsDefaults {
private:
	//storage for defaults, all maps of breed name against the variable
	std::map<std::string,struct AgentDefaults> _defaultProperties;
public:
	AgentsDefaults();
	virtual ~AgentsDefaults();

	//defaults
	AgentDefaults GetDefaultProperties(std::string BreedName);
	void SetDefaultShape(std::string BreedName, std::string ShapeName);
	std::string GetDefaultShape(std::string BreedName);
	void SetDefaultSize(std::string BreedName, float Size);
	float GetDefaultSize(std::string BreedName);
	void SetDefaultColour(std::string BreedName, glm::vec3 Colour);
	glm::vec3 GetDefaultColour(std::string BreedName);
};

} // namespace ABM

