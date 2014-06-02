#pragma once

#include <string>
#include <map>

#include "main.h"

#include "LogoVariantOwns.h"

//forward declarations
class Edge;	

namespace ABM {


	//forward declarations
	class Agent;
	class Links;
	
	//This class shadows the edges in the Graph structure, allowing an ABM model to get and set variables on links through
	//the standard NetLogo interface. The UserData for the Graph Edge contains a pointer to the link. Topology is stored in the
	//Graph, not the Link. Any operations to query Link ends refers back to the Graph.
	//The idea behind this is to make the more complex graph operations more efficient e.g. A*
	//Link is then just a wrapper for NetLogo Owns get/set (and link breeds?).
	class Link
	{
	public:
		static Links* _pParentLinks; //parent of all Link classes - Linkss, which needs to keep a list of its children
		Agent* end1; //agent at start end of link
		Agent* end2; //agent at destination end of link
		glm::vec3 colour; //colour of this link
		Edge* _GEdge; //the edge in the graph that this link relates to
		Link(void);
		~Link(void);
		
		//ownership - need to create wrapper for ownership functions as Link can't inherit from LogoVariantOwns as a base class
		//ownership interface
		//this is a copy of the Agent.h version, so ideally, I need to merge into one copy of code - how?
		std::map<std::string,LogoVariant> _owns; //key value pairs
		template <typename T>
		T Get(std::string Name) {
			std::map<std::string,LogoVariant>::iterator it = _owns.find(Name);
			if (it!=_owns.end()) {
				return it->second.Get<T>();
			}
			return 0;
		}

		template <typename T>
		void Set(std::string Name, T Value) {
			LogoVariant LVar(Value);
			_owns[Name]=LVar;
		}
		
		//template <typename T>
		//T Get(std::string VarName) {
		//	//return NULL;
		//	return 0;
		//};
		//Get<int>
		//template <>
		//int Get<int>(std::string VarName) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_integer;
		//};
		//Get<float>
		//template <>
		//float Get<float>(std::string VarName) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_float;
		//};
		//Get<bool>
		//template <>
		//bool Get<bool>(std::string VarName) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_bool;
		//};
		//Get<string>
		//template <>
		//std::string Get<std::string>(std::string VarName) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_string;
		//};
		//Get<Agent*>
		//template <>
		//Agent* Get<Agent*>(std::string VarName) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_pAgent;
		//};
		//Get<Link*>
		//template <>
		//Link* Get<Link*>(std::string VarName) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_pLink;
		//};

		//Get<LogoVariant> used internally
		//template <>
		//LogoVariant& Get<LogoVariant>(std::string Name) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(Name);
		//	return &(*it->second);
		//}
		LogoVariant GetVar(std::string VarName) {
			std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
			return it->second;
		};
		//Set
		//template <typename T>
		//void Set(std::string VarName, T Value) {
		//	//TODO: should also keep a global map of what this agent "owns"
		//	LogoVariant var(T);
		//	//_owns.insert(std::pair<std::string,LogoVariant>(VarName,var));
		//	_owns[VarName]=var;
		//};
		//template<>
		//void Set<int>(std::string VarName, int Value) {
		//	LogoVariant var(Value);
		//	//_owns.insert(std::pair<std::string,LogoVariant>(VarName,var));
		//	_owns[VarName]=var;
		//};
		//template<>
		//void Set<bool>(std::string VarName, bool Value) {
		//	LogoVariant var(Value);
		//	//_owns.insert(std::pair<std::string,LogoVariant>(VarName,var));
		//	_owns[VarName]=var;
		//};
		//template<>
		//void Set<float>(std::string VarName, float Value) {
		//	LogoVariant var(Value);
		//	//_owns.insert(std::pair<std::string,LogoVariant>(VarName,var));
		//	_owns[VarName]=var;
		//};
		//template<>
		//void Set<std::string>(std::string VarName, std::string Value) {
		//	LogoVariant var(Value);
		//	//_owns.insert(std::pair<std::string,LogoVariant>(VarName,var));
		//	_owns[VarName]=var;
		//};
		//template<>
		//void Set<Agent*>(std::string VarName, Agent* Value) {
		//	LogoVariant var(Value);
		//	//_owns.insert(std::pair<std::string,LogoVariant>(VarName,var));
		//	_owns[VarName]=var;
		//};
		//template<>
		//void Set<Link*>(std::string VarName, Link* Value) {
		//	LogoVariant var(Value);
		//	//_owns.insert(std::pair<std::string,LogoVariant>(VarName,var));
		//	_owns[VarName]=var;
		//};

	};

}

