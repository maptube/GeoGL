
#include "Camera.h"

namespace gengine {

	/**
	* Camera - simple camera class
	* Camera which looks at a fixed point in space and allows the user to rotate and translate the model.
	* The viewMatrix is the inverse of the cameraMatrix. Camera matrix is the position and rotation of the camera as if you were
	* going to draw it in the scene. The view matrix is the transform which is applied to the world to transform it into the view
	* from the camera. In other words the camera matrix is the inverse of the view matrix.
	* Hook into mouse events and use them to control a viewpoint.
	* THREE.js uses Camera and Controls objects where controls moves the camera.
	*/
	Camera::Camera(void)
	{
		viewMatrix = glm::dmat4(1);
		projectionMatrix = glm::dmat4(1);
	}

	Camera::~Camera(void)
	{
	}

	/**
	* Initialise perspective camera
	*/
	void Camera::SetupPerspective(int windowWidth, int windowHeight, float nearClip, float farClip)
	{
		//near=0.1f, far=100.0f, also need to pass in field of view angle which is fixed to 60 degrees
		projectionMatrix = glm::perspective(60.0f*glm::pi<float>()/180.0f, (float)windowWidth / (float)windowHeight, nearClip, farClip);  // Create our perspective matrix
	}

} //namespace gengine
