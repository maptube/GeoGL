
#include "Model.h"

namespace ABM {

	/// <summary>Construct a model which controls a scene graph</summary>
	/// <param name="pSceneGraph">The scene graph containing the object which this model controls</param>
	Model::Model(SceneGraphType* pSceneGraph)
	{
		_pSceneGraph=pSceneGraph;
	
		//create an object 3d in the scene as a placeholder for the agents
		Object3D* AgentsSceneRoot = new Object3D();
		AgentsSceneRoot->Name="_AGENTS_";
		_pSceneGraph->push_back(AgentsSceneRoot);
		_agents._pSceneRoot=AgentsSceneRoot; //I really want to get rid of this - shouldn't have to copy pointers to scene graph around
	
		//call setup for user defined model initialisation
		Setup();
	}

	/// <summary>Destructor</summary>
	Model::~Model(void)
	{
	}

	/// <summary>Initialisation of model - probably override this</summary>
	void Model::Setup()
	{
	}

	/// <summary>Step the model forward one unit of time - probably override this</summary>
	void Model::Step()
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
		//this mimicks:
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

}