#pragma once
#include "main.h"

/**
* Bounding box class used by 3D objects
*/
class BBox
{
public:
	bool IsValid;
	glm::vec3 min;
	glm::vec3 max;

	BBox(void);
	~BBox(void);
	glm::vec3 Centre();
	void ExpandToIncludePoint(float x,float y,float z);
	void ExpandToIncludePoint(glm::vec3 P);
	void Union(BBox& box);
};

