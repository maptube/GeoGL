#pragma once

#include "gengine/gengine.h"
#include "pathshapes.h"

//forward declarations
class Mesh2;


class Triangulator {
protected:
	PathShape _Shape;
public:
	Triangulator();
	~Triangulator();

	void Clear(void);
	void SetOuterRing(const LinearRing& Outer);
	void AddInnerRing(const LinearRing& Inner);
	void SetShape(const PathShape& Shape);
	void Triangulate(Mesh2& geom);
};

