#include "BBox.h"


BBox::BBox(void)
{
	IsValid=false;
}


BBox::~BBox(void)
{
}

/**
* Return the centre of the bounding box
*/
glm::vec3 BBox::Centre()
{
	glm::vec3 C = min+max;
	C.x/=2; C.y/=2; C.z/=2;
	return C;
}

/**
* Add a new point and expand the BBox if necessary
*/
void BBox::ExpandToIncludePoint(float x,float y,float z)
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

/**
* Overload of other ExpandToIncludePoint function
*/
void BBox::ExpandToIncludePoint(glm::vec3 P)
{
	ExpandToIncludePoint(P.x,P.y,P.z);
}

/**
* Union operation, expands this bbox to include the parameter bbox
*/
void BBox::Union(BBox& box)
{
	ExpandToIncludePoint(box.min);
	ExpandToIncludePoint(box.max);
}
