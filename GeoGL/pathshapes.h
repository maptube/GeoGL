#pragma once

#include "gengine/gengine.h"
#include <vector>

//definition of rings and anything else used for geographic objects
typedef std::vector<glm::vec3> LinearRing;

struct PathShape {
	LinearRing outer;
	std::vector<LinearRing> inner;
};
