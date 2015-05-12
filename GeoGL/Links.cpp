
#include "Links.h"
#include "Link.h"
#include "Agent.h"
#include "graph.h"
#include "tubegeometry.h"

#include <vector>

namespace ABM {

	/// <summary>
	/// Constructor
	/// </summary>
	Links::Links(void)
	{
		//_graph = new Graph(true);

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
	/// Create a link between two agents in a single direction between AStart and AEnd with the given Breed name
	/// </summary>
	Link* Links::CreateLink(std::string Breed, Agent* AStart, Agent* AEnd)
	{
		//std::set<std::string>::iterator it = _BreedNames.find(Breed);
		//if (it==_BreedNames.end()) {
		//	//Breed doesn't exist, so create a new one
		//	_BreedNames.insert(Breed);
		//	//_BreedGraphs!!!
		//}
		Graph* G = nullptr;
		std::unordered_map<std::string,Graph*>::iterator it = _BreedGraphs.find(Breed);
		if (it==_BreedGraphs.end()) {
			//Breed doesn't exist, so create a new one and a new graph
			G = new Graph(true); //TODO: breeds need to be directed and undirected - this is directed
			_BreedGraphs.insert(std::pair<std::string,Graph*>(Breed,G));
		}
		else {
			G = it->second;
		}
		Link* L = new Link();
		L->_BreedName=Breed;
		L->end1 = AStart;
		L->end2 = AEnd;
		//AStart->_OutLinks.push_back(L);
		//AEnd->_InLinks.push_back(L);
		//if start agent vertex isn't already created in the graph then make a new vertex for it
		if (AStart->_GVertex==NULL) {
			Vertex* V = G->AddVertex(); //_graph->AddVertex();
			V->_UserData = AStart;
			AStart->_GVertex = V; //note double linking
		}
		//same for the other agent vertex
		if (AEnd->_GVertex==NULL) {
			Vertex* V = G->AddVertex(); //_graph->AddVertex();
			V->_UserData = AEnd;
			AEnd->_GVertex = V; //note double linking
		}
		L->_GEdge = G->ConnectVertices(AStart->_GVertex,AEnd->_GVertex,"",0.0); //V1, V2, label, weight
		L->_GEdge->_UserData=L; //double linking of Link to edge and edge to Link
		_myLinks.push_back(L);
		return L;
	}

	/// <summary>
	/// Create the scene object for this links data.
	/// This is basically a copy of the NetGraphGeometry constructor, but using the UserData on the graph not for the Agent pointer and its position,
	/// rather than joining a string key, position tuple with named graph vertices. NetGraphGeometry builds on TubeGeometry, which has most of the
	/// functionality, so we just use that. This keeps NetGraphGeometry as a specialised class just for graph geometry if you have a vertex/position
	/// lookup table.
	/// TODO: need to separate breeds into different geometries - this does everything as one mesh
	/// TODO: colour and size etc are hard coded, these need to come from breed properties
	/// </summary>
	/// <param name="Parent">Parent object in the scene graph</param>
	void Links::Create3D(Object3D* Parent)
	{
		//TODO: colour needs to come from a breed specific setting
		//find its colour
		glm::vec3 Colour = glm::vec3(1.0,1.0,1.0);
		//radius and segs also need to be breed specific
		const float Radius = 10.0f; //0.00025f;
		//TODO: need this configurable by the model (and Radius)
		const int NumSegments = 4; //OK, 4 is square, but there are 50,000 of them!  //20;
		//now build the geometry


		//one alternative is to create a custom network graph geometry that understands links, but it's probably better to keep that
		//as specific to a plain graph and add some similar code here which is specialised to create a links based tube geometry
		for (std::unordered_map<std::string,Graph*>::iterator itBreed = _BreedGraphs.begin(); itBreed!=_BreedGraphs.end(); ++itBreed)
		{
			std::string BreedName = itBreed->first;
			Graph* G = itBreed->second;
			TubeGeometry* geom = new TubeGeometry();
			geom->Name="_LINKS_GEOM_"+BreedName; //might as well name the object in case we need to find it in the scene graph
			//flatten the graph into a set of continuous line segments without branches
			std::vector<Vertex*> flattened = G->Flatten();
			//now build path segments from the flattened node lists by joining the nodes to the vertex names in the graph
			std::vector<glm::vec3> spline;
			std::vector<glm::vec3> colours;
			colours.push_back(Colour);
			for (std::vector<Vertex*>::iterator it=flattened.begin(); it!=flattened.end(); ++it) {
				Vertex* V = *it;
				if (V==NULL) {
					geom->AddSpline(spline,colours); //check number of points?
					spline.clear();
				}
				else {
					Agent* A=reinterpret_cast<Agent*>(V->_UserData);
					glm::dvec3 pos = A->GetXYZ();
					//std::cout<<"Links::Create3D "<<pos.x<<" "<<pos.y<<" "<<pos.z<<std::endl;
					spline.push_back(glm::vec3(pos)); //TODO: casting the double vector to a single is losing precision
				}
			}
			//this assumes that the flattened list always ends with a null
			geom->GenerateMesh(Radius,NumSegments);

			Parent->AddChild(geom);
		}
	}

}
