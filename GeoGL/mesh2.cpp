
#include "mesh2.h"
#include <math.h>
#include <algorithm>
#include <iterator>
#include "DebugUtils.h"

#include "gengine/graphicscontext.h"
#include "gengine/scenedataobject.h"
#include "gengine/ogldevice.h"
#include "gengine/glbuffertypes.h"
#include "gengine/vertexbuffer.h"
#include "gengine/indexbuffer.h"
#include "gengine/vertexformat.h"
#include "gengine/vertexdata.h"
#include "gengine/shader.h"
#include "gengine/renderstate.h"
#include "gengine/primitivetypes.h"

using namespace std;

using namespace gengine;

///////MESH2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/// <summary>
/// DEBUG: print out points to console NOT cout
/// </summary>
void printPoints(vector<struct VertexColour> vertices, glm::vec3 P) {
	//debug print out points
	DebugUtils::WriteString("Insert point: "); DebugUtils::WriteFloat(P.x); DebugUtils::WriteLine();
	DebugUtils::WriteString("X: ");
	for (vector<struct VertexColour>::iterator it=vertices.begin(); it!=vertices.end(); ++it) {
		DebugUtils::WriteFloat((*it).P.x);
		DebugUtils::WriteString(" ");
	}
	DebugUtils::WriteLine();
}

/// <summary>
/// DEBUG
/// <summary>
void checkPointOrdering(vector<struct VertexColour> vertices) {
	//debug check points in the vertices vector are in the correct x ordering
	float x = -10000000; //hack max_float!
	for (vector<struct VertexColour>::iterator it=vertices.begin(); it!=vertices.end(); ++it) {
		if ((*it).P.x<x) {
			float delta = (*it).P.x - x;
			DebugUtils::WriteString("Error in x ");
			DebugUtils::WriteFloat((*it).P.x);
			DebugUtils::WriteString(" < ");
			DebugUtils::WriteFloat(x);
			DebugUtils::WriteString(" delta=");
			DebugUtils::WriteFloat(delta);
			DebugUtils::WriteLine();
		}
		x=(*it).P.x;
	}
}

/// <summary>
/// Constructor. Create an Object3D which has geometry. All geometric primitives derive from this class as it wraps the OpenGL buffer creation.
/// </summary>
Mesh2::Mesh2(void)
{
	epsilon = 0.0000001f;

	//set default vertex format to have position and colour indices
	_VertexFormat = PositionColour;
	_HasIndexBuffer = true; //default is to create an index buffer unless set to false (maybe tri-stripping or tiled earth?)
	_NumVertices=0;

	//vertices.reserve(1024);
	//faces.reserve(1024);

	renderState = new RenderState();
	drawObject._rs=renderState; //TODO: find a more elegant way of doing this
}

/// <summary>
/// Destructor
/// </summary>
Mesh2::~Mesh2(void)
{
	//cout<<"Destructor Mesh2"<<endl;
	//todo: free the non-fallback buffers as well
	//if (mem_vertexbuffer!=NULL) delete mem_vertexbuffer;
	//if (mem_colourbuffer!=NULL) delete mem_colourbuffer;
	//if (mem_indexbuffer!=NULL) delete mem_indexbuffer;

	FreeBuffers();
	delete renderState;
}

//Pattern: add vertices, then add faces OR just add faces?
//The best format for doing CSG might be to push sphere/cylinder/cube geoms to a mesh and manipluate in this class rather than them all deriving from mesh?

/// <summary>
/// return Manhattan distance between two vectors, either points or colours
/// </summary>
/// <param name="V1">First vector</param>
/// <param name="V2">Second vector</param>
/// <returns>The Manhattan distance between vectors V1 and V2</returns>
float Mesh2::ManhattanDist(glm::vec3 V1, glm::vec3 V2) {
	return abs(V1.x-V2.x)+abs(V1.y-V2.y)+abs(V1.z-V2.z);
}

//overload for texture coords which are 2D
float Mesh2::ManhattanDist(glm::vec2 V1, glm::vec2 V2) {
	return abs(V1.x-V2.x)+abs(V1.y-V2.y);
}

/// <summary>
/// Get bounding box for this object and all its children, but only for the geometry that you can actually see.
/// A pure Object3D returns nothing as you can't see it. A mesh and its subclasses return the bounds of their contained face vertices.
/// This is a mesh, so we return the geometry bounds.
/// TODO: you might want to check that the offsets and matrix rotations are right?
/// </summary>
/// <returns>The bounding box of any objects and children containing visible geometry</returns>
BBox Mesh2::GetGeometryBounds() {
	BBox box;
	box.Union(bounds); //initialise to parent
	for (vector<Object3D*>::iterator it=Children.begin(); it!=Children.end(); ++it) {
		Object3D* o3d = *it;
		//take child box, transform by model matrix and add on to this object's box
		BBox childBox = o3d->GetGeometryBounds(); //get bounds for child and all its children recursively
		childBox.min = glm::vec3( modelMatrix * glm::vec4(childBox.min,1) );
		childBox.max = glm::vec3( modelMatrix * glm::vec4(childBox.max,1) );
		box.Union(childBox); //this should return the bounds for the object and all its children
	}
	return box;
}

/// <summary>
/// Add a new point, but don't worry about duplicates, just push it onto the list
/// <summary>
/// <returns>The index of the point just added. This will always be in sequence i.e. 0,1,2...</returns>
int Mesh2::AddVertexRaw(glm::vec3 P) {
	VertexOnly item;
	item.P=P;
	item.Index=_NumVertices;
	++_NumVertices;
	vertices_V.push_back(item);
	bounds.ExpandToIncludePoint(item.P);
	return item.Index;
}

//copy of add vertex using only position
int Mesh2::AddVertex(glm::vec3 P) {
	//assumes x sorted vertices list
	VertexOnly item;
	item.P=P;
	item.P.x-=epsilon; //this is the lower bound on x
	vector<struct VertexOnly>::iterator it = std::lower_bound(vertices_V.begin(), vertices_V.end(), item);
	vector<struct VertexOnly>::iterator insertPoint = it;
	while ((it!=vertices_V.end())&&((*it).P.x<=P.x+epsilon)) {
		struct VertexOnly V=*it;
		glm::vec3 P2=V.P;
		float d = ManhattanDist(P,P2); //Manhattan dist between vertex pos
		if (d<epsilon) {
			return V.Index; //point at this position with this colour (<epsilon) is already in the list, so exit early
		}
		if (P.x>=P2.x) {
			insertPoint=it; //keep track of where we might need to insert a new x value
		}
		++it;
	}
	//if we've got to this point, then we need to add a new point, and as we bailed out when *it.P.x>P.x, then *it is the position to insert
	//NO - you need to go to x+epsilon as you can have x1=x2+e, y1=y2 and z1=z2 for a match.
	//The problem then is that the x ordering is wrong! The solution is to use the insertPoint that we've set to the correct position while iterating earlier.
	//InsertPoint holds last iteration of place while P.x>=P2.x holds, which is the correct insert point
	item.Index=_NumVertices; //vertices_V.size(); //TODO: check how efficient count is
	++_NumVertices;
	item.P=P; //put x back to its real value

	vertices_V.insert(insertPoint,item);
	bounds.ExpandToIncludePoint(item.P);
	return item.Index;
}

/// <summary>
/// Add a vertex with a colour to the list of vertices, but only if it doesn't already exist.
/// Returns index of the point added or existing
/// </summary>
/// <param name="P">Vertex position vector</param>
/// <param name="Colour">Colour vector</param>
/// <returns>The index of the point added, or the index of an existing point if it could weld them</returns>
int Mesh2::AddVertex(glm::vec3 P, glm::vec3 Colour) {
//http://www.asawicki.info/news_1352_the_beauty_of_sorted_arrays.html
//item = Item(1, 0, std::string());
//ItemVector::iterator findIt = std::lower_bound(vec.begin(), vec.end(), item);
//if (findIt != vec.end() && findIt->Id == item.Id)
//{
//  size_t foundIndex = std::distance(vec.begin(), findIt);
//}
//else
//{
//  // Not found.
//}

	//guard case for a different vertex format to this one (higher level objects create with all vertex data present and this converts down)
	if (_VertexFormat==Position) return AddVertex(P);
	//otherwise it MUST be Position information only


//assumes x sorted vertices list
	VertexColour item;
	item.P=P;
	item.P.x-=epsilon; //this is the lower bound on x
	item.RGB=Colour;
	vector<struct VertexColour>::iterator it = std::lower_bound(vertices_VC.begin(), vertices_VC.end(), item);
	vector<struct VertexColour>::iterator insertPoint = it;
	while ((it!=vertices_VC.end())&&((*it).P.x<=P.x+epsilon)) {
		struct VertexColour VC=*it;
		glm::vec3 P2=VC.P, C2=VC.RGB;
		float d = ManhattanDist(P,P2) + ManhattanDist(Colour,C2); //Manhattan dist between vertex pos and colour (could separate?)
		if (d<epsilon) {
			//DebugUtils::WriteString("return early"); DebugUtils::WriteLine();
			//printPoints(vertices,P);
			return VC.Index; //point at this position with this colour (<epsilon) is already in the list, so exit early
		}
		if (P.x>=P2.x) {
			insertPoint=it; //keep track of where we might need to insert a new x value
			//DebugUtils::WriteString("CMP: "); DebugUtils::WriteFloat(P.x);
			//DebugUtils::WriteString(">="); DebugUtils::WriteFloat(P2.x);
			//DebugUtils::WriteLine();
		}
		++it;
	}
	//if we've got to this point, then we need to add a new point, and as we bailed out when *it.P.x>P.x, then *it is the position to insert
	//NO - you need to go to x+epsilon as you can have x1=x2+e, y1=y2 and z1=z2 for a match.
	//The problem then is that the x ordering is wrong! The solution is to use the insertPoint that we've set to the correct position while iterating earlier.
	//InsertPoint holds last iteration of place while P.x>=P2.x holds, which is the correct insert point
	item.Index=_NumVertices; //vertices_VC.size(); //TODO: check how efficient count is
	++_NumVertices;
	item.P=P; //put x back to its real value
	
	vertices_VC.insert(insertPoint,item);
	//int insertPos = std::distance(insertPoint,vertices.begin());
	//DebugUtils::WriteString("Insert at: "); DebugUtils::WriteInt(insertPos); DebugUtils::WriteLine();
	//printPoints(vertices,P);
	//std::sort(vertices.begin(),vertices.end()); //hack!
	bounds.ExpandToIncludePoint(item.P);
	return item.Index;


//original non ordered version
/*	int idx=0;
	for (vector<struct VertexColour>::iterator it = vertices.begin(); it!=vertices.end(); ++it, ++idx) {
		struct VertexColour VC=*it;
		glm::vec3 P2=VC.P, C2=VC.RGB;
		float d = ManhattanDist(P,P2) + ManhattanDist(Colour,C2); //Manhattan dist between vertex pos and colour (could separate?)
		if (d<epsilon) return idx; //point at this position with this colour (<epsilon) is already in the list, so exit early
	}
	//if we've got to this point, then we need to add a new point
	idx=vertices.size();
	VertexColour VC;
	VC.P = P;
	VC.RGB = Colour;
	vertices.push_back(VC);
	bounds.ExpandToIncludePoint(VC.P);
	return idx;
*/
}

//copy of add vertex using only position, colour, normal
int Mesh2::AddVertex(glm::vec3 P, glm::vec3 Colour, glm::vec3 N) {
	//TODO: need guard cases here
	//guard case for a different vertex format to this one (higher level objects create with all vertex data present and this converts down)
	if (_VertexFormat==Position) return AddVertex(P);
	else if (_VertexFormat==PositionColour) return AddVertex(P,Colour);
	//otherwise it MUST be PositionColourNormal


	//assumes x sorted vertices list
	VertexColourNormal item;
	item.P=P;
	item.P.x-=epsilon; //this is the lower bound on x
	item.RGB=Colour;
	item.N=N;
	vector<struct VertexColourNormal>::iterator it = std::lower_bound(vertices_VCN.begin(), vertices_VCN.end(), item);
	vector<struct VertexColourNormal>::iterator insertPoint = it;
	while ((it!=vertices_VCN.end())&&((*it).P.x<=P.x+epsilon)) {
		struct VertexColourNormal VCN=*it;
		glm::vec3 P2=VCN.P, C2=VCN.RGB, N2=VCN.N;
		float d = ManhattanDist(P,P2) + ManhattanDist(Colour,C2) +
				ManhattanDist(N,N2); //Manhattan dist between vertex pos and colour (could separate?)
		if (d<epsilon) {
			return VCN.Index; //point at this position with this colour (<epsilon) is already in the list, so exit early
		}
		if (P.x>=P2.x) {
			insertPoint=it; //keep track of where we might need to insert a new x value
		}
		++it;
	}
	//if we've got to this point, then we need to add a new point, and as we bailed out when *it.P.x>P.x, then *it is the position to insert
	//NO - you need to go to x+epsilon as you can have x1=x2+e, y1=y2 and z1=z2 for a match.
	//The problem then is that the x ordering is wrong! The solution is to use the insertPoint that we've set to the correct position while iterating earlier.
	//InsertPoint holds last iteration of place while P.x>=P2.x holds, which is the correct insert point
	item.Index=_NumVertices; //vertices_VCN.size(); //TODO: check how efficient count is
	++_NumVertices;
	item.P=P; //put x back to its real value

	vertices_VCN.insert(insertPoint,item);
	bounds.ExpandToIncludePoint(item.P);
	return item.Index;
}

//copy of vertexcolour version but with addition of UV and N
int Mesh2::AddVertex(glm::vec3 P, glm::vec3 Colour, glm::vec2 UV, glm::vec3 N) {
	//TODO: need guard cases here
	//guard case for a different vertex format to this one (higher level objects create with all vertex data present and this converts down)
	if (_VertexFormat==Position) return AddVertex(P);
	else if (_VertexFormat==PositionColour) return AddVertex(P,Colour);
	else if (_VertexFormat==PositionColourNormal) return AddVertex(P,Colour,N);
	//otherwise it MUST be PositionColourTextureNormal

	//assumes x sorted vertices list
	VertexColourTextureNormal item;
	item.P=P;
	item.P.x-=epsilon; //this is the lower bound on x
	item.RGB=Colour;
	item.UV=UV;
	item.N=N;
	vector<struct VertexColourTextureNormal>::iterator it = std::lower_bound(vertices_VCTN.begin(), vertices_VCTN.end(), item);
	vector<struct VertexColourTextureNormal>::iterator insertPoint = it;
	while ((it!=vertices_VCTN.end())&&((*it).P.x<=P.x+epsilon)) {
		struct VertexColourTextureNormal VCTN=*it;
		glm::vec3 P2=VCTN.P, C2=VCTN.RGB, N2=VCTN.N;
		glm::vec2 UV2=VCTN.UV;
		float d = ManhattanDist(P,P2) + ManhattanDist(Colour,C2) +
				ManhattanDist(UV,UV2) + ManhattanDist(N,N2); //Manhattan dist between vertex pos and colour (could separate?)
		if (d<epsilon) {
			return VCTN.Index; //point at this position with this colour (<epsilon) is already in the list, so exit early
		}
		if (P.x>=P2.x) {
			insertPoint=it; //keep track of where we might need to insert a new x value
		}
		++it;
	}
	//if we've got to this point, then we need to add a new point, and as we bailed out when *it.P.x>P.x, then *it is the position to insert
	//NO - you need to go to x+epsilon as you can have x1=x2+e, y1=y2 and z1=z2 for a match.
	//The problem then is that the x ordering is wrong! The solution is to use the insertPoint that we've set to the correct position while iterating earlier.
	//InsertPoint holds last iteration of place while P.x>=P2.x holds, which is the correct insert point
	item.Index=_NumVertices; //vertices_VCTN.size(); //TODO: check how efficient count is
	++_NumVertices;
	item.P=P; //put x back to its real value

	vertices_VCTN.insert(insertPoint,item);
	bounds.ExpandToIncludePoint(item.P);
	return item.Index;
}

/// <summary>
/// Scale all vertices by a vector (Sx, Sy, Sz). Useful for objects which are created to unit size and need rescaling.
/// </summary>
/// <param name="Sx">Scale factor for X axis</param>
/// <param name="Sy">Scale factor for Y axis</param>
/// <param name="Sz">Scale factor for Z axis</param>
void Mesh2::ScaleVertices(double Sx, double Sy, double Sz) {
	//An alternative way of doing this might be just to iterate over all four vectors in turn regardless of _VertexFormat.
	//The unused ones are zero length after all.
	vector<struct VertexOnly>::iterator it_V;
	vector<struct VertexColour>::iterator it_VC;
	vector<struct VertexColourTexture>::iterator it_VCT;
	vector<struct VertexColourNormal>::iterator it_VCN;
	vector<struct VertexColourTextureNormal>::iterator it_VCTN;
	switch (_VertexFormat) {
	case Position:
		for (it_V = vertices_V.begin(); it_V!=vertices_V.end(); ++it_V) {
			(*it_V).P.x*=(float)Sx;
			(*it_V).P.y*=(float)Sy;
			(*it_V).P.z*=(float)Sz;
		}
		break;
	case PositionColour:
	case InterleavedPositionColour:
		for (it_VC = vertices_VC.begin(); it_VC!=vertices_VC.end(); ++it_VC) {
			//this is horrible, how about std::algorithms?
			//also, the vertex buffers are floats because of the GPU limitation, while everywhere else they're doubles
			(*it_VC).P.x*=(float)Sx;
			(*it_VC).P.y*=(float)Sy;
			(*it_VC).P.z*=(float)Sz;
		}
		break;
	case PositionColourTexture:
	case InterleavedPositionColourTexture:
		for (it_VCT = vertices_VCT.begin(); it_VCT!=vertices_VCT.end(); ++it_VCT) {
			(*it_VCT).P.x*=(float)Sx;
			(*it_VCT).P.y*=(float)Sy;
			(*it_VCT).P.z*=(float)Sz;
		}
		break;
	case PositionColourNormal:
	case InterleavedPositionColourNormal:
		for (it_VCN = vertices_VCN.begin(); it_VCN!=vertices_VCN.end(); ++it_VCN) {
			(*it_VCN).P.x*=(float)Sx;
			(*it_VCN).P.y*=(float)Sy;
			(*it_VCN).P.z*=(float)Sz;
		}
		break;
	case PositionColourTextureNormal:
	case InterleavedPositionColourTextureNormal:
		for (it_VCTN = vertices_VCTN.begin(); it_VCTN!=vertices_VCTN.end(); ++it_VCTN) {
			(*it_VCTN).P.x*=(float)Sx;
			(*it_VCTN).P.y*=(float)Sy;
			(*it_VCTN).P.z*=(float)Sz;
		}
		break;
	}
}

/// <summary>
/// Raw version of add face where we push the index numbers directly
/// <summary>
void Mesh2::AddFace(int Va, int Vb, int Vc) {
	faces.push_back(Va);
	faces.push_back(Vb);
	faces.push_back(Vc);
}

/// <summary>
/// overload for vertex position only
/// </summary>
void Mesh2::AddFace(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3) {
	//let AddVertex do all the work, it will either add a new vertex, or return an existing index
	int i_P1 = AddVertex(P1);
	int i_P2 = AddVertex(P2);
	int i_P3 = AddVertex(P3);

	//and create a face
	faces.push_back(i_P1); faces.push_back(i_P2); faces.push_back(i_P3);
}

/// <summary>
/// Add a face. Doesn't check anticlockwise ordering. Uses add vertex to weld vertices and avoid creating new ones if not necessary.
/// <summary>
/// <param name="P1">Position vector of vertex 1</param>
/// <param name="P2">Position vector of vertex 2</param>
/// <param name="P3">Position vector of vertex 3</param>
/// <param name="C1">Colour vector of vertex 1</param>
/// <param name="C2">Colour vector of vertex 1</param>
/// <param name="C3">Colour vector of vertex 1</param>
void Mesh2::AddFace(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3) {
	//let AddVertex do all the work, it will either add a new vertex, or return an existing index
	int i_P1 = AddVertex(P1,Colour1);
	int i_P2 = AddVertex(P2,Colour2);
	int i_P3 = AddVertex(P3,Colour3);

	//and create a face
	faces.push_back(i_P1); faces.push_back(i_P2); faces.push_back(i_P3);

	//debug
	//DebugUtils::WriteString("FACE"); DebugUtils::WriteLine();
	//DebugUtils::WriteString("X: ");
	//for (vector<struct VertexColour>::iterator it=vertices.begin(); it!=vertices.end(); ++it) {
	//	DebugUtils::WriteFloat((*it).P.x);
	//	DebugUtils::WriteString(" ");
	//}
	//DebugUtils::WriteLine();
	//printPoints(vertices,P1);
	//float x = -10000000; //hack max_float!
	//for (vector<struct VertexColour>::iterator it=vertices.begin(); it!=vertices.end(); ++it) {
	//	if ((*it).P.x<x) {
	//		float delta = (*it).P.x - x;
	//		DebugUtils::WriteString("Error in x ");
	//		DebugUtils::WriteFloat((*it).P.x);
	//		DebugUtils::WriteString(" < ");
	//		DebugUtils::WriteFloat(x);
	//		DebugUtils::WriteString(" delta=");
	//		DebugUtils::WriteFloat(delta);
	//		DebugUtils::WriteLine();
	//	}
	//	x=(*it).P.x;
	//}
	//checkPointOrdering(vertices);
	//end debug
}

/// <summary>
/// overload for normals
/// </summary>
void Mesh2::AddFace(
		glm::vec3 P1, glm::vec3 P2, glm::vec3 P3,
		glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3,
		glm::vec3 N1, glm::vec3 N2, glm::vec3 N3) {
	//let AddVertex do all the work, it will either add a new vertex, or return an existing index
	int i_P1 = AddVertex(P1,Colour1,N1);
	int i_P2 = AddVertex(P2,Colour2,N2);
	int i_P3 = AddVertex(P3,Colour3,N3);

	//and create a face
	faces.push_back(i_P1); faces.push_back(i_P2); faces.push_back(i_P3);
}

/// <summary>
/// overload for textures and normals
/// </summary>
void Mesh2::AddFace(
		glm::vec3 P1, glm::vec3 P2, glm::vec3 P3,
		glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3,
		glm::vec2 UV1, glm::vec2 UV2, glm::vec2 UV3,
		glm::vec3 N1, glm::vec3 N2, glm::vec3 N3) {
	//let AddVertex do all the work, it will either add a new vertex, or return an existing index
	int i_P1 = AddVertex(P1,Colour1,UV1,N1);
	int i_P2 = AddVertex(P2,Colour2,UV2,N2);
	int i_P3 = AddVertex(P3,Colour3,UV3,N3);

	//and create a face
	faces.push_back(i_P1); faces.push_back(i_P2); faces.push_back(i_P3);
}

/// <summary>
/// Change the vertex colour buffer data.
/// This is a slightly heavyweight way of doing things - chuck all the buffers and re-create them - but then again changing the colour is going to mess a lot of things up.
/// You could write a re-create buffers funtion which just applies new data to existing buffers without having to create them again, probably not much speedup though?
/// </summary>
/// <param name="new_colour"></param>
void Mesh2::SetColour(glm::vec3 new_colour)
{
//TODO: need to fix this!!!!!!!!!!!!!!!!!!!
	//basically, what I'm going to do here is to change all the colours assigned to the faces and then re-create the buffers
	/*for (vector<struct VertexColour>::iterator it = vertices.begin(); it!=vertices.end(); ++it) {
		it->RGB=new_colour;
	}
	FreeBuffers();
	CreateBuffers();*/

	//new code - NOTE: calling this before CreateBuffers has been called would be bad.
	float* buf_colours = vertexData->_vb[1]->CopyToMemory(); //don't forget to delete this later
	//OK, so basically we have a list of colours for this object - technically, I could have just set a new lot rather than getting the old ones, but might want a search and replace function later?
	unsigned int SizeBytes = vertexData->_vb[1]->_SizeBytes;
	unsigned int SizeFloats = SizeBytes>>2; //divide by 4
	for (unsigned int i=0; i<SizeFloats; i+=3) {
		buf_colours[i]=new_colour.x;
		buf_colours[i+1]=new_colour.y;
		buf_colours[i+2]=new_colour.z;
	}
	vertexData->_vb[1]->CopyFromMemory(buf_colours);
	delete [] buf_colours;
}


//need to convert mesh to object3d vertex and index buffers
/// <summary>
/// Take the vertices and faces currently stored and make Object3D buffers for OpenGL.
/// Assumes you have been calling AddFace() with the data first.
/// </summary>
void Mesh2::CreateBuffers() {

	//these should be constants? From the shader...
	//string AttributeName("in_Position");

	//bool hasColourBuffer = (_VertexFormat&0x2); //this should be a separate colour buffer ONLY
	bool isInterleaved = (_VertexFormat&0x10);

	//unsigned int NumVertices = vertices.size();
	unsigned int NumFaces = faces.size(); //this is the number of face indices i.e. 3 * actual number of faces

	//graphics card buffer vertices, colours, indexes
	
	VertexBuffer* vb=nullptr; //vertices
	VertexBuffer* vc=nullptr; //colours
	VertexBuffer* vt=nullptr; //textures
	VertexBuffer* vn=nullptr; //normals
	//create internal memory blocks in format suitable for copying to opengl vertex and index buffers
	float* buf_vertices, *buf_colours, *buf_textures, *buf_normals; //internal memory blocks for copying data
	//TODO: the sizes are really GL sizes, but we shouldn't be using GL types directly outside gengine - how to best do this?
	if (!isInterleaved) {
		vb = OGLDevice::CreateVertexBuffer("in_Position",ArrayBuffer,StaticDraw,_NumVertices*3*sizeof(float));
		buf_vertices = new float[_NumVertices*3]; //technically it's a GLfloat
		if (_VertexFormat&VertexFormatColourBit) {
			vc = OGLDevice::CreateVertexBuffer("in_Color",ArrayBuffer,StaticDraw,_NumVertices*3*sizeof(float));
			buf_colours = new float[_NumVertices*3]; //GLfloat
		}
		if (_VertexFormat&VertexFormatTextureBit) {
			vt = OGLDevice::CreateVertexBuffer("in_Texture",ArrayBuffer,StaticDraw,_NumVertices*2*sizeof(float));
			buf_textures = new float[_NumVertices*2]; //GLfloat
		}
		if (_VertexFormat&VertexFormatNormalBit) {
			vn = OGLDevice::CreateVertexBuffer("in_Normal",ArrayBuffer,StaticDraw,_NumVertices*3*sizeof(float));
			buf_normals = new float[_NumVertices*3]; //GLfloat
		}
	}
	else {
		//TODO:
		//interleaved
		//add up the size of all the components and create one buffer of that size times _NumVertices
	}

	IndexBuffer* ib;
	unsigned int* buf_indices;
	if (_HasIndexBuffer) {
		ib=OGLDevice::CreateIndexBuffer(ElementArrayBuffer,StaticDraw,NumFaces*sizeof(unsigned int)); //note that NumFaces is already *3 (see def above)
		buf_indices = new unsigned int[NumFaces*3]; //GLuint
	}

	//copy data from internal mesh vectors into array buffers for the graphics card
	switch (_VertexFormat) {
	case Position:
		for (vector<VertexOnly>::iterator vIT=vertices_V.begin(); vIT!=vertices_V.end(); ++vIT) {
			VertexOnly V=*vIT;
			int v = V.Index*3;
			buf_vertices[v]=V.P.x;
			buf_vertices[v+1]=V.P.y;
			buf_vertices[v+2]=V.P.z;
		}
		break;
	case PositionColour:
		for (vector<VertexColour>::iterator vIT=vertices_VC.begin(); vIT!=vertices_VC.end(); ++vIT) {
			VertexColour VC=*vIT;
			int v = VC.Index*3;
			buf_vertices[v]=VC.P.x;
			buf_vertices[v+1]=VC.P.y;
			buf_vertices[v+2]=VC.P.z;
			buf_colours[v]=VC.RGB.r;
			buf_colours[v+1]=VC.RGB.g;
			buf_colours[v+2]=VC.RGB.b;
		}
		break;
	case PositionColourTexture:
		for (vector<VertexColourTexture>::iterator vIT=vertices_VCT.begin(); vIT!=vertices_VCT.end(); ++vIT) {
			VertexColourTexture VCT=*vIT;
			int v = VCT.Index*3;
			buf_vertices[v]=VCT.P.x;
			buf_vertices[v+1]=VCT.P.y;
			buf_vertices[v+2]=VCT.P.z;
			buf_colours[v]=VCT.RGB.r;
			buf_colours[v+1]=VCT.RGB.g;
			buf_colours[v+2]=VCT.RGB.b;
			buf_textures[VCT.Index*2]=VCT.UV.x;
			buf_textures[VCT.Index*2+1]=VCT.UV.y;
		}
		break;
	case PositionColourTextureNormal:
		for (vector<VertexColourTextureNormal>::iterator vIT=vertices_VCTN.begin(); vIT!=vertices_VCTN.end(); ++vIT) {
			VertexColourTextureNormal VCTN=*vIT;
			int v = VCTN.Index*3;
			buf_vertices[v]=VCTN.P.x;
			buf_vertices[v+1]=VCTN.P.y;
			buf_vertices[v+2]=VCTN.P.z;
			buf_colours[v]=VCTN.RGB.r;
			buf_colours[v+1]=VCTN.RGB.g;
			buf_colours[v+2]=VCTN.RGB.b;
			buf_textures[VCTN.Index*2]=VCTN.UV.x;
			buf_textures[VCTN.Index*2+1]=VCTN.UV.y;
			buf_normals[v]=VCTN.N.x;
			buf_normals[v+1]=VCTN.N.y;
			buf_normals[v+2]=VCTN.N.z;
		}
		break;
	case PositionColourNormal:
		for (vector<VertexColourNormal>::iterator vIT=vertices_VCN.begin(); vIT!=vertices_VCN.end(); ++vIT) {
			VertexColourNormal VCN=*vIT;
			int v = VCN.Index*3;
			buf_vertices[v]=VCN.P.x;
			buf_vertices[v+1]=VCN.P.y;
			buf_vertices[v+2]=VCN.P.z;
			buf_colours[v]=VCN.RGB.r;
			buf_colours[v+1]=VCN.RGB.g;
			buf_colours[v+2]=VCN.RGB.b;
			buf_normals[v]=VCN.N.x;
			buf_normals[v+1]=VCN.N.y;
			buf_normals[v+2]=VCN.N.z;
		}
		break;
	case InterleavedPositionColour:
		//TODO:
		break;
	case InterleavedPositionColourTexture:
		//TODO:
		break;
	case InterleavedPositionColourNormal:
		//TODO:
		break;
	case InterleavedPositionColourTextureNormal:
		//TODO:
		break;
	}

	//now the faces, which are the same for any vertex format
	if (_HasIndexBuffer)
	{
		int fc=0;
		for (vector<int>::iterator fIT=faces.begin(); fIT!=faces.end(); ++fIT) {
			buf_indices[fc++]=*fIT;
		}
	}

	//create a vertex data object that holds our buffer definitions which we create in the next block
	vertexData = new VertexData();

	//now copy the vertex, colour, texture, normal (as appropriate) and index data to the buffers
	//then free the in-memory buffers as we don't need them any more
	vb->CopyFromMemory(buf_vertices);
	delete [] buf_vertices;
	vertexData->_vb.push_back(vb); //push the vertex buffer
	if (_VertexFormat&VertexFormatColourBit) {
		vc->CopyFromMemory(buf_colours);
		delete [] buf_colours;
		vertexData->_vb.push_back(vc); //push the colour buffer
	}
	if (_VertexFormat&VertexFormatTextureBit) {
		vt->CopyFromMemory(buf_textures);
		delete [] buf_textures;
		vertexData->_vb.push_back(vt); //push the texture buffer
	}
	if (_VertexFormat&VertexFormatNormalBit) {
		vn->CopyFromMemory(buf_normals);
		delete [] buf_normals;
		vertexData->_vb.push_back(vn); //push the normal buffer
	}
	if (_HasIndexBuffer) {
		ib->CopyFromMemory(buf_indices);
		delete [] buf_indices;
	}
	//todo: could clear vertices and faces as well to save memory as our original data still exists

	//now finish off the vertexData initialisation
	if (_HasIndexBuffer) {
		vertexData->_ib=ib; //set the index buffer
		vertexData->_NumElements=NumFaces; //*3;
	}
	else {
		vertexData->_NumElements=0;
	}
	drawObject._vertexData = vertexData; //attach the vertex data to the draw object so we know what buffers are needed for drawing

	//set render state object for this mesh e.g. front face culling, CCW?
	//do nothing for now, just use default

	//OK, that's the buffers and render state done, now set up the draw object needed to do the drawing
	drawObject._PrimType=ptTriangles; //ptTriangles; //ptTriangles; ptLines;
	//shader program set via attach shader

	drawObject._rs->_DepthTest._Enabled=true;
	drawObject._rs->_DepthTest._Function=Greater; //check - is this right? Less?
	drawObject._rs->_FaceCulling._Enabled=true;
	drawObject._rs->_FaceCulling._FaceTest=CullBackFace;
	drawObject._rs->_FaceCulling._WindingOrder=CCW;
	//clear depth 1.0 ?

	//TODO: render state, shaders etc here
}

/// <summary>
/// Fallback for versions of OpenGL before 2.0.
/// Older versions of OpenGL don't have support for the VAO and GenBuffers functions.
/// Buffers have to be created in memory and passed when drawing.
/// Should only be called when hasProgrammableShaders==false
/// </summary>
//void Mesh2::CreateBuffersFallback() {
//	unsigned int NumVertices = vertices.size();
//	unsigned int NumFaces = faces.size(); //this is the number of face indices i.e. 3 * actual number of faces
//
//	NumElements = NumFaces; //needed for object3d, otherwise it doesn't know how many elements to render, NumElements is the count of face index elements (3 per triangle face)
//
//	//create graphics card buffer vertices, colours, indexes
//	mem_vertexbuffer = new GLfloat[NumVertices*3];
//	mem_colourbuffer = new GLfloat[NumVertices*3];
//	mem_indexbuffer = new GLuint[NumFaces];
//
//	//copy data from internal mesh vectors into array buffers for the graphics card
//	//int v=0, vc=0;
//	for (vector<VertexColour>::iterator vIT=vertices.begin(); vIT!=vertices.end(); ++vIT) {
//		//VertexColour VC=*vIT;
//		//mem_vertexbuffer[v++]=VC.P.x;
//		//mem_vertexbuffer[v++]=VC.P.y;
//		//mem_vertexbuffer[v++]=VC.P.z;
//		////colours
//		//mem_colourbuffer[vc++]=VC.RGB.r;
//		//mem_colourbuffer[vc++]=VC.RGB.g;
//		//mem_colourbuffer[vc++]=VC.RGB.b;
//
//		//new code taking into account x ordering of vertices
//		VertexColour VC=*vIT;
//		int v=VC.Index*3;
//		mem_vertexbuffer[v]=VC.P.x;
//		mem_vertexbuffer[v+1]=VC.P.y;
//		mem_vertexbuffer[v+2]=VC.P.z;
//		//colours
//		mem_colourbuffer[v]=VC.RGB.r;
//		mem_colourbuffer[v+1]=VC.RGB.g;
//		mem_colourbuffer[v+2]=VC.RGB.b;
//	}
//	int fc=0;
//	for (vector<int>::iterator fIT=faces.begin(); fIT!=faces.end(); ++fIT) {
//		mem_indexbuffer[fc++]=*fIT;
//	}
//}

/// <summary>
/// TODO:
/// Free any GPU buffers that have been created. Should be called in the destroy code.
/// </summary>
void Mesh2::FreeBuffers() {
	//cout<<"Mesh2::FreeBuffers"<<endl;
	//this is easy, everything is taken care of in gengine

	delete vertexData; //as all the buffers are now invalid anyway
	vertexData=NULL;
}

/// <summary>
/// Shaders are attached to each mesh as a pointer to a shader in a global collection. This way, we're not
/// using a separate shader for each mesh in the scene which would be very bad.
/// Also, objects are drawn so as to minimise the number of shader program switches for optimisation.
/// If recursive is set, then this object and all its children have their shader assigned. If recursive is
/// false, then only this object is set. Also, any object where HasGeometry==false does not have the shader
/// set as there is no draw object to set the shader on, only a null draw object.
/// </summary>
/// <param name="pShader">Pointer to the shader program to attach i.e. use for drawing this object</param>
/// <param name="Recursive">If true, set this object and all children recursively, otherwise only set this object</param>
void Mesh2::AttachShader(gengine::Shader* pShader, bool Recursive) {
	drawObject._ShaderProgram = pShader;
	if (Recursive) {
		//now set all the children - NOTE: you still need to call the attach virtual even if a child is an Object3D as
		//it still might have children which are drawable.
		for (vector<Object3D*>::iterator childIT=Children.begin(); childIT!=Children.end(); ++childIT) {
			(*childIT)->AttachShader(pShader,true);
		}
	}
}

//not strictly used as we're using the GetDrawObject method
//void Mesh2::Render(glm::mat4 ParentMat) {
//	glm::mat4 mm = ParentMat * modelMatrix; //post multiply child matrix
//
//	//set model matrix for this model draw
//	drawObject._ModelMatrix = mm;
//	//OGLContext
//	//TODO: render here!
//
//	//then go on to render all the children
//	for (vector<Object3D*>::iterator childIT=Children.begin(); childIT!=Children.end(); ++childIT) {
//		(*childIT)->Render(mm);
//	}
//}

/// <summary>
/// Render mesh. The main point of this is to allow specialised objects to take control of the drawing and do something
/// special, for example, the globe chunked LOD rendering.
/// NOTE: this does not render any children.
/// </summary>
void Mesh2::Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo) {
	const DrawObject& dobj=GetDrawObject();
	GC->Render(dobj,sdo);
}

/// <summary>
/// Return the draw object, which is needed by the graphics context to draw this object. Contains all opengl state, buffers and matrix.
/// </summary>
/// <returns>The draw object containing everything needed to do the drawing</returns>
const gengine::DrawObject& Mesh2::GetDrawObject() {
	//we need to get the modelMatrix from the parent class property and put it into the draw object
	//TODO: you could pass in a parent modelMatrix and do the pre-multiply here?
	drawObject._ModelMatrix = this->modelMatrix;
	return drawObject;
}

/// <summary>
/// Attach a texture and assign its texture unit number
/// <summary>
void Mesh2::AttachTexture(unsigned int TextureUnitNum, gengine::Texture2D* Texture) {
	drawObject._textures[TextureUnitNum]=Texture;
	cout<<"Num textures = "<<drawObject._textures.size()<<endl;
}

