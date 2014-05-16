////////////////////////////////////////////////////////
//THIS CLASS IS NO LONGER USED - SUPERCEDED BY MESH2////
////////////////////////////////////////////////////////


#include "mesh.h"
#include <math.h>
#include <algorithm>
#include <iterator>
#include "DebugUtils.h"

using namespace std;

/// <summary>
/// DEBUG: print out points to console NOT cout
/// </summary>
//void printPoints(vector<struct VertexColour> vertices, glm::vec3 P) {
//	//debug print out points
//	DebugUtils::WriteString("Insert point: "); DebugUtils::WriteFloat(P.x); DebugUtils::WriteLine();
//	DebugUtils::WriteString("X: ");
//	for (vector<struct VertexColour>::iterator it=vertices.begin(); it!=vertices.end(); ++it) {
//		DebugUtils::WriteFloat((*it).P.x);
//		DebugUtils::WriteString(" ");
//	}
//	DebugUtils::WriteLine();
//}

/// <summary>
/// DEBUG
/// <summary>
//void checkPointOrdering(vector<struct VertexColour> vertices) {
//	//debug check points in the vertices vector are in the correct x ordering
//	float x = -10000000; //hack max_float!
//	for (vector<struct VertexColour>::iterator it=vertices.begin(); it!=vertices.end(); ++it) {
//		if ((*it).P.x<x) {
//			float delta = (*it).P.x - x;
//			DebugUtils::WriteString("Error in x ");
//			DebugUtils::WriteFloat((*it).P.x);
//			DebugUtils::WriteString(" < ");
//			DebugUtils::WriteFloat(x);
//			DebugUtils::WriteString(" delta=");
//			DebugUtils::WriteFloat(delta);
//			DebugUtils::WriteLine();
//		}
//		x=(*it).P.x;
//	}
//}

/// <summary>
/// Constructor. Create an Object3D which has geometry. All geometric primitives derive from this class as it wraps the OpenGL buffer creation.
/// </summary>
Mesh::Mesh(void)
{
	epsilon = 0.0000001f;

	//initialise fallback cpu buffers to null
	//mem_vertexbuffer = NULL;
	//mem_colourbuffer = NULL;
	//mem_indexbuffer = NULL;
}

/// <summary>
/// Destructor
/// </summary>
Mesh::~Mesh(void)
{
	//todo: free the non-fallback buffers as well
	//if (mem_vertexbuffer!=NULL) delete mem_vertexbuffer;
	//if (mem_colourbuffer!=NULL) delete mem_colourbuffer;
	//if (mem_indexbuffer!=NULL) delete mem_indexbuffer;

	FreeBuffers();
}

//Pattern: add vertices, then add faces OR just add faces?
//The best format for doing CSG might be to push sphere/cylinder/cube geoms to a mesh and manipluate in this class rather than them all deriving from mesh?

/// <summary>
/// return Manhattan distance between two vectors, either points or colours
/// </summary>
/// <param name="V1">First vector</param>
/// <param name="V2">Second vector</param>
/// <returns>The Manhattan distance between vectors V1 and V2</returns>
float Mesh::ManhattanDist(glm::vec3 V1, glm::vec3 V2) {
	return abs(V1.x-V2.x)+abs(V1.y-V2.y)+abs(V1.z-V2.z);
}

/// <summary>
/// Get bounding box for this object and all its children, but only for the geometry that you can actually see.
/// A pure Object3D returns nothing as you can't see it. A mesh and its subclasses return the bounds of their contained face vertices.
/// This is a mesh, so we return the geometry bounds.
/// TODO: you might want to check that the offsets and matrix rotations are right?
/// </summary>
/// <returns>The bounding box of any objects and children containing visible geometry</returns>
BBox Mesh::GetGeometryBounds() {
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
/// Add a vertex with a colour to the list of vertices, but only if it doesn't already exist.
/// Returns index of the point added or existing
/// </summary>
/// <param name="P">Vertex position vector</param>
/// <param name="Colour">Colour vector</param>
/// <returns>The index of the point added, or the index of an existing point if it could weld them</returns>
int Mesh::AddVertex(glm::vec3 P, glm::vec3 Colour) {
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


//assumes x sorted vertices list
	//this was the live code before mesh2
	//VertexColour item;
	//item.P=P;
	//item.P.x-=epsilon; //this is the lower bound on x
	//item.RGB=Colour;
	//vector<struct VertexColour>::iterator it = std::lower_bound(vertices.begin(), vertices.end(), item);
	//vector<struct VertexColour>::iterator insertPoint = it;
	//while ((it!=vertices.end())&&((*it).P.x<=P.x+epsilon)) {
	//	struct VertexColour VC=*it;
	//	glm::vec3 P2=VC.P, C2=VC.RGB;
	//	float d = ManhattanDist(P,P2) + ManhattanDist(Colour,C2); //Manhattan dist between vertex pos and colour (could separate?)
	//	if (d<epsilon) {
	//		//DebugUtils::WriteString("return early"); DebugUtils::WriteLine();
	//		//printPoints(vertices,P);
	//		return VC.Index; //point at this position with this colour (<epsilon) is already in the list, so exit early
	//	}
	//	if (P.x>=P2.x) {
	//		insertPoint=it; //keep track of where we might need to insert a new x value
	//		//DebugUtils::WriteString("CMP: "); DebugUtils::WriteFloat(P.x);
	//		//DebugUtils::WriteString(">="); DebugUtils::WriteFloat(P2.x);
	//		//DebugUtils::WriteLine();
	//	}
	//	++it;
	//}
	//if we've got to this point, then we need to add a new point, and as we bailed out when *it.P.x>P.x, then *it is the position to insert
	//NO - you need to go to x+epsilon as you can have x1=x2+e, y1=y2 and z1=z2 for a match.
	//The problem then is that the x ordering is wrong! The solution is to use the insertPoint that we've set to the correct position while iterating earlier.
	//InsertPoint holds last iteration of place while P.x>=P2.x holds, which is the correct insert point
	//item.Index=vertices.size(); //TODO: check how efficient count is
	//item.P=P; //put x back to its real value
	//
	//vertices.insert(insertPoint,item);
	////int insertPos = std::distance(insertPoint,vertices.begin());
	////DebugUtils::WriteString("Insert at: "); DebugUtils::WriteInt(insertPos); DebugUtils::WriteLine();
	////printPoints(vertices,P);
	////std::sort(vertices.begin(),vertices.end()); //hack!
	//bounds.ExpandToIncludePoint(item.P);
	//return item.Index;


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
	return 0; //HACK - added this as the rest is commented out
}

/// <summary>
/// Scale all vertices by a vector (Sx, Sy, Sz). Useful for objects which are created to unit size and need rescaling.
/// </summary>
/// <param name="Sx">Scale factor for X axis</param>
/// <param name="Sy">Scale factor for Y axis</param>
/// <param name="Sz">Scale factor for Z axis</param>
void Mesh::ScaleVertices(float Sx, float Sy, float Sz) {
	//vector<struct VertexColour>::iterator it;
	//for (it = vertices.begin(); it!=vertices.end(); ++it) {
	//	//this is horrible, how about std::algorithms?
	//	(*it).P.x*=Sx;
	//	(*it).P.y*=Sy;
	//	(*it).P.z*=Sz;
	//}
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
void Mesh::AddFace(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3) {
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
/// Change the vertex colour buffer data.
/// This is a slightly heavyweight way of doing things - chuck all the buffers and re-create them - but then again changing the colour is going to mess a lot of things up.
/// You could write a re-create buffers funtion which just applies new data to existing buffers without having to create them again, probably not much speedup though?
/// </summary>
/// <param name="new_colour"></param>
void Mesh::SetColour(glm::vec3 new_colour)
{
	////basically, what I'm going to do here is to change all the colours assigned to the faces and then re-create the buffers
	//for (vector<struct VertexColour>::iterator it = vertices.begin(); it!=vertices.end(); ++it) {
	//	it->RGB=new_colour;
	//}
	//FreeBuffers();
	//CreateBuffers();
}


//need to convert mesh to object3d vertex and index buffers
/// <summary>
/// Take the vertices and faces currently stored and make Object3D buffers for OpenGL.
/// Assumes you have been calling AddFace() with the data first.
/// </summary>
void Mesh::CreateBuffers() {
	//commented this whole procedure out as it's using a shader that no longer exists - need to move everything to Mesh2
	////fallback trap
	//if (!OpenGLContext::hasProgrammableShaders) {
	//	CreateBuffersFallback();
	//	return;
	//}

	//unsigned int NumVertices = vertices.size();
	//unsigned int NumFaces = faces.size(); //this is the number of face indices i.e. 3 * actual number of faces

	////graphics card buffer vertices, colours, indexes
	//GLfloat* buf_vertices = new GLfloat[NumVertices*3];
	//GLfloat* buf_colours = new GLfloat[NumVertices*3];
	//GLuint* buf_indices = new GLuint[NumFaces];

	////copy data from internal mesh vectors into array buffers for the graphics card
	////int v=0, vc=0;
	//for (vector<VertexColour>::iterator vIT=vertices.begin(); vIT!=vertices.end(); ++vIT) {
	//	//VertexColour VC=*vIT;
	//	//buf_vertices[v++]=VC.P.x;
	//	//buf_vertices[v++]=VC.P.y;
	//	//buf_vertices[v++]=VC.P.z;
	//	////colours
	//	//buf_colours[vc++]=VC.RGB.r;
	//	//buf_colours[vc++]=VC.RGB.g;
	//	//buf_colours[vc++]=VC.RGB.b;

	//	//new code taking into account x ordering of vertices
	//	VertexColour VC=*vIT;
	//	int v = VC.Index*3;
	//	buf_vertices[v]=VC.P.x;
	//	buf_vertices[v+1]=VC.P.y;
	//	buf_vertices[v+2]=VC.P.z;
	//	//colours
	//	buf_colours[v]=VC.RGB.r;
	//	buf_colours[v+1]=VC.RGB.g;
	//	buf_colours[v+2]=VC.RGB.b;
	//}
	//int fc=0;
	//for (vector<int>::iterator fIT=faces.begin(); fIT!=faces.end(); ++fIT) {
	//	buf_indices[fc++]=*fIT;
	//}

	////now start setting up the buffers

	//NumElements = NumFaces; //*3; //needed for object3d, otherwise it doesn't know how many elements to render, NumElements is the count of face index elements (3 per triangle face)

	//glGenVertexArrays(1, &vaoID); // Create our Vertex Array Object

	//glBindVertexArray(vaoID); // Bind our Vertex Array Object so we can use it
	//
	//glGenBuffers(1, &vboID); // Generate our Vertex Buffer Object
	//
	////be careful with the sizes here, as the buffers need real memory sizes, not numbers of points or indices
	//glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind our Vertex Buffer Object
	//glBufferData(GL_ARRAY_BUFFER, NumVertices*3 * sizeof(GLfloat), &buf_vertices[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
	//glVertexAttribPointer((GLuint)Shader::v_inPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer

	////bind index array
	//glGenBuffers(1, &iboID); //generate 1 buffer object name, returned in the array
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID); //bind index buffer object - this links the name to the binding point of the specified buffer type
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumFaces/**3*/ * sizeof(GLuint), &buf_indices[0], GL_STATIC_DRAW); //note: actually indices.size*sizeof(unsigned int) (=12*4?)

	////bind colour array
	//glGenBuffers(1, &vcboID); //generate colour buffer object
	//glBindBuffer(GL_ARRAY_BUFFER, vcboID); //bind vertex colour buffer
	//glBufferData(GL_ARRAY_BUFFER, NumVertices*3 * sizeof(GLfloat), &buf_colours[0], GL_STATIC_DRAW);
	//glVertexAttribPointer((GLuint)Shader::v_inColor, 3, GL_FLOAT, GL_FALSE, 0, NULL); // (note enable later)

	//glEnableVertexAttribArray(Shader::v_inPosition); // Enable our vertex buffer in our Vertex Array Object
	//glEnableVertexAttribArray(Shader::v_inColor); // Enable our colour buffer in our Vertex Array Object

	//glBindVertexArray(0); // Disable our Vertex Buffer Object

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //disable vertex buffer object
	//glBindBuffer(GL_ARRAY_BUFFER, 0); //and the other one

	////make sure we clean up after ourselves
	//delete [] buf_vertices;
	//delete [] buf_colours;
	//delete [] buf_indices;
}

/// <summary>
/// Fallback for versions of OpenGL before 2.0.
/// Older versions of OpenGL don't have support for the VAO and GenBuffers functions.
/// Buffers have to be created in memory and passed when drawing.
/// Should only be called when hasProgrammableShaders==false
/// </summary>
void Mesh::CreateBuffersFallback() {
	//unsigned int NumVertices = vertices.size();
	//unsigned int NumFaces = faces.size(); //this is the number of face indices i.e. 3 * actual number of faces

	//NumElements = NumFaces; //needed for object3d, otherwise it doesn't know how many elements to render, NumElements is the count of face index elements (3 per triangle face)

	////create graphics card buffer vertices, colours, indexes
	//mem_vertexbuffer = new GLfloat[NumVertices*3];
	//mem_colourbuffer = new GLfloat[NumVertices*3];
	//mem_indexbuffer = new GLuint[NumFaces];

	////copy data from internal mesh vectors into array buffers for the graphics card
	////int v=0, vc=0;
	//for (vector<VertexColour>::iterator vIT=vertices.begin(); vIT!=vertices.end(); ++vIT) {
	//	//VertexColour VC=*vIT;
	//	//mem_vertexbuffer[v++]=VC.P.x;
	//	//mem_vertexbuffer[v++]=VC.P.y;
	//	//mem_vertexbuffer[v++]=VC.P.z;
	//	////colours
	//	//mem_colourbuffer[vc++]=VC.RGB.r;
	//	//mem_colourbuffer[vc++]=VC.RGB.g;
	//	//mem_colourbuffer[vc++]=VC.RGB.b;

	//	//new code taking into account x ordering of vertices
	//	VertexColour VC=*vIT;
	//	int v=VC.Index*3;
	//	mem_vertexbuffer[v]=VC.P.x;
	//	mem_vertexbuffer[v+1]=VC.P.y;
	//	mem_vertexbuffer[v+2]=VC.P.z;
	//	//colours
	//	mem_colourbuffer[v]=VC.RGB.r;
	//	mem_colourbuffer[v+1]=VC.RGB.g;
	//	mem_colourbuffer[v+2]=VC.RGB.b;
	//}
	//int fc=0;
	//for (vector<int>::iterator fIT=faces.begin(); fIT!=faces.end(); ++fIT) {
	//	mem_indexbuffer[fc++]=*fIT;
	//}
}

/// <summary>
/// TODO:
/// Free any GPU buffers that have been created. Should be called in the destroy code.
/// </summary>
void Mesh::FreeBuffers() {
	////fallback trap
	//if (!OpenGLContext::hasProgrammableShaders) {
	//	if (mem_vertexbuffer!=NULL) delete mem_vertexbuffer;
	//	if (mem_colourbuffer!=NULL) delete mem_colourbuffer;
	//	if (mem_indexbuffer!=NULL) delete mem_indexbuffer;
	//}
	//else {
	//	glDeleteVertexArrays(1, &vaoID); // Delete our Vertex Array Object
	//	glGenBuffers(1, &vboID); // Delete our Vertex Buffer Object
	//	glGenBuffers(1, &iboID); //Delete index buffer object
	//	glGenBuffers(1, &vcboID); //Delete colour buffer object
	//}

}

