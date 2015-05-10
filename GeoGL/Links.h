#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

//forward declarations
class Graph;
class Object3D;

namespace ABM {

	//forward declarations
	class Agent;
	class Link;
	
	/// <summary>
	/// Class to handle all operations on links. Creates link between two agents and keeps the physical representation of the network up to date.
	/// Links have breeds in the same way as agents and also own things
	/// </summary>
	class Links
	{
	protected:
		//std::set<std::string> _BreedNames;
		//TODO: one graph per breed
		std::unordered_map<std::string,Graph*> _BreedGraphs;
		//TODO: _graph needs to come out in favour of breeds graphs
		Graph* _graph; //this is the graph structure underlying the agent links, basically, the links are references to vertices
		std::vector<Link*> _myLinks; //list of created links - do you need this? it's a duplicate of the edges
	public:
		Links(void);
		~Links(void);

		Object3D* _pSceneRoot; //Root of links in scene graph. I'd like to get rid of this, but we need it to keep the 3D in step

		Link* CreateLink(std::string Breed, Agent* AStart, Agent* AEnd);

		void Create3D(Object3D* Parent); //create meshes for 3D by flattening graph and creating tube geometry

		//size_t NumLinks() { return _myLinks.size(); }

	};

}
