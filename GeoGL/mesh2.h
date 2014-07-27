#pragma once

#include "BBox.h"

///////MESH2!!!!!!

/**
* Helper class for creating mesh objects by adding vertices and faces. Welding is based on an epsilon value for vertices and colours.
* Inherits from Object3D to provide a class for general meshes.
*/
//#include "main.h"
//#include "opengl4.h"
#include <vector>

#include "gengine/gengine.h"
#include "gengine/vertexformat.h"
//#include "gengine/vertexbuffer.h"
//#include "gengine/indexbuffer.h"
//#include "gengine/shader.h"
#include "gengine/drawobject.h"
#include "object3d.h"

//forward declarations
namespace gengine {
	class GraphicsContext;
	class SceneDataObject;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;
	class Texture2D;
}

/////Vertex formats

struct VertexOnly { //could do with a better name for this (Vertex=graph)
	glm::vec3 P; //vertex (point)
	int Index; //face index number (labels point uniquely)
	bool operator < (const VertexOnly &rhs) const { return P.x < rhs.P.x; } //x order sort
};

struct VertexColour {
	glm::vec3 P; //vertex (point)
	glm::vec3 RGB; //colour
	int Index; //face index number (labels point uniquely)
	bool operator < (const VertexColour &rhs) const { return P.x < rhs.P.x; } //x order sort
};

struct VertexColourTexture {
	glm::vec3 P; //vertex
	glm::vec3 RGB; //colour
	glm::vec2 UV; //texture coordinates
	int Index; //face index number (labels point uniquely)
	bool operator < (const VertexColourTexture &rhs) const { return P.x < rhs.P.x; } //x order sort
};

struct VertexColourNormal {
	glm::vec3 P; //vertex
	glm::vec3 RGB; //colour
	glm::vec3 N; //normal
	int Index; //face index number (labels point uniquely)
	bool operator < (const VertexColourNormal &rhs) const { return P.x < rhs.P.x; } //x order sort
};

struct VertexColourTextureNormal {
	glm::vec3 P; //vertex
	glm::vec3 RGB; //colour
	glm::vec2 UV; //texture coordinates
	glm::vec3 N; //normal
	int Index; //face index number (labels point uniquely)
	bool operator < (const VertexColourTextureNormal &rhs) const { return P.x < rhs.P.x; } //x order sort
};

/////////End of vertex formats

class Mesh2 : public Object3D
{
public:
	float epsilon;
	enum gengine::VertexFormat _VertexFormat; //Format of buffers used e.g. separate or interleaved, position, colour, texture, normal

	Mesh2(void);
	~Mesh2(void);
	static float ManhattanDist(glm::vec2 V1, glm::vec2 V2);
	static float ManhattanDist(glm::vec3 V1, glm::vec3 V2);
	int AddVertexRaw(glm::vec3 P);
	int AddVertex(glm::vec3 P);
	int AddVertex(glm::vec3 P, glm::vec3 Colour);
	int AddVertex(glm::vec3 P, glm::vec3 Colour, glm::vec3 Normal);
	int AddVertex(glm::vec3 P, glm::vec3 Colour, glm::vec2 UV, glm::vec3 Normal);
	void AddFace(int Va, int Vb, int Vc);
	void AddFace(
		glm::vec3 P1, glm::vec3 P2, glm::vec3 P3
	);
	void AddFace(
			glm::vec3 P1, glm::vec3 P2, glm::vec3 P3,
			glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3
	);
	void AddFace(
			glm::vec3 P1, glm::vec3 P2, glm::vec3 P3,
			glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3,
			glm::vec3 N1, glm::vec3 N2, glm::vec3 N3
	);
	void AddFace(
			glm::vec3 P1, glm::vec3 P2, glm::vec3 P3,
			glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3,
			glm::vec2 UV1, glm::vec2 UV2, glm::vec2 UV3,
			glm::vec3 N1, glm::vec3 N2, glm::vec3 N3
	);
	void AttachTexture(unsigned int TextureUnitNum, gengine::Texture2D* Texture);
	void SetColour(glm::vec3 new_colour);
	void ScaleVertices(double Sx,double Sy,double Sz);
	void CreateBuffers();
	void FreeBuffers();
	virtual void AttachShader(gengine::Shader* pShader, bool Recursive);
	virtual BBox GetGeometryBounds();
	//virtual void Render(glm::mat4 ParentMat);
	virtual void Render(gengine::GraphicsContext* GC, const gengine::SceneDataObject& sdo);
	virtual bool HasGeometry() { return true; } //yes, this object has geometry associated with it and can be rendered
	virtual const gengine::DrawObject& GetDrawObject();
	//TODO: SetMatrix - VERY IMPORTANT!
protected:
	gengine::VertexData* vertexData; //collection of vertex and index buffers
	gengine::RenderState* renderState;
	gengine::DrawObject drawObject; //contains textures

	int _NumVertices;

	//only one of these formats gets used - ideally I would like to get rid of what is a nasty hack, but templates won't work and
	//the obvious polymorphism looks overkill
	std::vector<struct VertexOnly> vertices_V; //vertex only i.e. no colour, texture or normal
	std::vector<struct VertexColour> vertices_VC; //vertex colour format
	std::vector<struct VertexColourTexture> vertices_VCT; //vertex colour texture format
	std::vector<struct VertexColourNormal> vertices_VCN; //vertex colour normal format
	std::vector<struct VertexColourTextureNormal> vertices_VCTN; //vertex colour texture normal format
	std::vector<int> faces; //indexed into vertices
	//void CreateBuffersFallback(); //UNNECESSARY??
};

