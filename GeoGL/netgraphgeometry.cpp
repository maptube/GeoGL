
#include "netgraphgeometry.h"
#include <sstream>

using namespace std;

/**
* Build geometry for a network graph.
* TODO: maybe the unordered map should be an attribute table?
* Also, does this deserve a special class as it's only a TubeGeometry made from a Graph joined with vertex positions?
* @param pGraph
* @param VertexPositions
* @param Radius The radius of the tube.
* @param NumSegments Number of segments around a ring of the tube.
* @param Colour	The colour of this graph geometry. At the moment individual graphs can only be a single colour.
*/
NetGraphGeometry::NetGraphGeometry(Graph* pGraph, std::unordered_map<std::string,struct GraphNameXYZ>* VertexPositions,float Radius, unsigned int NumSegments, glm::vec3 Colour)
{
	//flatten the graph into a set of continuous line segments without branches
	vector<Vertex*> flattened = pGraph->Flatten();
	//now build path segments from the flattened node lists by joining the nodes to the vertex names in the graph
	vector<glm::vec3> spline;
	vector<glm::vec3> colours;
	//colours.push_back(glm::vec3(1.0,0,0));
	//colours.push_back(glm::vec3(0,0,1.0));
	colours.push_back(Colour);
	for (vector<Vertex*>::iterator it=flattened.begin(); it!=flattened.end(); ++it) {
		Vertex* V = *it;
		//debug
		//std::ostringstream ss;
		//if (*it==NULL) ss<<"BREAK"<<endl;
		//else ss<<"Path: "<<(*it)->_Name<<endl;
		//OutputDebugStringA(ss.str().c_str());
		//end debug
		if (V==NULL) {
			AddSpline(spline,colours); //check number of points?
			spline.clear();
		}
		else {
			struct GraphNameXYZ pos;
			pos = VertexPositions->find(V->_Name)->second;
			spline.push_back(pos.P);
		}
	}
	//this assumes that the flattened list always ends with a null
	GenerateMesh(Radius,NumSegments);
}

NetGraphGeometry::~NetGraphGeometry(void)
{
}
