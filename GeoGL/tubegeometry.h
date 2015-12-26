#pragma once
//#include "opengl4.h"
#include "mesh2.h"

#include <vector>

/**
* Geometry representing a spline which has a physical radius (a tube in other words). In 3DS Max they call this a renderable spline, but this is specifically circular.
* Technically, a spline is a single line, but this handles a list of splines.
*/
class TubeGeometry : public Mesh2
{
public:
	TubeGeometry(void);
	~TubeGeometry(void);
	void AddSpline(std::vector<glm::vec3> points,std::vector<glm::vec3> point_colours);
	void GenerateTube(float Radius, unsigned int NumSegments, std::vector<glm::vec3> p, std::vector<glm::vec3> colours);
	void GenerateMesh(float Radius, unsigned int NumSegments);
protected:
	std::vector<std::vector<glm::vec3>> splines; //list of a list of contiguous points
	std::vector<std::vector<glm::vec3>> spline_colours; //list of a list of colours to go with the spline points
};

