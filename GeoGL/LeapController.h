#pragma once
#include "main.h"
#include "gengine/Camera.h"
//#include "EventListener.h"
#include "ellipsoid.h"

#include "Leap.h"

/// <summary>
/// Basically a fly controller that is controlled by a Leap Motion sensor
/// </summary>
class LeapController : public Leap::Listener
{
public:
	static const double rateRoll; //rates applied to cursor position for aileron
	static const double ratePitch; //rates applied to cursor position for elevator
	static const double rateYaw; // rates applied to cursor position for rudder

	Leap::Controller* controller;
	gengine::Camera *con_camera; //the camera being controlled
	GLFWwindow *con_window; //window to inject mouse events into
	//float Aileron, Elevator; //angles of rotation in radians
	//float Speed; //distance moved forward each time Forward() is called
	double Roll, Pitch, Yaw; //angles of rotation from LeapMotion device

	LeapController(gengine::Camera *camera, GLFWwindow* window);
	~LeapController(void);

	//Leap methods (see sample.cpp in the Leap SDK samples)
	virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onExit(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    virtual void onFocusGained(const Leap::Controller&);
    virtual void onFocusLost(const Leap::Controller&);
	//end of Leap methods

	//Controller navigation
	//TODO: should really pass the speed or time in here
	void Forward();
	void Backward();
	void SimulateMultiRotor();
	void GeoNavigate(const Leap::Controller& controller);
protected:
	//event callbacks
	//virtual void CursorPosCallback(GLFWwindow *window, double mx, double my);
	//virtual void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	//virtual void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	void testPoint(const Leap::Controller& controller);
	void EllipsoidDistance(Ellipsoid& e,const float LeapY);
};

