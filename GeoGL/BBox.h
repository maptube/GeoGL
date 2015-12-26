#pragma once
#include "main.h"

//TODO: need to write the bounding radius code!!!!!

/// <summary>
/// Bounding box class used by 3D objects
/// </summary>
class BBox
{
protected:
	double radius; //bounding radius from origin
public:
	bool IsValid;
	glm::dvec3 min; //bounding box
	glm::dvec3 max;

	BBox(void);
	~BBox(void);
	glm::dvec3 Centre();
	double Radius();
	void ExpandToIncludePoint(double x,double y,double z);
	void ExpandToIncludePoint(glm::dvec3 P);
	void ExpandToIncludePoint(glm::vec3 P);
	void Union(BBox& box);
};

