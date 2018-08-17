#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

#include "agentsdefaults.h"

//forward declarations
class Graph;
class Vertex;
class Object3D;

namespace ABM {

	//forward declarations
	class Agent;
	class Link;
	
	/// <summary>
	/// Class to handle all operations on links. Creates link between two agents and keeps the physical representation of the network up to date.
	/// Links have breeds in the same way as agents and also own things. The BreedName controls the colour.
	/// There is a single graph structure which holds all the links (all breeds), which share agent vertices. This avoid the nasty problem if there is
	/// a different graph for each breed, where you can't follow the links for a breed graph if the vertices are shared because the vertex out links
	/// contain edges which don't belong to the breed. The simplest method is to have a single graph and use the edge labels to store the breed name,
	/// then use a flatten algorithm in graph which only follows a specific edge label. This is the most logical pattern and you can still separate
	/// the agent vertices and link them together separately if you want. With shared vertices, this is the best solution.
	/// </summary>
	class Links : public AgentsDefaults
	{
	protected:
		std::set<std::string> _BreedNames;
		//TODO: one graph per breed
		//std::unordered_map<std::string,Graph*> _BreedGraphs;
		//TODO: _graph needs to come out in favour of breeds graphs
		//NO, switched back to a single graph representation with edge labels holding the breed name which can be followed
		Graph* _graph; //this is the graph structure underlying the agent links, basically, the links are references to vertices
		std::vector<Link*> _myLinks; //list of created links - do you need this? it's a duplicate of the edges

	public:
		Links(void);
		~Links(void);

		Object3D* _pSceneRoot; //Root of links in scene graph. I'd like to get rid of this, but we need it to keep the 3D in step

		Link* CreateLink(std::string Breed, Agent* AStart, Agent* AEnd);
		bool DeleteLink(Link* L);

		void Create3D(Object3D* Parent); //create meshes for 3D by flattening graph and creating tube geometry
		void Recreate3D(); //calls Create3D, but removes existing geometry first and clears isDirty

		size_t NumLinks() { return _myLinks.size(); }
		bool isDirty; //true if the links have changed and the graphics need to be re-created on the next render


	};

}
