#include "BBox.h"


BBox::BBox(void)
{
	IsValid=false;
	radius=0;
}


BBox::~BBox(void)
{
}

/// <summary>
/// Return the centre of the bounding box
/// </summary>
glm::dvec3 BBox::Centre()
{
	glm::dvec3 C = min+max;
	C.x/=2; C.y/=2; C.z/=2;
	return C;
}

/// <summary>
/// return radius from centre
/// TODO: could pre-compute this
/// </summary>
/// <returns>The radius based on the maximum box dimension</returns>
double BBox::Radius()
{
	//This is wrong - you need to check each point as it's added
	//the centre is always half way between the min and max, so return bounding radius enclosing maximum dimension length
	//glm::dvec3 C = min+max;
	//C.x/=2; C.y/=2; C.z/=2; //work out centre of box
	//glm::dvec3 L = max-C; //work out distance from centre of box to maximum dimensions
	//return glm::sqrt(L.x*L.x+L.y*L.y+L.z*L.z);
	return 0;
}

/// <summary>
/// Add a new point and expand the BBox if necessary
/// </summary>
void BBox::ExpandToIncludePoint(double x,double y,double z)
{
	if (!IsValid) {
		IsValid=true;
		min=glm::vec3(x,y,z);
		max=glm::vec3(x,y,z);
	}
	else {
		if (x<min.x) min.x=x;
		if (y<min.y) min.y=y;
		if (z<min.z) min.z=z;
		if (x>max.x) max.x=x;
		if (y>max.y) max.y=y;
		if (z>max.z) max.z=z;
	}
}

/// <summary>
/// Overload of other ExpandToIncludePoint function
/// </summary>
void BBox::ExpandToIncludePoint(glm::vec3 P)
{
	ExpandToIncludePoint((float)P.x,(float)P.y,(float)P.z);
}

/// <summary>
/// Overload of other ExpandToIncludePoint function
/// </summary>
void BBox::ExpandToIncludePoint(glm::dvec3 P)
{
	ExpandToIncludePoint(P.x,P.y,P.z);
}

/// <summary>
/// Union operation, expands this bbox to include the parameter bbox
/// </summary>
void BBox::Union(BBox& box)
{
	ExpandToIncludePoint(box.min);
	ExpandToIncludePoint(box.max);
	//TODO: how do you calculate the new radius?
}
