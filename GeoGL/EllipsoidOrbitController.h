#pragma once

#include "gengine/events/EventListener.h"

//forward declarations
namespace gengine {
	class Camera;
}

class Ellipsoid;
class Globe; //debug only

class EllipsoidOrbitController : public gengine::events::EventListener
{
public:
	Ellipsoid* _pEllipsoid; //the ellipsoid that we're clicking on the surface of
	float scrollSpeed; //scale factor for zooming in - bigger is faster
	float rollSpeed; //scale factor for rolling using the right mouse button (aileron)
	float pitchSpeed; //scale factor for pitching up and down using the right mouse button (elevator)

	gengine::Camera *con_camera; //the camera being controlled
	//speed etc?
	//radius, it's the camera distance from origin?
	glm::dvec3 centre; //in world coordinates (camera space)
	Globe *globe; //debug only

	EllipsoidOrbitController(gengine::Camera *camera, Ellipsoid* pEllipsoid);
	~EllipsoidOrbitController(void);

protected:
	bool dragging; //true when left mouse is dragging camera direction around orbit
	bool looking; //true when right mouse is rotating the view left/right/up/down
	glm::dvec3 dragPoint; //point on sphere (ellipsoid) that is being dragged
	glm::dvec2 lookPoint; //point on screen that we are using as a linear rotation point
	glm::dmat4 lookCameraMatrix; //camera matrix at the point when the look started

	//event callbacks
	virtual void CursorPosCallback(GLFWwindow *window, double mx, double my);
	virtual void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	virtual void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};

