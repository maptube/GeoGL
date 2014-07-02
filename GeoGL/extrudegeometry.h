#pragma once

#include <vector>

#include "pathshapes.h"
#include "mesh2.h"

//forward references
class Ellipsoid;

//extrude geometry is a factory for mesh geometry
class ExtrudeGeometry
{
private:
	void ExtrudeSidesFromRing(Mesh2& geom,Ellipsoid& e,bool isClockwise,const LinearRing& ring,float HeightMetres);
public:
	std::vector<PathShape> _shapes;

	ExtrudeGeometry(void);
	~ExtrudeGeometry(void);

	//add shape containing hole
	//from Mesh2?
	//from GeoJSON?
	//create mesh or extrude method - takes spheroid and height to extrude by

	void AddShape(const PathShape& Shape);
	void ExtrudeMesh(Mesh2& geom, Ellipsoid& e, float HeightMetres);
};

