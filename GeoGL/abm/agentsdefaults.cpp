/*
 * agentsdefaults.cpp
 *
 *  Created on: 6 Apr 2016
 *      Author: richard
 */

#include "agentsdefaults.h"

namespace ABM {

AgentsDefaults::AgentsDefaults() {
	// TODO Auto-generated constructor stub

}

AgentsDefaults::~AgentsDefaults() {
	// TODO Auto-generated destructor stub
}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//start of defaults get/set
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Get the properties structure for this Breed, or return an empty one if it doesn't already exist
	/// </summary>
	/// <returns>Either an existing agent defaults struct, or a new empty one if it didn't already exist.</returns>
	AgentDefaults AgentsDefaults::GetDefaultProperties(std::string BreedName)
	{
		struct AgentDefaults props;
		props.shape_present=props.size_present=props.colour_present=false;
		std::map<std::string,struct AgentDefaults>::iterator it = _defaultProperties.find(BreedName);
		if (it!=_defaultProperties.end()) props=it->second;
		return props;
	}

	/// <summary>
	/// Get the default shape for this particular breed. If no default has been created then it returns "turtle"
	/// </summary>
	/// <param name="BreedName">The name of the breed to set the default shape for</param>
	/// <param name="ShapeName">The plain text name of the shape i.e. "turtle", "sphere", "cube"</param>
	void AgentsDefaults::SetDefaultShape(std::string BreedName, std::string ShapeName)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		props.shape_present=true;
		props.shape=ShapeName;
		//std::cout<<"SetDefaultShape before "<<BreedName<<" size="<<_defaultProperties.size()<<std::endl;
		//_defaultProperties.insert(std::pair<std::string,struct AgentDefaults>(BreedName,props)); //doesn't work - is it refusing to insert a second identical key?
		_defaultProperties[BreedName]=props;
		//std::cout<<"SetDefaultShape after "<<BreedName<<" size="<<_defaultProperties.size()<<std::endl;
	}

	/// <summary>
	/// Get the default shape for this particular breed. If no default has been created then it returns "turtle"
	/// </summary>
	/// <param name="BreedName">The name of the breed to get the default shape for</param>
	/// <returns>The plain text string of the shape name, or "turtle" if no default has been set for this breed.</returns>
	std::string AgentsDefaults::GetDefaultShape(std::string BreedName)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		if (props.shape_present) return props.shape;
		return "turtle";
	}

	/// <summary>
	/// </summary>
	void AgentsDefaults::SetDefaultSize(std::string BreedName, float Size)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		props.size_present=true;
		props.size=Size;
		//std::cout<<"SetDefaultSize before "<<BreedName<<" size="<<_defaultProperties.size()<<std::endl;
		//_defaultProperties.insert(std::pair<std::string,struct AgentDefaults>(BreedName,props));
		_defaultProperties[BreedName]=props;
		//std::cout<<"SetDefaultSize after "<<BreedName<<" size="<<_defaultProperties.size()<<std::endl;
	}

	/// <summary>
	/// </summary>
	/// <returns>The default size if found, otherwise it returns 0.1 (better size default than 1.0 when using lat/lon coords)</returns>
	float AgentsDefaults::GetDefaultSize(std::string BreedName)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		if (props.size_present) return props.size;
		return 0.1f;
	}

	void AgentsDefaults::SetDefaultColour(std::string BreedName, glm::vec3 Colour)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		props.colour_present=true;
		props.colour=Colour;
		_defaultProperties[BreedName]=props;
	}

	glm::vec3 AgentsDefaults::GetDefaultColour(std::string BreedName)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		if (props.colour_present) return props.colour;
		return glm::vec3(1.0f,1.0f,1.0f);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//end of defaults get/set
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


} // namespace ABM
