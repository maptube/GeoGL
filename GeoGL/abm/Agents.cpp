
#include "Agents.h"
#include "Agent.h"
#include "sensor/geofence.h"

#include "gengine/shader.h"
#include "gengine/vertexformat.h"

//geometry
#include "agentobject.h"
#include "turtle.h"
#include "sphere.h"
#include "cuboid.h"
#include "cylinder.h"
#include "pyramid4.h"

#include <sstream>
#include <algorithm>

//TODO: you need an agentset class so you can do multiple with statements

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
		Birth=0; Death=0;
		//initialise static pointer from Agent to Agents (this)
		//This is necessary so that an Agent can hatch another Agent, although Agents needs to keep the list
		Agent::_pParentAgents=this;

		//create agent mesh lookup - a better way of doing this might be to create the shape when the breed is created i.e. dynamically,
		//but, since there are a limited number at the moment, we might as well do it here...
		_AgentMeshes["turtle"]=new Turtle(1.0,gengine::PositionColourNormal);
		_AgentMeshes["sphere"]=new Sphere(1.0,10,10);
		_AgentMeshes["cube"]=new Cuboid(1.0,1.0,1.0);
		_AgentMeshes["cylinder"]=new Cylinder(1.0,1.0,8);
		_AgentMeshes["pyramid4"]=new Pyramid4(1.0,1.0,1.0);
		//now go through all the template shape meshes and attach the shader
		for (std::map<std::string,Mesh2*>::iterator it=_AgentMeshes.begin(); it!=_AgentMeshes.end(); ++it) {
			it->second->AttachShader(agentShader,false);
		}
	}

	/// <summary>Destructor</summary>
	Agents::~Agents(void)
	{
		//free all the created agent structures
		for (std::vector<Agent*>::iterator it=_Agents.begin(); it!=_Agents.end(); ++it) {
			delete *it;
		}
		//delete mesh shape templates
		for (std::map<std::string,Mesh2*>::iterator it=_AgentMeshes.begin(); it!=_AgentMeshes.end(); ++it) {
			delete it->second;
		}
	}

	void Agents::SetAgentShader(gengine::Shader* pShader) {
		agentShader=pShader;
		for (std::map<std::string,Mesh2*>::iterator it=_AgentMeshes.begin(); it!=_AgentMeshes.end(); ++it) {
			it->second->AttachShader(agentShader,false);
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
		//original code that creates a new mesh for each agent in the scene
		//NEED TO KEEP THIS FOR PERFORMANCE TESTS
		//Mesh2* mesh;
		//std::string ShapeName = GetDefaultShape(a._BreedName);
		//if ((ShapeName.length()==0)||(ShapeName=="none"))
		//	mesh = nullptr;
		//else if (ShapeName=="turtle")
		//	mesh = new Turtle(a.size); //HACK! need default turtle size, in fact defaults for all of these!
		//else if (ShapeName=="sphere")
		//	mesh = new Sphere(a.size,10,10);
		//else if (ShapeName=="cube")
		//	mesh = new Cuboid(a.size,a.size,a.size);
		//else if (ShapeName=="cylinder")
		//	mesh = new Cylinder(a.size,a.size,8); //that's not a cylinder, that's an octagon
		//else if (ShapeName=="pyramid4")
		//	mesh = new Pyramid4(a.size,a.size,a.size);
		//
		//if (mesh!=nullptr)
		//	mesh->AttachShader(agentShader,false);
		//a._pAgentMesh = mesh; //REMEMBER, you still have to add this to the scene and give the mesh a unique name

		//new code that puts an AgentObject into the scene that uses a shared mesh object
		AgentObject* mesh;
		std::string ShapeName = GetDefaultShape(a._BreedName);
		if ((ShapeName.length()==0)||(ShapeName=="none"))
			mesh = nullptr;
		else {
			//TODO: need to trap the ShapeName not found here!!!!
			mesh = new AgentObject(_AgentMeshes[ShapeName]);
			mesh->SetSize(a.size,a.size,a.size);
			//attach shader here? should have already been done?
		}
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
		++Birth;
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
		if (a->_pAgentMesh!=nullptr) { //add mesh to scene unless there is none
			a->_pAgentMesh->Name="Agent_"+intToString(a->Number); //name the mesh
			_pSceneRoot->AddChild(a->_pAgentMesh); //add mesh to scene
		}

		return a;
	}

	/// <summary>
	/// Kill the specified agent
	/// </summary>
	void Agents::Die(Agent* A)
	{
		std::vector<Agent*>::iterator it = std::find(_Agents.begin(),_Agents.end(),A);
		_Agents.erase(it);
		--NumAgents; //do you need to do this, as it's a sequence number of created agents?
		++Death;
		if (A->_pAgentMesh!=nullptr) { //remove mesh from scene and free if this agent actually has a mesh specified
			_pSceneRoot->RemoveChild(A->_pAgentMesh); //remove from scenegraph
			delete A->_pAgentMesh; //and free the mesh
		}
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
		//LogoVariant test(Value);

		std::vector<ABM::Agent*> Results;
		//for all the agents?
		for(std::vector<ABM::Agent*>::iterator it=_Agents.begin(); it!=_Agents.end(); ++it) {
			Agent* A = (*it);
			//LogoVariant var = A->GetVar(VariableName);
			//if (test==var) Results.push_back(*it);
			
			//if (A->Name==Value) Results.push_back(*it); //quicker but very dirty - hardcode name
			//if (A->GetVar(VariableName)==test) Results.push_back(*it);

			//alternative method based on the fact that we're always looking for a string
			//std::string tmp = A->Get<std::string>(VariableName);
			//std::string tmp = A->GetString(VariableName);
			//if (Value==tmp) Results.push_back(*it);
			if (A->TestString(VariableName,Value)) Results.push_back(*it);
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
/*	AgentDefaults Agents::GetDefaultProperties(std::string BreedName)
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

	void Agents::SetDefaultColour(std::string BreedName, glm::vec3 Colour)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		props.colour_present=true;
		props.colour=Colour;
		_defaultProperties[BreedName]=props;
	}

	glm::vec3 Agents::GetDefaultColour(std::string BreedName)
	{
		struct AgentDefaults props = GetDefaultProperties(BreedName);
		if (props.colour_present) return props.colour;
		return glm::vec3(1.0f,1.0f,1.0f);
	}
*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//end of defaults get/set
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Extension
	/// Run the sensor test loop on every agent in this set
	/// </summary>
	void Agents::SensorTests(ABM::sensor::GeoFence* Sensor)
	{
		Sensor->Test(_Agents);
	}

}
