#pragma once

//#include "opengl4.h"
#include "mesh2.h"
#include "gengine/vertexformat.h"

/// <summary>
/// Turtle class for representing a mesh shaped like an ABM turtle
/// </summary>
class Turtle : public Mesh2
{
protected:
	void init(float TurtleSize, gengine::VertexFormat VF);
public:
	float Size;
	glm::vec3 Colour;
	Turtle(float TurtleSize);
	Turtle(float TurtleSize, gengine::VertexFormat VF);
	~Turtle(void);
};
