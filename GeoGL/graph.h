#pragma once
//Basically a copy of the C# ANTS graph utility class, but modified for STL use, with everything made public as pointers

#include <string>
#include <list>
#include <unordered_map>
#include <vector>

//prototypes
class Edge;
class Vertex;
class Graph;

//implementation
class Edge {
//properties
public:
	bool _IsDirected;
	Vertex* _FromVertex;
	Vertex* _ToVertex;
	std::string _Label;
	float _Weight;
	void* _UserData;

//methods
public:
	/// <summary>
	/// Edge constructor
	/// </summary>
	/// <param name="IsDirected">Whether this edge is directed or undirected</param>
	/// <param name="FromVertex">The vertex that this edge is connecting from</param>
	/// <param name="ToVertex">The vertex that this edge is connecting to</param>
	/// <param name="Label">A label for the edge</param>
	/// <param name="Weight">A weight for the edge</param>
	Edge(bool IsDirected, Vertex* FromVertex, Vertex* ToVertex, std::string Label, float Weight)
	{
		_IsDirected = IsDirected;
		_FromVertex = FromVertex;
		_ToVertex = ToVertex;
		_Label = Label;
		_Weight = Weight;
	}
};


class Vertex {
//properties
public:
	std::list<Edge*> _OutEdges; //are we calling them edges, arcs or links?
	std::list<Edge*> _InEdges;
	int _VertexId; //unique vertex id number e.g. 0,1,2,3...
	std::string _Name; //add a name for a vertex e.g. tube station code
	bool _IsVisited; //used in flattening to mark a vertex as visited
	//private T _UserData; //user data tagged to this node
	//public Vertex<T> Root; //Root of vertex used for Kruskal
	//public float Rank; //Rank of vertex used for Kruskal
	//public float Distance; //Distance used in Dijkstra
	//public Vertex<T> Previous; //Previous vertex used in Dijkstra path
	void* _UserData; //allow user to tag vertex with his own data

//methods
public:
	Vertex(int Id)
	{
		_VertexId = Id;
	}
};

class Graph {
//properties
public:
	bool _IsDirected; //this is set in the constructor
	int _VertexIdCounter;
	std::unordered_map<int,Vertex*> _Vertices; //the key is the VertexId
	std::list<Edge*> _Edges; //master list of all graph edges

//methods
public:
	Graph(bool IsDirected)
	{
		_IsDirected = IsDirected;
		_VertexIdCounter=0;
		//_Vertices = new Dictionary<int,Vertex<T>>();
		//_Edges = new List<Edge<T>>();
	}

	~Graph()
	{
		//guess I had better delete all those pointers I created
		//_Vertices
		//_Edges
	}

	/// <summary>
	/// Add a new vertex to the graph and return it. This vertex has a unique id number, but no edges yet,
	/// so these will have to be added.
	/// </summary>
	/// <returns>The new vertex</returns>
	Vertex* AddVertex()
	{
		return AddVertex(_VertexIdCounter++);
	}
	
	/// <summary>
	/// Overloaded add to allow users to specify their own unique id number
	/// TODO: need exception for if unique id constraint is violated
	/// </summary>
	/// <param name="Id"></param>
	/// <returns>The new vertex</returns>
	Vertex* AddVertex(int Id)
	{
		//if (_Vertices.ContainsKey(Id)) throw duplicate id exception
		Vertex* V = new Vertex(Id);
		_Vertices.insert(std::make_pair/*<int,Vertex*>*/(V->_VertexId, V));
		return V;
	}
	
	/// <summary>
	/// Delete a vertex and any edges connected to it using the integer Vertex Id.
	/// Assumes that "VertexId" actually exists.
	/// TODO: I have no idea whether this actually works, or whether it leaks memory - UNTESTED!
	/// </summary>
	/// <param name="VertexId">The unique id of the vertex to delete</param>
	void DeleteVertex(int VertexId)
	{
		Vertex* V = _Vertices[VertexId];
		//delete all edges coming into this vertex
		for (std::list<Edge*>::iterator itE = V->_InEdges.begin(); itE!=V->_InEdges.end(); ++itE)
		{
			Edge *E = *itE;
			//make a new list of out edges for this vertex, but without the vertex we are in the process of removing
			//List<Edge<T>> NewOutEdges = new List<Edge<T>>();
			std::list<Edge*> NewOutEdges;
			for (std::list<Edge*>::iterator itOutE = (E->_FromVertex)->_OutEdges.begin(); itOutE!=(E->_FromVertex)->_OutEdges.end(); ++itOutE) 
			//foreach (Edge<T> OutE in E.FromVertex.OutEdges)
			{
				Edge *OutE=*itOutE;
				//if (OutE.ToVertex.VertexId != VertexId) NewOutEdges.Add(OutE);
				if ((*OutE->_ToVertex)._VertexId !=VertexId) NewOutEdges.push_back(OutE);
			}
			//E.FromVertex.OutEdges.Clear();
			//(E->_FromVertex->_OutEdges).clear();
			//E.FromVertex.OutEdges.InsertRange(0, NewOutEdges);
			//(E->_FromVertex->_OutEdges).insert(NewOutEdges);
			E->_FromVertex->_OutEdges=NewOutEdges;
		}
		
		//delete all edges going out of this vertex
		//foreach (Edge<T> E in V.OutEdges)
		for (std::list<Edge*>::iterator itE = V->_OutEdges.begin(); itE!=V->_OutEdges.end(); ++itE)
		{
			Edge *E = *itE;
			//make a new list of in edges for this vertex, but without the vertex we are in the process of removing
			//List<Edge<T>> NewInEdges = new List<Edge<T>>();
			std::list<Edge*> NewInEdges;
			for (std::list<Edge*>::iterator itInE = (E->_FromVertex)->_InEdges.begin(); itInE!=(E->_FromVertex)->_InEdges.end(); ++itInE)
			//foreach (Edge<T> InE in E.FromVertex.InEdges)
			{
				Edge *InE=*itInE;
				//if (InE.FromVertex.VertexId != VertexId) NewInEdges.Add(InE);
				if ((*InE->_FromVertex)._VertexId != VertexId) NewInEdges.push_back(InE);
			}
			//E.FromVertex.InEdges.Clear();
			//(E->_FromVertex->_InEdges).clear();
			//E.FromVertex.InEdges.InsertRange(0, NewInEdges);
			//(E->_FromVertex)->_InEdges->Insert(NewInEdges);
			E->_FromVertex->_InEdges=NewInEdges;
		}
		
		//delete copies from the main edge list either going in or out of this vertex
		//List<Edge<T>> NewEdges = new List<Edge<T>>();
		std::list<Edge*> NewEdges;
		//foreach (Edge<T> E in _Edges)
		for (std::list<Edge*>::iterator itE = _Edges.begin(); itE!=_Edges.end(); ++itE)
		{
			//if ((E.FromVertex.VertexId != VertexId) && (E.ToVertex.VertexId != VertexId))
			//	NewEdges.Add(E);
			Edge *E = *itE;
			if (((E->_FromVertex)->_VertexId!=VertexId)&&((E->_ToVertex)->_VertexId!=VertexId))
				NewEdges.push_back(E);
		}
		_Edges = NewEdges;
		
		//delete the vertex itself
		//_Vertices.Remove(VertexId);
		_Vertices.erase(VertexId);
	}
	
	/// <summary>
	/// Connect two vertices together with an edge. Handles directed or undirected correctly.
	/// </summary>
	/// <param name="VertexId1"></param>
	/// <param name="VertexId2"></param>
	/// <returns></returns>
	Edge* ConnectVertices(int VertexId1, int VertexId2, std::string Label, float Weight)
	{
		Vertex* VertexA = _Vertices[VertexId1];
		Vertex* VertexB = _Vertices[VertexId2];
		//TODO: probably need to check that A and B exist?
		//I'm creating a directed edge here, so it's up to the user if he wants to create the
		//opposite link as well
		return ConnectVertices(VertexA, VertexB, Label, Weight);
	}
	
	/// <summary>
	/// Overload to add a label and weight when connecting nodes
	/// </summary>
	/// <param name="VertexA">From Vertex</param>
	/// <param name="VertexB">To Vertex</param>
	/// <param name="Label">Label for the edge</param>
	/// <param name="weight">Weight for the edge</param>
	/// <returns></returns>
	Edge* ConnectVertices(Vertex* VertexA, Vertex* VertexB, std::string Label, float Weight)
	{
		//Create a directed or undirected edge based on the graph type
		Edge* E = new Edge(_IsDirected, VertexA, VertexB, Label, Weight);
		_Edges.push_back(E); //make sure you add it to the graph's master edge list
		//now add the in and out links to the two nodes so we can traverse it
		VertexA->_OutEdges.push_back(E);
		VertexB->_InEdges.push_back(E);
		//in the undirected case, the edge is marked as undirected and added as both an in and out
		//edge to both the A and B vertices. In graph traversal, this case must be checked for, as
		//the FromVertex and ToVertex can be traversed in either direction.
		if (!_IsDirected)
		{
			VertexA->_InEdges.push_back(E);
			VertexB->_OutEdges.push_back(E);
		}
		return E;
	}

	/// <summary>
	/// Delete an edge between two vertices. Opposite of connect.
	/// </summary>
	/// <param name="E">The edge to delete</param>
	/// <returns>True on success</returns>
	bool DeleteEdge(Edge* E)
	{
		Vertex* VertexA = E->_FromVertex;
		Vertex* VertexB = E->_ToVertex;
		_Edges.remove(E); //remove edge from the master list of edges
		//remove from VertexA out list and VertexB in list
		VertexA->_OutEdges.remove(E);
		VertexB->_InEdges.remove(E);
		//In the undirected case, you need to remove it the other way around too
		if (!_IsDirected)
		{
			VertexA->_InEdges.remove(E);
			VertexB->_OutEdges.remove(E);
		}
		delete E;
		return true; //OK, it never fails because it never checks that E really exists TODO: fix the error checking
	}

	/*
	* Flatten a graph into a list of vertices with contiguous segments connected as a path and a NULL meaning break path
	* and start a new one.
	* In other words, build a list of lines which we can use to draw the graph from.
	* TODO: you could add something to find the root of the tree, as this algorithm tends to start in the middle and work out, resulting
	* in a line that's broken into two parts. (see Knuth, finding root of tree in part 1).
	*/
	std::vector<Vertex*> Flatten()
	{
		std::vector<Vertex*> Result;

		//see Knuth... almost certainly
		//set all vertices to not visited
		int numverts = _Vertices.size();
		int numedges = _Edges.size();
		for (std::unordered_map<int,Vertex*>::iterator vIT=_Vertices.begin(); vIT!=_Vertices.end(); ++vIT) {
			vIT->second->_IsVisited=false;
		}

		//for all vertices not visited, FollowLinks until a dead end, setting isvisited as you go
		//This is only any use for disconnected parts of the network - FollowLinks must be recursive
		//Result.push_back(std::vector<Vertex*>());
		for (std::unordered_map<int,Vertex*>::iterator vIT=_Vertices.begin(); vIT!=_Vertices.end(); ++vIT) {
			Vertex* V = vIT->second;
			if (!V->_IsVisited) {
				FollowLinks(V,Result);
			}
		}

		return Result;
	}

	/**
	* For the recursive Flatten
	* Recursive depth first polyline follower to turn a minimum spanning tree into a list of polylines.
	* Follow links that haven't already been visited until you hit a dead end.
	*/
	void FollowLinks(Vertex* V,std::vector<Vertex*>& paths)
	{
		/*Vstart->_IsVisited=true; //mark this node as visited once
		//std::vector<Vertex*> path=paths.at(paths.size()-1); //current path being built

		//follow all the unvisited outlinks from Vstart on the current path to make a continuous depth first chain
		for (std::list<Edge*>::iterator eIT = Vstart->_OutEdges.begin(); eIT!=Vstart->_OutEdges.end(); ++eIT) {
			Vertex* V=(*eIT)->_ToVertex;
			if (!V->_IsVisited) {
				paths.push_back(Vstart); //push the start node
				FollowLinks(V,paths); //follow this link to the end of the chain
				//break the chain at this point and start a new path
				//std::vector<Vertex*> newpath;
				//path=newpath;
				//paths.push_back(newpath);
				paths.push_back(NULL);
			}
		}*/

		if ((V->_IsVisited)||(V->_OutEdges.size()==0)) {
			//guard case, we've hit a dead end if we come to an already visited node, or one with no out links
			//NOTE: this is not the only terminal node case
			V->_IsVisited=true;
			paths.push_back(V);
			paths.push_back(NULL);
			return; //probably not necessary as there is only one block
		}
		else {
			V->_IsVisited=true;
			for (std::list<Edge*>::iterator eIT = V->_OutEdges.begin(); eIT!=V->_OutEdges.end(); ++eIT) {
				paths.push_back(V);
				FollowLinks((*eIT)->_ToVertex,paths);
			}
		}

	}

	///////////////////////

	/*
	 * This is a copy of the Flatten functionality in Graph, but with an added complication of only following links belonging to
	 * the given BreedName. This is a version of flatten that is Link aware. It is necessary because the vertices (Agents) which the links
	 * connect are shared between all breed graphs, resulting in incorrect links otherwise.
	 *
	 * Flatten a graph into a list of vertices with contiguous segments connected as a path and a NULL meaning break path
	 * and start a new one.
	 * In other words, build a list of lines which we can use to draw the graph from.
	 * TODO: you could add something to find the root of the tree, as this algorithm tends to start in the middle and work out, resulting
	 * in a line that's broken into two parts. (see Knuth, finding root of tree in part 1).
	 */
	std::vector<Vertex*> Flatten(std::string Label)
	{
		std::vector<Vertex*> Result;

		//see Knuth... almost certainly
		//set all vertices to not visited
		int numverts = _Vertices.size();
		int numedges = _Edges.size();
		for (std::unordered_map<int,Vertex*>::iterator vIT=_Vertices.begin(); vIT!=_Vertices.end(); ++vIT) {
			vIT->second->_IsVisited=false;
		}

		//for all vertices not visited, FollowLinks until a dead end, setting isvisited as you go
		//This is only any use for disconnected parts of the network - FollowLinks must be recursive
		//Result.push_back(std::vector<Vertex*>());
		for (std::unordered_map<int,Vertex*>::iterator vIT=_Vertices.begin(); vIT!=_Vertices.end(); ++vIT) {
			Vertex* V = vIT->second;
			if (!V->_IsVisited) {
				FollowLinks(V,Label,Result);
			}
		}

		return Result;
	}

	/*
	 * For the recursive Flatten, following a label name
	 * Recursive depth first polyline follower to turn a minimum spanning tree into a list of polylines.
	 * Follow links that haven't already been visited until you hit a dead end.
	 */
	void FollowLinks(Vertex* V,std::string Label,std::vector<Vertex*>& paths)
	{
		if ((V->_IsVisited)||(V->_OutEdges.size()==0)) {
			//guard case, we've hit a dead end if we come to an already visited node, or one with no out links
			//NOTE: this is not the only terminal node case
			V->_IsVisited=true;
			paths.push_back(V);
			paths.push_back(NULL);
			return; //probably not necessary as there is only one block
		}
		else {
			V->_IsVisited=true;
			for (std::list<Edge*>::iterator eIT = V->_OutEdges.begin(); eIT!=V->_OutEdges.end(); ++eIT) {
				//testing edge label here so we only follow a specific labelled edge (e.g. ABM Link Breeds)
				if ((*eIT)->_Label==Label) {
					paths.push_back(V);
					FollowLinks((*eIT)->_ToVertex,Label,paths);
				}
			}
		}

	}
	////////////////////////////////////////////////////////////

};
