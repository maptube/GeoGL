
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
/// </summary>
/// <param name="isClockwise">True if points in ring are going clockwise (outer? with inner opposite winding).</param>
/// <param name="ring">The 2D ring (lat/lon) to extrude the points upwards from</param>
/// <param name="geom">The mesh object which the new geometry is added to</param>
//You could put the height into the z coord of the linear ring and and extrude each surface point by a different amount - not required at the moment
void ExtrudeGeometry::ExtrudeSidesFromRing(Mesh2& geom,Ellipsoid& e,bool isClockwise,const LinearRing& ring,float HeightMetres)
{
	glm::vec3 green(0.0,1.0,0.0);

	glm::dvec3 SP0=ring.front(); //need to keep the spherical lat/lon coords and the cartesian coords
	glm::vec3 P0 = e.toVector(glm::radians(SP0.x),glm::radians(SP0.y),0);
	for (vector<glm::dvec3>::const_iterator it = ring.begin(); it!=ring.end(); ++it) {
		glm::dvec3 SP1=*it;
		glm::vec3 P1 = e.toVector(glm::radians(SP1.x),glm::radians(SP1.y),0);
		//is this an epsilon check?
		if (P0!=P1) //OK, so skipping the first point like this isn't great programming
		{
			glm::vec3 P2 = e.toVector(glm::radians(SP1.x),glm::radians(SP1.y),HeightMetres);
			glm::vec3 P3 = e.toVector(glm::radians(SP0.x),glm::radians(SP0.y),HeightMetres);
			if (isClockwise)
			{
				geom.AddFace(P1,P0,P3,green,green,green);
				geom.AddFace(P1,P3,P2,green,green,green);
			}
			else {
				geom.AddFace(P3,P0,P1,green,green,green);
				geom.AddFace(P2,P3,P1,green,green,green);
			}
		}
		SP0=SP1;
		P0=P1;
	}
	//TODO: add last to first here - I don't think you need to do this?
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
		ExtrudeSidesFromRing(geom,e,true,shape.outer,HeightMetres);

		//inner faces from the holes in the outer shape
		for (vector<LinearRing>::iterator itRings = shape.inner.begin(); itRings!=shape.inner.end(); ++itRings) {
			LinearRing ring = *itRings;
			ExtrudeSidesFromRing(geom,e,true,ring,HeightMetres); //inner rings should be wound the opposite way, but this is the same as the outer
		}

		//build top and triangulate (2D), lifting the shape when converting onto the ellipsoid
		tri.Triangulate(); //triangulate data from tri.SetShape() into geom

		//now read the triangles back and go through all the triangles to make the faces
		
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
			geom.AddFace(P1,P2,P3,red,red,red); //uses x-order uniqueness of point, so not best efficiency
		}

	}
}
