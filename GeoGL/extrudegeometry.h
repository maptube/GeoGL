#pragma once

#include <vector>

#include "pathshapes.h"
#include "mesh2.h"


//extrude geometry is a factory for mesh geometry
class ExtrudeGeometry
{
public:
	std::vector<PathShape> _shapes;

	ExtrudeGeometry(void);
	~ExtrudeGeometry(void);

	//add shape containing hole
	//from Mesh2?
	//from GeoJSON?
	//create mesh or extrude method - takes spheroid and height to extrude by

	void AddShape(const PathShape& Shape);
	Mesh2* ExtrudeMesh(float HeightMetres);
};

