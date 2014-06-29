
#include "extrudegeometry.h"

#include <vector>

#include "pathshapes.h"
#include "mesh2.h"
#include "triangulator.h"

using namespace std;

ExtrudeGeometry::ExtrudeGeometry(void)
{
}


ExtrudeGeometry::~ExtrudeGeometry(void)
{
}

/// <summary>
/// Add a shape containing an outer ring and one or more inner rings.
/// Coordinates are geographic in wgs84.
/// </summary>
void ExtrudeGeometry::AddShape(const PathShape& Shape)
{
	_shapes.push_back(Shape);
}

/// <summary>
/// Extrude geometry above surface
/// TODO: add a degrees split for LOD and curvature
/// </summary>
Mesh2* ExtrudeGeometry::ExtrudeMesh(float HeightMetres)
{
	glm::vec3 red(1.0,0.0,0.0);

	Mesh2* mesh = new Mesh2();
	Triangulator tri;
	for (vector<PathShape>::iterator it = _shapes.begin(); it!=_shapes.end(); ++it)
	{
		PathShape shape = *it;
		tri.Clear();
		tri.SetShape(shape);

		//build sides
		//This is done by walking the outer and inner rings, checking for duplicate points
		//as you go and building the facade. First point might be the same as the last, so
		//we have to check for this.
		glm::vec3 P0 = shape.outer.front();
		for (vector<glm::vec3>::iterator it = shape.outer.begin(); it!=shape.outer.end(); ++it) {
			glm::vec3 P1 = *it;
			//is this an epsilon check?
			if (P0!=P1) //OK, so skipping the first point like this isn't great programming
			{
				glm::vec3 P2(P1.x,P1.y+HeightMetres,P1.z), P3(P0.x,P0.y+HeightMetres,P0.z);
				mesh->AddFace(P1,P0,P3,red,red,red);
				mesh->AddFace(P1,P3,P2,red,red,red);
			}
			P0=P1;
		}
		//TODO: add last to first here

		//TODO: inner faces going the other way

		//build top and triangulate
		tri.Triangulate(*mesh);
	}

	return mesh;
}
