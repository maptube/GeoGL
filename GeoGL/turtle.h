#pragma once

#include "opengl4.h"
#include "mesh.h"

/// <summary>
/// Turtle class for representing a mesh shaped like an ABM turtle
/// </summary>
class Turtle : public Mesh
{
public:
	float Size;
	glm::vec3 Colour;
	Turtle(float TurtleSize);
	~Turtle(void);
};
