#pragma once
#include "BBox.h"

/**
* Helper class for creating mesh objects by adding vertices and faces. Welding is based on an epsilon value for vertices and colours.
* Inherits from Object3D to provide a class for general meshes.
*/
#include "main.h"
#include "opengl4.h"
#include <vector>

struct VertexColour {
	glm::vec3 P; //vertex (point)
	glm::vec3 RGB; //colour
	int Index; //face index number (labels point uniquely)
	bool operator < (const VertexColour &rhs) const { return P.x < rhs.P.x; } //x order sort
};

class Mesh : public Object3D
{
public:
	float epsilon;
	Mesh(void);
	~Mesh(void);
	static float ManhattanDist(glm::vec3 V1, glm::vec3 V2);
	int AddVertex(glm::vec3 P, glm::vec3 Colour);
	void AddFace(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 Colour1, glm::vec3 Colour2, glm::vec3 Colour3);
	void SetColour(glm::vec3 new_colour);
	void ScaleVertices(float Sx,float Sy,float Sz);
	void CreateBuffers();
	void FreeBuffers();
	virtual BBox GetGeometryBounds();
protected:
	std::vector<struct VertexColour> vertices;
	std::vector<int> faces; //indexed into vertices
	void CreateBuffersFallback();
};

