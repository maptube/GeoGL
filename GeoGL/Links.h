#pragma once

#include <vector>


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
		Graph* _graph; //this is the graph structure underlying the agent links, basically, the links are references to vertices
		std::vector<Link*> _myLinks; //list of created links - do you need this? it's a duplicate of the edges
	public:
		Links(void);
		~Links(void);

		Object3D* _pSceneRoot; //Root of links in scene graph. I'd like to get rid of this, but we need it to keep the 3D in step

		Link* Create(Agent* AStart,Agent* AEnd);

	};

}