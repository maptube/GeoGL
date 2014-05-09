#pragma once
#include "main.h"
#include "gengine/Camera.h"
#include "gengine/events/EventListener.h"

/// <summary>
/// Controller class for camera which you can fly around the model using the mouse or a gesture controller
/// </summary>
class FlyController : public gengine::events::EventListener
{
public:
	static const float rateX; //rates applied to cursor position for aileron
	static const float rateY; //rates applied to cursor position for elevator

	gengine::Camera *con_camera; //the camera being controlled
	float Aileron, Elevator; //angles of rotation in radians
	float Speed; //distance moved forward each time Forward() is called

	FlyController(gengine::Camera *camera);
	~FlyController(void);

	void Forward();
protected:
	//event callbacks
	virtual void CursorPosCallback(GLFWwindow *window, double mx, double my);
	virtual void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	virtual void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};

