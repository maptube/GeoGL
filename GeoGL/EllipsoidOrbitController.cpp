
#include "EllipsoidOrbitController.h"
#include "ellipsoid.h"
#include "gengine/Camera.h"

//TODO: VERY IMPORTANT
//When this is working, it might be a good idea to separate it from the orbit controller so it's not a sub class

//EllipsoidOrbitController::EllipsoidOrbitController(void)
//{
//}

EllipsoidOrbitController::EllipsoidOrbitController(gengine::Camera* camera, Ellipsoid* pEllipsoid) : OrbitController(camera)
{
	_pEllipsoid = pEllipsoid;
}


EllipsoidOrbitController::~EllipsoidOrbitController(void)
{
}

/// <summary>
/// </summary>
/// <param name="window">Window handle of the GLFW window, used in any GLFW library calls</param>
/// <param name="mx">Mouse X in window pixel coordinates (origin top left)</param>
/// <param name="my">Mouse Y in window pixel coordinates (origin top left)</param>
void EllipsoidOrbitController::CursorPosCallback(GLFWwindow *window, double mx, double my)
{
	//This is the basic idea:
	//You click on a point and drag, so you have a drag point (where you clicked) and a current mouse point where you're dragging the clicked point to.
	//Unproject both points to a ray and intersect with the ellipsoid so you know the points on the surface that are being dragged.
	//Find Euler angles between the two points and rotate camera so that ellipsoid drag point moves to where the current mouse point now is.

	//std::cout<<"Ellipse Orbiting...."<<mx<<" "<<my<<std::endl;
	if (dragging) {
		//int width,height;
		//glfwGetFramebufferSize(window, &width, &height);
		//glm::vec4 vViewport(0,0,width,height);

		//THIS SHOULD NOT BE HERE!
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		float winX = mx;
		float winY = viewport[3]-my;
		float winZ;
		glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ); //read depth buffer at required point (won't work! although it will result in dragging around what you clicked on)
		
		//glm::vec4 vViewport((float)viewport[0],(float)viewport[1],(float)viewport[2],(float)viewport[3]);
		//glm::vec3 P1 = glm::unProject(glm::vec3(dragPoint.x,dragPoint.y,1.0),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject click point
		//glm::vec3 P2 = glm::unProject(glm::vec3((double)mx,(double)my,1.0),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject current mouse point
		//now find intersections with ellipsoid
		//NO, skipped this as using z coord from buffer - alternative method would be to calculate my own two rays from above and do the ellipsoid intersect test

		//new bit
		//float winX = mx;
		//float winY = viewport[3]-my;
		//glm::vec4 vViewport((float)viewport[0],(float)viewport[1],(float)viewport[2],(float)viewport[3]);
		//glm::vec3 P1 = glm::unProject(glm::vec3(dragPoint.x,dragPoint.y,1.0),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject click point
		//glm::vec3 P2 = glm::unProject(glm::vec3(winX,winY,1.0),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject current mouse point
		//now work out rotation between P1 and P2

		//delta rotation method
		//get distance from camera to object origin, which is the radius of the rotating sphere that the camera is fixed to
		glm::vec3 vCameraPos = con_camera->GetCameraPos();
		double radius = glm::distance(centre,vCameraPos);
		glm::vec4 vViewport((float)viewport[0],(float)viewport[1],(float)viewport[2],(float)viewport[3]);
		glm::vec3 P1 = glm::unProject(glm::vec3(dragPoint.x,dragPoint.y,0.5),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject click point
		glm::vec3 P2 = glm::unProject(glm::vec3((double)mx,(double)my,(double)winZ),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject current mouse point
		std::cout<<"click point="<<P1.x<<","<<P1.y<<","<<P1.z<<std::endl;
		std::cout<<"current point="<<P2.x<<","<<P2.y<<","<<P2.z<<std::endl;
		//test - project and unproject the same vector
		//glm::vec3 test1 = glm::project(glm::vec3(100,100,300),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport);
		//glm::vec3 test2 = glm::unProject(test1,con_camera->viewMatrix,con_camera->projectionMatrix,vViewport);


		glm::vec3 delta=P2-P1;
		//delta.z=-radius;

		//GLSL asin undefined for |x|>1, so use atan2 for Phi and Theta instead
		//double hy=glm::sqrt(radius*radius-delta.y*delta.y);
		//double hy=glm::sqrt(delta.z*delta.z-delta.y*delta.y);
		//double Phi = glm::atan<double>(delta.y,hy);
		//double Theta = glm::atan<double>(delta.x,delta.z);
		double Phi=0.0; double Theta=0.0;
		std::cout<<"Phi="<<Phi<<" Theta="<<Theta<<" delta.x="<<delta.x<<" delta.y="<<delta.y<<" delta.z="<<delta.z<<std::endl;

		glm::mat4 m = glm::eulerAngleYX((float)Theta,(float)Phi);
		glm::mat4 mCam = dragCameraMatrix;
		//move camera so it's centred on the origin of rotation (this.centre)
		//don't do this: mCam = glm::translate(mCam,-centre) as the translate code performs
		// mCam[3]=m[0]*v[0]+m[1]*v[1]+m[2]*v[2]+m[3] i.e. translation v along main direction of martix m
		mCam[3][0]-=centre.x; mCam[3][1]-=centre.y; mCam[3][2]-=centre.z; //move mCam to (centre) in world space
		//perform rotation of camera by Euler angles. Camera is radius units from world origin to rotate about correct point
		mCam = m * mCam;
		//move (not translate) camera back to world space centre again (add what we took off previously)
		mCam[3][0]+=centre.x; mCam[3][1]+=centre.y; mCam[3][2]+=centre.z;
		con_camera->SetCameraMatrix(mCam); //sets viewMatrix=inverse(cameraMatrix)
	}
}

/// <summary>
/// Use mouse wheel to alter the camera distance from the centre (the radius of the orbit).
/// This version is similar to the OrbitCamera implementation, but calculates the height above the ellipsoid so you can't go through the surface and also scales intelligently
/// as you get close to the surface where the data is.
/// </summary>
/// <param name="window"></param>
/// <param name="xoffset"></param>
/// <param name="yoffset">The mouse wheeel button delta to scroll up or down (or zoom in and out in orbit camera sense)</param>
void EllipsoidOrbitController::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	const double speed = 0.01;

	//std::cout<<"OrbitController::ScrollCallback xoffset="<<xoffset<<" yoffset="<<yoffset<<std::endl;

	//complex version where it zooms in a percentage of the distance from the eye to the centre
	glm::vec3 vCameraPos = con_camera->GetCameraPos();
	glm::mat4 mCamera = con_camera->GetCameraMatrix();
	//old code
	//float radius = glm::distance(centre,vCameraPos);
	//float delta = -radius*yoffset*speed; //where speed is the percentage i.e. 1/100=0.01
	//new code - diffferent radius (now height)
	double h = _pEllipsoid->heightAboveSurfaceAtPoint(vCameraPos);
	std::cout<<"h="<<h<<std::endl;
	float delta = -h*yoffset*speed; //where speed is the percentage i.e. 1/100=0.01
	glm::mat4 mNewCamera = glm::translate(mCamera,glm::vec3(0,0,delta));
	con_camera->SetCameraMatrix(mNewCamera);

	//simple version where we zoom in a fixed amount - this works better
	//glm::mat4 mCamera = con_camera->GetCameraMatrix();
	//float delta = speed*yoffset;
	//mCamera = glm::translate(mCamera,glm::vec3(0,0,-delta));
	//con_camera->SetCameraMatrix(mCamera);
}
