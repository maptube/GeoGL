
#include "mesh2.h"
#include <math.h>
#include <algorithm>
#include <iterator>
#include "DebugUtils.h"

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

	//vertices.reserve(1024);
	//faces.reserve(1024);

	renderState = new RenderState();
	textureUnits = new TextureUnits();
	drawObject._rs=renderState; //TODO: find a more elegant way of doing this
	drawObject._texUnits = textureUnits; //TODO: and this
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
	delete textureUnits;
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


//assumes x sorted vertices list
	VertexColour item;
	item.P=P;
	item.P.x-=epsilon; //this is the lower bound on x
	item.RGB=Colour;
	vector<struct VertexColour>::iterator it = std::lower_bound(vertices.begin(), vertices.end(), item);
	vector<struct VertexColour>::iterator insertPoint = it;
	while ((it!=vertices.end())&&((*it).P.x<=P.x+epsilon)) {
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
	item.Index=vertices.size(); //TODO: check how efficient count is
	item.P=P; //put x back to its real value
	
	vertices.insert(insertPoint,item);
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

/// <summary>
/// Scale all vertices by a vector (Sx, Sy, Sz). Useful for objects which are created to unit size and need rescaling.
/// </summary>
/// <param name="Sx">Scale factor for X axis</param>
/// <param name="Sy">Scale factor for Y axis</param>
/// <param name="Sz">Scale factor for Z axis</param>
void Mesh2::ScaleVertices(double Sx, double Sy, double Sz) {
	vector<struct VertexColour>::iterator it;
	for (it = vertices.begin(); it!=vertices.end(); ++it) {
		//this is horrible, how about std::algorithms?
		//also, the vertex buffers are floats because of the GPU limitation, while everywhere else they're doubles
		(*it).P.x*=(float)Sx;
		(*it).P.y*=(float)Sy;
		(*it).P.z*=(float)Sz;
	}
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

	bool hasColourBuffer = (_VertexFormat&0x2); //this should be a separate colour buffer ONLY

	unsigned int NumVertices = vertices.size();
	unsigned int NumFaces = faces.size(); //this is the number of face indices i.e. 3 * actual number of faces

	//graphics card buffer vertices, colours, indexes
	
	VertexBuffer* vb=nullptr;
	VertexBuffer* vc=nullptr;
	//TODO: the sizes are really GL sizes, but we shouldn't be using GL types directly outside gengine - how to best do this?
	vb = OGLDevice::CreateVertexBuffer("in_Position",ArrayBuffer,StaticDraw,NumVertices*3*sizeof(float));
	if (hasColourBuffer)
		vc = OGLDevice::CreateVertexBuffer("in_Color",ArrayBuffer,StaticDraw,NumVertices*3*sizeof(float));
	IndexBuffer* ib=OGLDevice::CreateIndexBuffer(ElementArrayBuffer,StaticDraw,NumFaces*sizeof(unsigned int)); //note that NumFaces is already *3 (see def above)

	//create internal memory blocks in format suitable for copying to opengl vertex and index buffers
	float* buf_vertices, *buf_colours;
	buf_vertices = new float[NumVertices*3]; //technically it's a GLfloat
	if (hasColourBuffer)
		buf_colours = new float[NumVertices*3]; //GLfloat
	unsigned int* buf_indices = new unsigned int[NumFaces*3]; //GLuint

	//copy data from internal mesh vectors into array buffers for the graphics card
	//int v=0, vc=0;
	for (vector<VertexColour>::iterator vIT=vertices.begin(); vIT!=vertices.end(); ++vIT) {
		//VertexColour VC=*vIT;
		//buf_vertices[v++]=VC.P.x;
		//buf_vertices[v++]=VC.P.y;
		//buf_vertices[v++]=VC.P.z;
		////colours
		//buf_colours[vc++]=VC.RGB.r;
		//buf_colours[vc++]=VC.RGB.g;
		//buf_colours[vc++]=VC.RGB.b;

		//new code taking into account x ordering of vertices
		VertexColour VC=*vIT;
		int v = VC.Index*3;
		buf_vertices[v]=VC.P.x;
		buf_vertices[v+1]=VC.P.y;
		buf_vertices[v+2]=VC.P.z;
		//colours
		if (hasColourBuffer)
		{
			buf_colours[v]=VC.RGB.r;
			buf_colours[v+1]=VC.RGB.g;
			buf_colours[v+2]=VC.RGB.b;
		}
	}
	int fc=0;
	for (vector<int>::iterator fIT=faces.begin(); fIT!=faces.end(); ++fIT) {
		buf_indices[fc++]=*fIT;
	}

	//now copy the vertex, colour and index data to the buffers
	vb->CopyFromMemory(buf_vertices);
	if (hasColourBuffer)
		vc->CopyFromMemory(buf_colours);
	ib->CopyFromMemory(buf_indices);

	//free the in-memory buffers as we don't need them any more
	delete [] buf_vertices;
	if (hasColourBuffer)
		delete [] buf_colours;
	delete [] buf_indices;
	//todo: could clear vertices and faces as well to save memory as our original data still exists

	//create a vertex data object that holds our buffer definitions
	vertexData = new VertexData();
	vertexData->_vb.push_back(vb); //push the vertex buffer
	if (hasColourBuffer)
		vertexData->_vb.push_back(vc); //push the colour buffer
	vertexData->_ib=ib; //set the index buffer
	vertexData->_NumElements=NumFaces; //*3;
	drawObject._vertexData = vertexData; //attach the vertex data to the draw object so we know what buffers are needed for drawing

	//set render state object for this mesh e.g. front face culling, CCW?
	//do nothing for now, just use default

	//OK, that's the buffers and render state done, now set up the draw object needed to do the drawing
	drawObject._PrimType=ptTriangles;
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
	
	//localised these and stored in vertexData
	//delete vb;
	//delete vc;
	//delete ib;
	//vb=NULL; vc=NULL; ib=NULL;

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
/// Return the draw object, which is needed by the graphics context to draw this object. Contains all opengl state, buffers and matrix.
/// </summary>
/// <returns>The draw object containing everything needed to do the drawing</returns>
const gengine::DrawObject& Mesh2::GetDrawObject() {
	//we need to get the modelMatrix from the parent class property and put it into the draw object
	//TODO: you could pass in a parent modelMatrix and do the pre-multiply here?
	drawObject._ModelMatrix = this->modelMatrix;
	return drawObject;
}


//void TestMesh2() {
//	//Creates a mesh object directly for testing purposes
//	//test2 - the mesh2 object
//	Mesh2* mesh2 = new Mesh2();
//	glm::vec3 mesh_v[] = {
//		glm::vec3(-1, -1, 0),
//		glm::vec3(1, -1, 0),
//		glm::vec3(1, 1, 0),
//		glm::vec3(-1, 1, 0)
//	};
//	glm::vec3 mesh_c[] = {
//		glm::vec3(1,0,0),
//		glm::vec3(0,1,0),
//		glm::vec3(0,0,1),
//		glm::vec3(1,0,1)
//	};
//	mesh2->AddFace(mesh_v[0],mesh_v[1],mesh_v[2], mesh_c[0],mesh_c[1],mesh_c[2]);
//	mesh2->AddFace(mesh_v[0],mesh_v[2],mesh_v[3], mesh_c[0],mesh_c[2],mesh_c[3]);
//	mesh2->AttachShader(shader); //attach the same shader as the triangle
//	mesh2->CreateBuffers();
//	//position the mesh
//	glm::mat4 mesh_mm=glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-2));
//	mesh2->drawObject._ModelMatrix = mm;
//	//OK, that's a mesh set up - need to delete it later though!
//}
