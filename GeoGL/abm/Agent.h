#pragma once

#include "mesh2.h"
#include "LogoVariantOwns.h"

#include <iostream>
#include <string>
#include <map>

//forward declarations
class Vertex;
class AgentObject;


namespace ABM {

	//forward declarations
	//class LogoVariantOwns;
	class Link;
	class Agents;
	
	//An agent can't create itself - it needs the Agents class to do it, otherwise we couldn't track them globally
	//Can an agent manipulate the scene graph though?
	class Agent
	{
		//this needs to be private
	public:
		//Mesh2* _pAgentMesh; //pointer to the mesh representing this agent in the 3D scene graph
		AgentObject* _pAgentMesh; //pointer to the mesh representing this agent in the 3D scene graph
	private:
		glm::dvec3 position; //copy of position so agents without meshes can have position
		glm::mat4 agentMatrix; //copy of matrix so agents without meshes can have position and orientation
	public:
		Agent(void);
		~Agent(void);

		//void SynchroniseWithSceneGraph(Object3D* pAgentsRoot);

		static Agents* _pParentAgents; //parent of all Agent classes - Agents, which needs to keep a list of its children
		
		int Number; //unique agent number
		std::string _BreedName;
		//std::string _AgentName; //unique key
		std::string Name; //unique key (is it unique?)
		//colour
		glm::vec3 _colour; //this should be private
		void SetColour(glm::vec3 new_colour);
		glm::vec3 GetColour();
		//position
		//orientation
		//shape
		float size;

		//TODO: need a better way of merging get/set ownership with object properties. At the moment, "name" is hard coded to
		//return this.Name if you Get<string>("name")
		//ownership - need to create wrapper for ownership functions as Agent can't inherit from LogoVariantOwns as a base class
		//ownership interface
		std::map<std::string,LogoVariant> _owns; //key value pairs
		//template <typename T>
		//T Get(std::string Name) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(Name);
		//	if (it!=_owns.end()) {
		//		if (typeid(T).hash_code()==typeid(int).hash_code()) {
		//			if (it->second.type_id==LogoVariant::t_int)
		//				return (T)(it->second.as_integer);
		//		}
		//		else if (typeid(T).hash_code()==typeid(bool).hash_code()) {
		//			if (it->second.type_id==LogoVariant::t_bool)
		//				return (T)(it->second.as_bool);
		//		}
		//		else if (typeid(T).hash_code()==typeid(float).hash_code()) {
		//			if (it->second.type_id==LogoVariant::t_float)
		//				return (T)(it->second.as_float);
		//		}
		//		//else if (typeid(T).hash_code()==typeid(std::string).hash_code()) {
		//		//	if (it->second.type_id==LogoVariant::t_string)
		//		//		return (T)(it->second.as_string);
		//		//}
		//		else if (typeid(T).hash_code()==typeid(Agent*).hash_code()) {
		//			if (it->second.type_id==LogoVariant::t_pAgent)
		//				return (T)(it->second.as_pAgent);
		//		}
		//		else if (typeid(T).hash_code()==typeid(Link*).hash_code()) {
		//			if (it->second.type_id==LogoVariant::t_pLink)
		//				return (T)(it->second.as_pLink);
		//		}
		//	}
		//	//return NULL;
		//	return 0;
		//};

		template <typename T>
		T Get(std::string Name) {
			if (Name=="Name") return LogoVariant(this->Name).Get<T>(); //internal property Name which maps to the real object property (need to cast to LogoV due to <T>)
			std::map<std::string,LogoVariant>::iterator it = _owns.find(Name);
			if (it!=_owns.end()) {
				return it->second.Get<T>();
			}
			std::cerr<<"Agent property "<<Name<<" not found"<<std::endl;
			return 0; //what type are we supposed to return?
		}

		template <typename T>
		void Set(std::string Name, T Value) {
			//TODO: allow set on property Name?
			LogoVariant LVar(Value);
			_owns[Name]=LVar;
		}

		//Get<int>
		//template <>
		//int Get<int>(std::string VarName) {
		//	std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_integer;
		//};
		//Get<float>
		//template <>
		//float Get<float>(std::string VarName) {
		//	std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_float;
		//};
		//Get<bool>
		//template <>
		//bool Get<bool>(std::string VarName) {
		//	std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_bool;
		//};
		//Get<string>
		//template <>
		//std::string Get<std::string>(std::string VarName) {
		//	if (VarName=="name") return Name; //built-in value
		//	std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_string;
		//};
		//Get<Agent*>
		//template <>
		//Agent* Get<Agent*>(std::string VarName) {
		//	std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_pAgent;
		//};
		//Get<Link*>
		//template <>
		//Link* Get<Link*>(std::string VarName) {
		//	std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
		//	return it->second.as_pLink;
		//};

		//HACK for performance check!!!!!!
		//This is much faster than getting a value back and testing it
		bool TestString(const std::string& VarName,const std::string& Test) {
			if (VarName=="name") return Name==Test; //built-in value
			if (_owns.size()==0) return false;
			std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
			return (*it->second.value.as_pString)==Test;
			return false;
		}

		//Get<LogoVariant> used internally
		//template <>
		//LogoVariant& Get<LogoVariant>(std::string Name) {
		//	std::unordered_map<std::string,LogoVariant>::iterator it = _owns.find(Name);
		//	return &(*it->second);
		//}
		LogoVariant GetVar(std::string VarName) {
			if (VarName=="name") return LogoVariant(Name); //built-in value
			std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
			return it->second;
		};
		//Set
		//template <typename T>
		//void Set(std::string VarName, T Value) {
		//	//TODO: should also keep a global map of what this agent "owns"
		//	LogoVariant var(T);
		//	//if (typeid(T).hash_code()==typeid(int).hash_code()) {
		//	//	var.type_id=LogoVariant::t_int;
		//	//	var.as_integer=(int)Value;
		//	//}
		//	//else if (typeid(T).hash_code()==typeid(bool).hash_code()) {
		//	//	var.type_id=LogoVariant::t_bool;
		//	//	var.as_bool=(bool)Value;
		//	//}
		//	//else if (typeid(T).hash_code()==typeid(float).hash_code()) {
		//	//	var.type_id=LogoVariant::t_float;
		//	//	var.as_float=(float)Value;
		//	//}
		//	//else if (typeid(T).hash_code()==typeid(std::string).hash_code()) {
		//	//	var.type_id=LogoVariant::t_string;
		//	//	var.as_string=Value;
		//	//}
		//	//else if (typeid(T).hash_code()==typeid(Agent*).hash_code()) {
		//	//	var.type_id=LogoVariant::t_pAgent;
		//	//	var.as_pAgent=(Agent*)Value;
		//	//}
		//	//else if (typeid(T).hash_code()==typeid(Link*).hash_code()) {
		//	//	var.type_id=LogoVariant::t_pLink;
		//	//	var.as_pLink=(Link*)Value;
		//	//}
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


		//create, destroy
		std::vector<Agent*> Hatch(int N, std::string BreedName);
		void Die(void);

		//Links
		//these need to be functions that get the data from the graph
		//std::vector<Link*> _InLinks; //list of all links coming in to this agent
		//std::vector<Link*> _OutLinks; //list of all links going out of this agent
		Vertex* _GVertex; //pointer to graph vertex for this agent
		std::vector<Link*> InLinks();
		std::vector<Link*> OutLinks();

		//movement and orientation
		glm::dvec3 GetXYZ();
		double xcor(void);
		double ycor(void);
		double zcor(void); //added this
		//random-x-cor
		//random-y-cor
		//random-z-cor
		//pxcor()
		//pycor()
		void Face(Agent& A);
		void Forward(float d);
		void Back(float d);
		void Left(float d);
		void Right(float d);
		void Up(float d); //added this
		void Down(float d); //added this
		void SetXYZ(const double X, const double Y, const double Z); //should be setxy
		void MoveTo(Agent& A);
		

		//measurement, calculation
		double Distance(Agent& A);
		bool InBox(Agent& A,double Distance);

		//drawing, style
		void PenUp(void);
		void PenDown(void);
		void PenErase(void);
		void PenSize(void);

		//extension methods not in NetLogo
		//TODO:
		/// <summary>
		/// Test whether a variable is owned by an agent, returns true if it is.
		/// Should this be called just 'Owns'?
		/// </summary>
		bool OwnsVariable(std::string VarName) {
			if (VarName=="name") return true; //built in value
			std::map<std::string,LogoVariant>::iterator it = _owns.find(VarName);
			return it!=_owns.end();
		}

	};

}

