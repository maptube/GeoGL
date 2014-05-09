#pragma once
#include "main.h"
#include "gengine/Camera.h"
#include "gengine/events/EventListener.h"

/*
* Controller class for spinning a camera around the model using the mouse or a gesture controller
*/
class OrbitController : public gengine::events::EventListener
{
public:
	gengine::Camera *con_camera; //the camera being controlled
	//speed etc?
	//radius, it's the camera distance from origin?
	glm::vec3 centre; //in world coordinates (camera space)

	OrbitController(gengine::Camera *camera);
	~OrbitController(void);

	//neither of these is strictly necessary as the camera can be moved directly and the origin is public
	void SetCameraPos(glm::vec3 pos)
	{
		//NOTE: this is in world coordinates i.e. camera space
		//direct manipulation of GLSL matrix, but can't see any other way of doing this in GLM
		//con_camera->viewMatrix[3][0]=pos.x;
		//con_camera->viewMatrix[3][1]=pos.y;
		//con_camera->viewMatrix[3][2]=pos.z;
		con_camera->SetCameraPos(pos);
	};
	void SetCentre(glm::vec3 C)
	{
		//world coordinates
		centre = C;
	};
protected:
	bool dragging; //true when left mouse is dragging camera direction around orbit
	bool panning; //true when right mouse is panning the view left/right/up/down
	glm::vec3 dragPoint; //point on sphere that is being dragged
	glm::mat4 dragCameraMatrix; //camera matrix at the point when the drag began
	glm::mat4 dragViewMatrix; //view matrix at the point when the drag began (one of these two is going to be useful)

	//event callbacks
	virtual void CursorPosCallback(GLFWwindow *window, double mx, double my);
	virtual void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	virtual void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};

