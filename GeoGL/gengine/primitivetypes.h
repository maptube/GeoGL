#pragma once

#include "main.h"

namespace gengine {

//wrapper for gl draw primitive types
enum PrimitiveType {
	ptPoints=GL_POINTS,
	ptLineStrip=GL_LINE_STRIP,
	ptLineLoop=GL_LINE_LOOP,
	ptLines=GL_LINES,
	ptPolygon=GL_POLYGON,
	ptTriangleStrip=GL_TRIANGLE_STRIP,
	ptTriangleFan=GL_TRIANGLE_FAN,
	ptTriangles=GL_TRIANGLES,
	ptQuadStrip=GL_QUAD_STRIP,
	ptQuads=GL_QUADS,
	ptLinesAdjacency=GL_LINES_ADJACENCY,
	ptLineStripAdjacency=GL_LINE_STRIP_ADJACENCY,
	ptTrianglesAdjacency=GL_TRIANGLES_ADJACENCY,
	ptTriangleStripAdjacency=GL_TRIANGLE_STRIP_ADJACENCY
};

}