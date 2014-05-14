
#include "Links.h"
#include "Link.h"
#include "Agent.h"
#include "graph.h"

namespace ABM {

	/// <summary>
	/// Constructor
	/// </summary>
	Links::Links(void)
	{
		_graph = new Graph(true);

		//initialise static pointer from Link to Links (this)
		//This is necessary so that a Link can create another Link, although Links needs to keep the list
		Link::_pParentLinks=this;
	}

	/// <summary>
	/// Destructor
	/// </summary>
	Links::~Links(void)
	{
		//free all the created link structures
		for (std::vector<Link*>::iterator it=_myLinks.begin(); it!=_myLinks.end(); ++it) {
			delete *it;
		}
	}

	/// <summary>
	/// Create a link between two agents in a single direction between AStart and AEnd
	/// </summary>
	Link* Links::Create(Agent* AStart, Agent* AEnd)
	{
		Link* L = new Link();
		L->end1 = AStart;
		L->end2 = AEnd;
		//AStart->_OutLinks.push_back(L);
		//AEnd->_InLinks.push_back(L);
		//if start agent vertex isn't already created in the graph then make a new vertex for it
		if (AStart->_GVertex==NULL) {
			Vertex* V = _graph->AddVertex();
			V->_UserData = AStart;
			AStart->_GVertex = V; //note double linking
		}
		//same for the other agent vertex
		if (AEnd->_GVertex==NULL) {
			Vertex* V = _graph->AddVertex();
			V->_UserData = AEnd;
			AEnd->_GVertex = V; //note double linking
		}
		L->_GEdge = _graph->ConnectVertices(AStart->_GVertex,AEnd->_GVertex,"",0.0); //V1, V2, label, weight
		L->_GEdge->_UserData=L; //double linking of Link to edge and edge to Link
		_myLinks.push_back(L);
		return L;
	}

}