#pragma once
/*

 * envelope2d.h
 *
 *  Created on: 28 Dec 2015
 *      Author: richard
 *  Also see bbox class for 3D version from a computer graphics bounding box viewpoint.
 *  This is a geographic implementation based on the GeoAPI.
 */

//needed for glm
#include "gengine/gengine.h"

namespace gis {

class Envelope2D {
public:
	bool IsValid;
	glm::dvec2 min; //bounding box
	glm::dvec2 max;
public:
	Envelope2D() : IsValid(false) {};
	Envelope2D(double xmin,double xmax,double ymin,double ymax) : min(glm::dvec2(xmin,ymin)), max(glm::dvec2(xmax,ymax)), IsValid(true) {};
	virtual ~Envelope2D();

	glm::dvec2 Centre();
	double width() { if (!IsValid) return 0; return max.x-min.x; }
	double height() { if (!IsValid) return 0; return max.y-min.y; }
	void ExpandToIncludePoint(double x,double y);
	void ExpandToIncludePoint(glm::vec2 P);
	void ExpandToIncludePoint(glm::dvec2 P);
	void Union(gis::Envelope2D& env);
	bool Contains(glm::dvec2 P) { return IsValid && ((P.x>=min.x)&&(P.x<max.x)&&(P.y>=min.y)&&(P.y<max.y)); }
};

} //namespace gis
