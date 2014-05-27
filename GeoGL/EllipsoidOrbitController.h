#pragma once

#include "gengine/Camera.h"
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

	gengine::Camera *con_camera; //the camera being controlled
	//speed etc?
	//radius, it's the camera distance from origin?
	glm::dvec3 centre; //in world coordinates (camera space)
	Globe *globe; //debug only

	//EllipsoidOrbitController(void);
	EllipsoidOrbitController(gengine::Camera *camera, Ellipsoid* pEllipsoid);
	~EllipsoidOrbitController(void);

protected:
	bool dragging; //true when left mouse is dragging camera direction around orbit
	bool panning; //true when right mouse is panning the view left/right/up/down
	glm::dvec3 dragPoint; //point on sphere (ellipsoid) that is being dragged

	//event callbacks
	virtual void CursorPosCallback(GLFWwindow *window, double mx, double my);
	virtual void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	virtual void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};

