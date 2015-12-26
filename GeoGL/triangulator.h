#pragma once

#include "gengine/gengine.h"
#include "pathshapes.h"
#include "poly2tri.h"

//forward declarations
class Mesh2;


class Triangulator {
protected:
	PathShape _Shape;
private:
	//storage for points used by triangulator
	std::vector<std::vector<p2t::Point*>> _p2t_linearrings;
	bool _hasData; //used to determine whether data needs to be deleted before a new triangulation is performed
	void FreeData(void);
public:
	Triangulator();
	~Triangulator();

	p2t::CDT* _cdt; //this allows access to the triangulator object to get the points back, although you can call GetTriangles

	void Clear(void);
	void SetOuterRing(const LinearRing& Outer);
	void AddInnerRing(const LinearRing& Inner);
	void SetShape(const PathShape& Shape);
	void Triangulate(); //call this then access _cdt for results
	void debugWriteTriangles();
	std::vector<p2t::Triangle*> GetTriangles() { return _cdt->GetTriangles(); };
};

