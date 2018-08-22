
#include "Links.h"
#include "Link.h"
#include "Agent.h"
#include "graph.h"
#include "tubegeometry.h"
#include "agentobject.h"

#include <vector>

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
		delete _graph;

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
		std::set<std::string>::iterator it = _BreedNames.find(Breed);
		if (it==_BreedNames.end()) {
			//Breed doesn't exist, so create a new one
			_BreedNames.insert(Breed);
			//_BreedGraphs!!!
		}
		//Graph* G = nullptr;
		//std::unordered_map<std::string,Graph*>::iterator it = _BreedGraphs.find(Breed);
		//if (it==_BreedGraphs.end()) {
		//	//Breed doesn't exist, so create a new one and a new graph
		//	G = new Graph(true); //TODO: breeds need to be directed and undirected - this is directed
		//	_BreedGraphs.insert(std::pair<std::string,Graph*>(Breed,G));
		//}
		//else {
		//	G = it->second;
		//}
		Link* L = new Link();
		L->_BreedName=Breed;
		L->end1 = AStart;
		L->end2 = AEnd;
		//AStart->_OutLinks.push_back(L);
		//AEnd->_InLinks.push_back(L);
		//if start agent vertex isn't already created in the graph then make a new vertex for it
		if (AStart->_GVertex==NULL) {
			Vertex* V = _graph->AddVertex(); //_graph->AddVertex();
			V->_UserData = AStart;
			AStart->_GVertex = V; //note double linking
		}
		//same for the other agent vertex
		if (AEnd->_GVertex==NULL) {
			Vertex* V = _graph->AddVertex(); //_graph->AddVertex();
			V->_UserData = AEnd;
			AEnd->_GVertex = V; //note double linking
		}
		L->_GEdge = _graph->ConnectVertices(AStart->_GVertex,AEnd->_GVertex,"",0.0); //V1, V2, label, weight
		L->_GEdge->_UserData=L; //double linking of Link to edge and edge to Link
		L->_GEdge->_Label = Breed; //label the edge with the breed name so we can follow breeds using the label - user can always override this later
		_myLinks.push_back(L);

		isDirty = true; //flag the fact that the geometry will need to be changed to reflect this new link
		return L;
	}

	/// <summary>
	/// Delete a link from the graph.
	/// </summary>
	/// <returns> True on success </returns>
	bool Links::DeleteLink(Link* L)
	{
		bool Success = false;
		_graph->DeleteEdge(L->_GEdge); //delete the edge underlying this ABM link from the graph structure
		auto it = std::find(_myLinks.begin(),_myLinks.end(),L);
		if (it != _myLinks.end())
		{
			_myLinks.erase(it); //remove the link from the duplicate ABM master list of links
			delete L; //and free up the memory
			Success = true;
		}

		isDirty = true; //flag the fact that the geometry will need to be changed to reflect the absence of the link that we just deleted
		return Success;
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
		//for (std::unordered_map<std::string,Graph*>::iterator itBreed = _BreedGraphs.begin(); itBreed!=_BreedGraphs.end(); ++itBreed)
		for (std::set<std::string>::iterator itBreed = _BreedNames.begin(); itBreed!=_BreedNames.end(); ++itBreed)
		{
			//std::string BreedName = itBreed->first;
			std::string BreedName = *itBreed;
			//std::cout<<"Links::Create3D BreedName="<<BreedName<<std::endl;
			//if (BreedName!="D") continue; //hack!
			//Graph* G = itBreed->second;
			TubeGeometry* geom = new TubeGeometry();
			geom->Name="_LINKS_GEOM_"+BreedName; //might as well name the object in case we need to find it in the scene graph
			//flatten the graph into a set of continuous line segments without branches
			//std::vector<Vertex*> flattened = G->Flatten();
			std::vector<Vertex*> flattened = _graph->Flatten(BreedName); //flatten based on only following links of the given breed
			//now build path segments from the flattened node lists by joining the nodes to the vertex names in the graph
			std::vector<glm::vec3> spline;
			std::vector<glm::vec3> colours;
			//TODO: this is where you need to set a breed colour
			//NOTE: the colours array only contains one colour, which is used for all vertex colours in this breed's mesh. The colours are used
			//for each vertex in the tube geometry in order, returning to the start when you get to the end of the array. This allows for stripey
			//tubes if you want to go down that route. You only need one colour element in the array to set a solid colour though.
			Colour = GetDefaultColour(BreedName);
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
			//std::cout<<"Mesh: "<<geom->bounds.Centre().x<<","<<geom->bounds.Centre().y<<","<<geom->bounds.Centre().z<<std::endl;

			Parent->AddChild(geom);
		}
	}

	///<summary>
	///Called in response to the underlying network structure changing and the grahics buffers needing to ne recreated.
	///NOTE: you still need a shader to be set on Parent! Model::SetLinksShader
	///</summary>
	void Links::Recreate3D()
	{
		Object3D* Parent = _pSceneRoot; //root of network mesh in scene
		Parent->RemoveAllChildren(); //remove child meshes
		Create3D(Parent); //create all new ones
		isDirty = false; //clear dirty flag as we're all good now
	}



}
