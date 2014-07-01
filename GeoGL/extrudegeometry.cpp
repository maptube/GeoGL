
#include "extrudegeometry.h"

#include <vector>

#include "pathshapes.h"
#include "mesh2.h"
#include "triangulator.h"
#include "ellipsoid.h"

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
/// Extrude geometry above surface.
/// Uses the _shapes property for the input data (vector of path shapes) and outputs to the Mesh2 geom parameter
/// TODO: add a degrees split for LOD and curvature
/// </summary>
/// <param name="geom">The mesh to put the extruded geometry into</param>
/// <param name="e">The ellipsoid that these points sit on so that we can extrude correctly</param>
/// <param name="HeightMetres">The height to extrude by</param>
void ExtrudeGeometry::ExtrudeMesh(Mesh2& geom, Ellipsoid& e, float HeightMetres)
{
	glm::vec3 red(1.0,0.0,0.0);
	glm::vec3 green(0.0,1.0,0.0);

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
		glm::vec3 SP0=shape.outer.front(); //need to keep the spherical lat/lon coords and the cartesian coords
		glm::vec3 P0 = e.toVector(glm::radians(SP0.x),glm::radians(SP0.y),0);
		for (vector<glm::vec3>::iterator it = shape.outer.begin(); it!=shape.outer.end(); ++it) {
			glm::vec3 SP1=*it;
			glm::vec3 P1 = e.toVector(glm::radians(SP1.x),glm::radians(SP1.y),0);
			//is this an epsilon check?
			if (P0!=P1) //OK, so skipping the first point like this isn't great programming
			{
				glm::vec3 P2 = e.toVector(glm::radians(SP1.x),glm::radians(SP1.y),HeightMetres);
				glm::vec3 P3 = e.toVector(glm::radians(SP0.x),glm::radians(SP0.y),HeightMetres);
				geom.AddFace(P1,P0,P3,green,green,green);
				geom.AddFace(P1,P3,P2,green,green,green);
			}
			SP0=SP1;
			P0=P1;
		}
		//TODO: add last to first here

		//TODO: inner faces going the other way

		//build top and triangulate (2D), lifting the shape when converting onto the ellipsoid
		tri.Triangulate(); //triangulate data from tri.SetShape() into geom
		//now read the triangles back
		glm::vec3 Col(1.0,0.0,0.0); //red
		//now go through all the triangles and make the faces
		
		//can't get index from tri, so going to build face using points
		vector<p2t::Triangle*> triangles = tri.GetTriangles();
		for (vector<p2t::Triangle*>::iterator trIT = triangles.begin(); trIT!=triangles.end(); ++trIT ) {
			p2t::Point* v1 = (*trIT)->GetPoint(0);
			p2t::Point* v2 = (*trIT)->GetPoint(1);
			p2t::Point* v3 = (*trIT)->GetPoint(2);
			//todo: the height goes in here!
			glm::vec3 P1 = e.toVector(glm::radians(v1->x),glm::radians(v1->y),HeightMetres); //don't forget to convert to radians for the ellipse!
			glm::vec3 P2 = e.toVector(glm::radians(v2->x),glm::radians(v2->y),HeightMetres);
			glm::vec3 P3 = e.toVector(glm::radians(v3->x),glm::radians(v3->y),HeightMetres);
			geom.AddFace(P1,P2,P3,Col,Col,Col); //uses x-order uniqueness of point, so not best efficiency
		}

	}
}
