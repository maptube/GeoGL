
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
		_fov = 60.0f*glm::pi<float>()/180.0f;
	}

	Camera::~Camera(void)
	{
	}

	/// <summary>
	/// Returns OpenGL viewport values. Origin is bottom left, so mouse coordinates in the window need to be converted
	/// using WinY=viewport[3]-mouseY
	/// TODO: maybe viewport should come from the graphics context (could even be static)?
	/// </summary>
	/// <returns>The OpenGL viewport in the four elements of a vec4 as min x,min y,max x,max y</returns>
	glm::vec4 Camera::GetViewport()
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		return glm::vec4((float)viewport[0],(float)viewport[1],(float)viewport[2],(float)viewport[3]);
	}

	/**
	* Initialise perspective camera
	*/
	void Camera::SetupPerspective(int windowWidth, int windowHeight, float nearClip, float farClip)
	{
		_near=nearClip; _far=farClip;
		_width=(float)windowWidth; _height=(float)windowHeight;
		//near=0.1f, far=100.0f, also need to pass in field of view angle which is fixed to 60 degrees
		projectionMatrix = glm::perspective(_fov, (float)windowWidth / (float)windowHeight, nearClip, farClip);  // Create our perspective matrix
	}

} //namespace gengine
