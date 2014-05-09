
#include "Agents.h"
#include "Agent.h"

//geometry
#include "turtle.h"
#include "sphere.h"
#include "cuboid.h"
#include "cylinder.h"

#include <sstream>

namespace ABM {

	std::string intToString(int i)
	{
		std::stringstream ss;
		std::string s;
		ss << i;
		s = ss.str();

		return s;
	}

	/// <summary>Constructor</summary>
	Agents::Agents(void)
	{
		NumAgents=0;
		//initialise static pointer from Agent to Agents (this)
		//This is necessary so that an Agent can hatch another Agent, although Agents needs to keep the list
		Agent::_pParentAgents=this;
	}

	/// <summary>Destructor</summary>
	Agents::~Agents(void)
	{
		//free all the created agent structures
		for (std::vector<Agent*>::iterator it=_Agents.begin(); it!=_Agents.end(); ++it) {
			delete *it;
		}
	}

	/// <summary>Set the defaults for the agent based on his breed. OK, this is strange, but the Agents class has the defaults as it's global, not the agent itself</summary>
	void Agents::SetAgentDefaults(Agent& A)
	{
		std::string Breed = A._BreedName;
		A.size = GetDefaultSize(Breed);
		//OK, there are going to be some more here...
		//Also, the mesh doesn't get created here from shapename
	}

	/// <summary>
	/// Creates a new agent of a specific breed, creates the breed if it doesn't exist and adds the agent to the global agent list for the model.
	/// </summary>
	/// <param name="Breed">The breed of agent to create</param>
	/// <returns>A pointer to the new agent that has just been created.</returns>
	//Agent* Agents::SproutAgent(std::string Breed)
	//{
	//	std::set<std::string>::iterator it = _BreedNames.find(Breed);
	//	if (it==_BreedNames.end()) {
	//		//Breed doesn't exist, so create a new one
	//		_BreedNames.insert(Breed);
	//	}
	//	Agent* a = new Agent();
	//	a->_BreedName = Breed;
	//
	//	//create visual representation (mesh)
	//	//add to scenegraph
	//	
	//	return a;
	//}

	/// <summary>
	/// create a mesh for an agent based on its breed and name. The result is to set agent._pAgentMesh with a pointer to a new mesh object.
	/// </summary>
	void Agents::CreateShapeMesh(Agent& a)
	{
		Mesh* mesh;
		std::string ShapeName = GetDefaultShape(a._BreedName);
		if (ShapeName=="turtle")
			mesh = new Turtle(a.size); //HACK! need default turtle size, in fact defaults for all of these!
		else if (ShapeName=="sphere")
			mesh = new Sphere(a.size,10,10);
		else if (ShapeName=="cube")
			mesh = new Cuboid(a.size,a.size,a.size);
		else if (ShapeName=="cylinder")
			mesh = new Cylinder(a.size,a.size,8); //that's not a cylinder, that's an octagon

		a._pAgentMesh = mesh; //REMEMBER, you still have to add this to the scene and give the mesh a unique name
	}

	/// <summary>
	/// Creates a new agent of a specific breed, creates the breed if it doesn't exist and adds the agent to the global agent list for the model.
	/// </summary>
	/// <param name="Breed">The breed of agent to create</param>
	/// <returns>A pointer to the new agent that has just been created.</returns>
	Agent* Agents::Hatch(std::string Breed)
	{
		std::set<std::string>::iterator it = _BreedNames.find(Breed);
		if (it==_BreedNames.end()) {
			//Breed doesn't exist, so create a new one
			_BreedNames.insert(Breed);
		}
		//create agent
		Agent* a = new Agent();
		a->Number=NumAgents;
		++NumAgents;
		a->_BreedName = Breed;
		SetAgentDefaults(*a); //sets the size etc, but NOT the mesh - must wait for the breed to be set before calling this
		_Agents.push_back(a); //put him onto the global agent list

		//create visual representation (mesh)
		//add to scenegraph object that forms root for agents
		//Turtle* mesh = new Turtle(0.01f); //HACK! need default turtle size
		//mesh->Name="Agent_"+intToString(a->Number);
		//_pSceneRoot->AddChild(mesh);
		//a->_pAgentMesh = mesh; //THIS IS A HACK!
		//new code with mesh shape names
		CreateShapeMesh(*a); //create mesh for this breed shape
		a->_pAgentMesh->Name="Agent_"+intToString(a->Number); //name the mesh
		_pSceneRoot->AddChild(a->_pAgentMesh); //add mesh to scene

		return a;
	}

	/// <summary>
	/// TODO: Return a set of agents matching a description
	/// This should really be returning an iterator (i.e. std::find pattern)
	/// Need a breed name?
	/// </summary>
	/// <param name="VariableName">Name of the variable to check</param
	/// <param Name="Value">The value to check Name for</param>
	/// <returns>A list of agents matching the condition</returns>
	std::vector<ABM::Agent*> Agents::With(std::string VariableName,std::string Value)
	{
		//LogoVariant test;
		////this is a hack
		//test.type_id=LogoVariant::t_string;
		//test.as_string=Value;
		LogoVariant test(Value);

		std::vector<ABM::Agent*> Results;
		//for all the agents?
		for(std::vector<ABM::Agent*>::iterator it=_Agents.begin(); it!=_Agents.end(); ++it) {
			Agent* A = (*it);
			LogoVariant var = A->GetVar(VariableName);
			if (test==var) Results.push_back(*it);
		}

		return Results;
	}

	/// <summary>
	/// This might be best if we return an iterator - how? At the moment I build an agentset (i.e. a vector of agent pointers which are live)
	/// TODO: like with, but returns a single agent in its singular form e.g. ask agent 0 or vector in its plural form e.g. ask drivers
	std::vector<ABM::Agent*> Agents::Ask(std::string BreedName)
	{
		std::vector<ABM::Agent*> Results;
		for(std::vector<ABM::Agent*>::iterator it=_Agents.begin(); it!=_Agents.end(); ++it) {
			Agent* A = (*it);
			if (A->_BreedName==BreedName) Results.push_back(*it);
		}

		return Results;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//start of defaults get/set
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Get the properties structure for this Breed, or return an empty one if it doesn't already exist
	/// </summary>
	/// <returns>Either an existing agent defaults struct, or a new empty one if it didn't already exist.</returns>
	AgentDefaults Agents::GetDefaultProperties(std::string BreedName)
	{
		struct AgentDefaults props;
		props.shape_present=props.size_present=false;
		std::map<std::string,struct AgentDefaults>::iterator it = _defaultProperties.find(BreedName);
		if (it!=_defaultProperties.end()) props=it->second;
		return props;
	}

	/// <summary>
	/// Get the default shape for this particular breed. If no default has been created then it returns "turtle"
	/// </summary>
	/// <param name="BreedName">The name of the breed to set the default shape for</param>
	/// <param name="ShapeName">The plain text name of the shape i.e. "turtle", "sphere", "cube"</param>
	void Agents::SetDefaultShape(std::string BreedName, std::string ShapeName)
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
	std::string Agents::GetDefaultShape(std::string BreedName)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		if (props.shape_present) return props.shape;
		return "turtle";
	}

	/// <summary>
	/// </summary>
	void Agents::SetDefaultSize(std::string BreedName, float Size)
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
	float Agents::GetDefaultSize(std::string BreedName)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		if (props.size_present) return props.size;
		return 0.1f;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//end of defaults get/set
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
