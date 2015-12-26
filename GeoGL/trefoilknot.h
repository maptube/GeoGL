#pragma once
//#include "opengl4.h"
#include "tubegeometry.h"

class TrefoilKnot : public TubeGeometry
{
public:
	TrefoilKnot(float Radius, int NumSegments);
	~TrefoilKnot(void);
};

