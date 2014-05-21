#pragma once

#include "OrbitController.h"

//forward declarations
namespace gengine {
	class Camera;
}

class Ellipsoid;


class EllipsoidOrbitController : public OrbitController
{
public:
	Ellipsoid* _pEllipsoid;

	//EllipsoidOrbitController(void);
	EllipsoidOrbitController(gengine::Camera *camera, Ellipsoid* pEllipsoid);
	~EllipsoidOrbitController(void);

protected:
	//event callbacks
	virtual void CursorPosCallback(GLFWwindow *window, double mx, double my);
	virtual void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	//virtual void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};

