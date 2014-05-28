
#include "EllipsoidOrbitController.h"
#include "ellipsoid.h"
#include "gengine/Camera.h"
#include "gengine/events/EventManager.h"

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "globe.h" //debug only

using namespace gengine;

using namespace events;


EllipsoidOrbitController::EllipsoidOrbitController(gengine::Camera* camera, Ellipsoid* pEllipsoid)
{
	_pEllipsoid = pEllipsoid;
	con_camera = camera;

	scrollSpeed = 0.05f; //speed when zooming in and out - bigger is faster
	dragging = false;
	panning = false; //TODO: there is no pan mode!

	//now hook up the mouse events
	EventManager& em=EventManager::getInstance();
	em.AddCursorPosEventListener(this);
	em.AddScrollEventListener(this);
	em.AddMouseButtonEventListener(this);
}


EllipsoidOrbitController::~EllipsoidOrbitController(void)
{
	//tidy up after ourselves by removing the static event callback hooks from the EventManager
	EventManager& em=EventManager::getInstance();
	em.RemoveCursorPosEventListener(this);
	em.RemoveScrollEventListener(this);
	em.RemoveMouseButtonEventListener(this);
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//TODO: this probably wants to be in a general library
//It's a ray intersect with ellipsoid test similar the to the ray sphere one in glm
bool intersectRayEllipsoid(
	glm::dvec3 rayStarting,
	glm::dvec3 rayNormalizedDirection,
	glm::dvec3 ellipsoidCentre,
	glm::dvec3 ellipsoidRadius, //vector of [a,b,c]
	glm::dvec3& intersectionPosition)
{
	//see: http://www.cs.oberlin.edu/~bob/cs357.08/VectorGeometry/VectorGeometry.pdf
	//V1=vM
	glm::dvec3 V1=rayNormalizedDirection;
	V1.x/=ellipsoidRadius.x; V1.y/=ellipsoidRadius.y; V1.z/=ellipsoidRadius.z;
	//P1=P0M-CM
	glm::dvec3 P1 = rayStarting-ellipsoidCentre;
	P1.x/=ellipsoidRadius.x; P1.y/=ellipsoidRadius.y; P1.z/=ellipsoidRadius.z; //M scaling by axis half widths
	//solve quadratic At^2 + Bt + C = 0
	double A=glm::dot(V1,V1); //|V1|^2
	double B=2.0 * glm::dot(P1,V1);
	double C=glm::dot(P1,P1)-1.0; //|P1|^2-1
	if ((B*B-4*A*C)<0) return false;
	double t0=(-B+sqrt(B*B-4*A*C))/(2*A);
	double t1=(-B-sqrt(B*B-4*A*C))/(2*A);
	if (t0<t1) {
		intersectionPosition = rayStarting + t0 * rayNormalizedDirection;
	}
	else {
		intersectionPosition = rayStarting + t1 * rayNormalizedDirection;
	}
	return true;
}

/// <summary>
/// Callback that follows the mouse position as the left mouse button is used to drag a point on the ellipsoid
/// to a new location on the screen.
/// </summary>
/// <param name="window">Window handle of the GLFW window, used in any GLFW library calls</param>
/// <param name="mx">Mouse X in window pixel coordinates (origin top left)</param>
/// <param name="my">Mouse Y in window pixel coordinates (origin top left)</param>
void EllipsoidOrbitController::CursorPosCallback(GLFWwindow *window, double mx, double my)
{
	//std::cout<<"Ellipse Orbiting...."<<mx<<" "<<my<<std::endl;
	if (dragging) {

		//get the viewport and convert mouse (mx,my) into opengl coordinates (origin bottom left, so Y swaps)
		glm::vec4 vViewport = con_camera->GetViewport();
		double winX = mx;
		double winY = ((double)vViewport[3])-my;

		//Here's the method for calculating the rotation:
		//P1 is the drag point, which was back projected and stored in the click handler which initiated the drag.
		//Back project P2 which is the current mouse cursor position that we're using to drag the sphere.
		//As these use Z in device coordinates, use Z=0.9999 to get a ray that hits the max Z location, then use
		//the intersection of the ray with a sphere to get a point in 3D world space on the surface of the sphere.
		//Subtract the orbit centre from these points and normalise.
		//Now the cross product gives a normal which can be used for a plane of rotation while the dot product gives
		//the angle between the two points on the plane.
		//These two pieces of information are used to rotate the camera.

		glm::dvec3 vCameraPos = con_camera->GetCameraPos();
		glm::dvec4 dvViewport(vViewport);
		//glm::dvec3 P1=dragPoint; //don't need this, but P1 is the dragPoint where the user clicked first
		glm::dvec3 P2 = glm::unProject(glm::dvec3(winX,winY,0.9999),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject current mouse point

		glm::dvec3 intersectionPosition, intersectionNormal;
		//bool test = glm::intersectRaySphere(
		//	vCameraPos, //rayStarting,
		//	glm::normalize(P2-vCameraPos), //rayNormalizedDirection,
		//	centre, //genType const &  sphereCenter,
		//	6378137.0/1000.0, //  sphereRadius,
		//	intersectionPosition,
		//	intersectionNormal
		//);
		bool test = intersectRayEllipsoid(
			vCameraPos,
			glm::normalize(P2-vCameraPos),
			centre,
			glm::dvec3(_pEllipsoid->A(),_pEllipsoid->B(),_pEllipsoid->C()),
			intersectionPosition
		);
		if (test) {
			//std::cout<<"Sphere intersect: "<<intersectionPosition.x<<","<<intersectionPosition.y<<","<<intersectionPosition.z<<std::endl;
			//std::cout<<"Sphere intersect N: "<<intersectionNormal.x<<","<<intersectionNormal.y<<","<<intersectionNormal.z<<std::endl;
			globe->debugPositionCube(2,intersectionPosition.x,intersectionPosition.y,intersectionPosition.z);

			P2=intersectionPosition; //we just worked this out
			//take two normalised vectors from orbit centre to points P1 and P2 on sphere
			glm::dvec3 NV1 = glm::normalize(dragPoint-centre);
			glm::dvec3 NV2 = glm::normalize(P2-centre);
			double CosTheta = glm::dot(NV1,NV2); //A.B = |A||B| Cos(Theta), but A and B are normalised. This is the rotation angle on the plane.
			//TODO: check whether acos is defined for |x|>1 ???????
			double Theta = glm::acos(CosTheta);
			//std::cout<<"Angle="<<Theta*180/3.1415<<std::endl;
			glm::dvec3 PlaneNormal = glm::normalize(glm::cross(NV2,NV1)); //Cross the two vectors to get the normal to the plane of rotation. Either -(1 x 2) or (2 x 1)
			
			//you can also work out the angle like this as a double check on theta:
			//double Theta2 = 2 * asin(glm::length(P2-dragPoint)/2.0/(6378137.0/1000.0));
			//std::cout<<"Angle="<<Theta*180/3.1415<<"  angle2="<<Theta2*180/3.1415<<std::endl;
		
			//OK, we have a plane of rotation and an angle, so let's rotate the camera
			glm::dmat4 mCam = con_camera->GetCameraMatrix();
			glm::dmat4 m = glm::rotate(glm::dmat4(1),Theta,PlaneNormal);
			mCam = m * mCam;
			con_camera->SetCameraMatrix(mCam);
			//std::cout<<"Camera: "<<vCameraPos.x<<" "<<vCameraPos.y<<" "<<vCameraPos.z<<" "<<glm::length(vCameraPos)<<std::endl;
		}
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
	//std::cout<<"OrbitController::ScrollCallback xoffset="<<xoffset<<" yoffset="<<yoffset<<std::endl;

	//complex version where it zooms in a percentage of the distance from the eye to the centre
	glm::dvec3 vCameraPos = con_camera->GetCameraPos();
	glm::dmat4 mCamera = con_camera->GetCameraMatrix();
	//old code
	//float radius = glm::distance(centre,vCameraPos);
	//float delta = -radius*yoffset*speed; //where speed is the percentage i.e. 1/100=0.01
	//new code - diffferent radius (now height)
	double h = _pEllipsoid->heightAboveSurfaceAtPoint(vCameraPos);
	std::cout<<"h="<<h<<std::endl;
	float delta = -h*yoffset*scrollSpeed; //where speed is the percentage i.e. 1/100=0.01
	glm::dmat4 mNewCamera = glm::translate(mCamera,glm::dvec3(0,0,delta));
	con_camera->SetCameraMatrix(mNewCamera);

	//simple version where we zoom in a fixed amount - this works better
	//glm::mat4 mCamera = con_camera->GetCameraMatrix();
	//float delta = speed*yoffset;
	//mCamera = glm::translate(mCamera,glm::vec3(0,0,-delta));
	//con_camera->SetCameraMatrix(mCamera);
}

void EllipsoidOrbitController::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	//std::cout<<"OrbitController::MouseButtonCallback"<<std::endl;
	if (button==GLFW_MOUSE_BUTTON_LEFT) {
		if (action==GLFW_PRESS) {
			dragging=true;
			//set click point
			double Px,Py;
			glfwGetCursorPos(window,&Px,&Py);
			glm::dvec3 vCameraPos = con_camera->GetCameraPos();
			
			glm::vec4 vViewport = con_camera->GetViewport();
			double winX = Px;
			double winY = (double)vViewport[3]-Py;
			glm::dvec4 dvViewport(vViewport);
			glm::dvec3 P1 = glm::unProject(glm::dvec3(winX,winY,0.9999),con_camera->viewMatrix,con_camera->projectionMatrix,vViewport); //unproject click point
			//find intersection on sphere
			glm::dvec3 intersectionPosition, intersectionNormal;
			//bool test = glm::intersectRaySphere(
			//	vCameraPos, //rayStarting,
			//	glm::normalize(P1-vCameraPos), //rayNormalizedDirection,
			//	centre, //genType const &  sphereCenter,
			//	6378137.0/1000.0, //  sphereRadius,
			//	intersectionPosition,
			//	intersectionNormal
			//);
			bool test = intersectRayEllipsoid(
				vCameraPos,
				glm::normalize(P1-vCameraPos),
				centre,
				glm::dvec3(_pEllipsoid->A(),_pEllipsoid->B(),_pEllipsoid->C()),
				intersectionPosition
				);
			if (!test) {
				//didn't click on the ellipsoid, so drop out of dragging mode
				dragging=false;
				return;
			}
			dragPoint = intersectionPosition;
			std::cout<<"Intersect: "<<intersectionPosition.x<<" "<<intersectionPosition.y<<" "<<intersectionPosition.z<<std::endl;
			globe->debugPositionCube(1,dragPoint.x,dragPoint.y,dragPoint.z);
			//if test==false then dragging=false? i.e. you haven't clicked on the sphere

			//std::cout<<"Drag Camera: "<<dragCameraMatrix[3][0]<<" "<<dragCameraMatrix[3][1]<<" "<<dragCameraMatrix[3][2]<<std::endl;
			//std::cout<<"Centre: "<<centre.x<<" "<<centre.y<<" "<<centre.z<<std::endl;
			//std::cout<<"Drag Point: "<<dragPoint.x<<" "<<dragPoint.y<<" "<<dragPoint.z<<" "<<glm::length(dragPoint)<<std::endl;
		}
		else { //GLFW_RELEASE
			dragging=false;
		}
	}
	else if (button==GLFW_MOUSE_BUTTON_RIGHT) {
		if (action==GLFW_PRESS) {
			panning=true;
			//set click point
			double Px,Py;
			glfwGetCursorPos(window,&Px,&Py);
			dragPoint = glm::dvec3(Px,Py,0);
		}
		else {
			panning=false;
		}
	}
}

