#pragma once

#include "gengine/gengine.h"
#include <vector>

//definition of rings and anything else used for geographic objects
//the z coordinate isn't used except for extrusion by height of the 2D path shapes
typedef std::vector<glm::dvec3> LinearRing;

struct PathShape {
	LinearRing outer;
	std::vector<LinearRing> inner;
};
