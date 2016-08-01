/*
 * envelope2d.cpp
 *
 *  Created on: 28 Dec 2015
 *      Author: richard
 */

#include "envelope2d.h"

namespace gis {

//constructors defined in header file
//Envelope2D::Envelope2D() {
//	// TODO Auto-generated constructor stub
//
//}

Envelope2D::~Envelope2D() {
	// TODO Auto-generated destructor stub
}

/// <summary>
/// Return the centre of the bounding box
/// </summary>
glm::dvec2 Envelope2D::Centre()
{
	glm::dvec2 C = min+max;
	C.x/=2; C.y/=2;
	return C;
}

/// <summary>
/// Add a new point and expand the BBox if necessary
/// </summary>
void Envelope2D::ExpandToIncludePoint(double x,double y)
{
	if (!IsValid) {
		IsValid=true;
		min=glm::vec2(x,y);
		max=glm::vec2(x,y);
	}
	else {
		if (x<min.x) min.x=x;
		if (y<min.y) min.y=y;
		if (x>max.x) max.x=x;
		if (y>max.y) max.y=y;
	}
}

/// <summary>
/// Overload of other ExpandToIncludePoint function
/// </summary>
void Envelope2D::ExpandToIncludePoint(glm::vec2 P)
{
	ExpandToIncludePoint((float)P.x,(float)P.y);
}

/// <summary>
/// Overload of other ExpandToIncludePoint function
/// </summary>
void Envelope2D::ExpandToIncludePoint(glm::dvec2 P)
{
	ExpandToIncludePoint(P.x,P.y);
}

/// <summary>
/// Union operation, expands this bbox to include the parameter bbox
/// </summary>
void Envelope2D::Union(gis::Envelope2D& env)
{
	ExpandToIncludePoint(env.min);
	ExpandToIncludePoint(env.max);
}


} //namepsace gis
