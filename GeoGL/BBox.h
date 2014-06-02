#pragma once
#include "main.h"

/// <summary>
/// Bounding box class used by 3D objects
/// </summary>
class BBox
{
public:
	bool IsValid;
	glm::dvec3 min;
	glm::dvec3 max;

	BBox(void);
	~BBox(void);
	glm::dvec3 Centre();
	void ExpandToIncludePoint(double x,double y,double z);
	void ExpandToIncludePoint(glm::dvec3 P);
	void ExpandToIncludePoint(glm::vec3 P);
	void Union(BBox& box);
};

