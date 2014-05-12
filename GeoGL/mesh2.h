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
//#include "gengine/vertexbuffer.h"
//#include "gengine/indexbuffer.h"
//#include "gengine/shader.h"
#include "gengine/drawobject.h"
#include "object3d.h"

//forward declarations
namespace gengine {
	class VertexBuffer;
	class IndexBuffer;
	class Shader;
}

struct VertexColour {
	glm::vec3 P; //vertex (point)
	glm::vec3 RGB; //colour
	int Index; //face index number (labels point uniquely)
	bool operator < (const VertexColour &rhs) const { return P.x < rhs.P.x; } //x order sort
};

class Mesh2 : public Object3D
{
public:
	float epsilon;
	gengine::DrawObject drawObject; //promoted from protected
	Mesh2(void);
	~Mesh2(void);
	static float ManhattanDist(glm::vec3 V1, glm::vec3 V2);
	int AddVertex(glm::vec3 P, glm::vec3 Colour);
	void AddFace(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3);
	void SetColour(glm::vec3 new_colour);
	void ScaleVertices(float Sx,float Sy,float Sz);
	void CreateBuffers();
	void FreeBuffers();
	void AttachShader(gengine::Shader* pshader);
	virtual BBox GetGeometryBounds();
	virtual void Render(glm::mat4 ParentMat);
protected:
	gengine::VertexBuffer* vb; //vertex buffer
	gengine::VertexBuffer* vc; //colour buffer
	gengine::IndexBuffer* ib; //index buffer
	//gengine::Shader* shader;
	gengine::VertexData* vertexData; //collection of vertex and index buffers
	gengine::RenderState* renderState;
	//gengine::DrawObject drawObject;

	std::vector<struct VertexColour> vertices;
	std::vector<int> faces; //indexed into vertices
	//void CreateBuffersFallback(); //UNNECESSARY??
};

