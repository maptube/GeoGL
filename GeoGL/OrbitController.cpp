
#include "OrbitController.h"
#include "gengine/events/EventManager.h"

using namespace gengine;

using namespace events;

/*
* OrbitController constructor
* @param camera The camera being controlled
*/
OrbitController::OrbitController(Camera *camera)
{
	con_camera = camera;
	dragging = false;
	panning = false;

	//now hook up the mouse events
	EventManager& em=EventManager::getInstance();
	em.AddCursorPosEventListener(this);
	em.AddScrollEventListener(this);
	em.AddMouseButtonEventListener(this);
}

/*
* OrbitController destructor
*
*/
OrbitController::~OrbitController(void)
{
	//tidy up after ourselves by removing the static event callback hooks from the EventManager
	EventManager& em=EventManager::getInstance();
	em.RemoveCursorPosEventListener(this);
	em.RemoveScrollEventListener(this);
	em.RemoveMouseButtonEventListener(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Event handling

/**
* CursorPosCallback
* Called when the mouse moves in the window.
* The orbit camera moves the camera around the surface of a sphere which is centred on the "origin" property. When the user
* clicks on a point in the viewport, a ray is projected from the screen through the origin, with the new camera positioned
* where this ray intersects the sphere. The camera always points towards the origin point.
* The camera radius can be changed using the mouse wheel.
*/
/*void OrbitController::CursorPosCallback(GLFWwindow *window, double mx, double my)
{
	//std::cout<<"OrbitController::CursorPosCallback"<<std::endl;
	int left_but = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
	if (left_but==GLFW_PRESS) {
	//if (dragging) {
		//std::cout<<"Orbiting...."<<mx<<" "<<my<<std::endl;

		int width,height;
		glfwGetFramebufferSize(window, &width, &height);

		//click point in 3d on near clip plane, Z=0.1
		//NOTE: you could pre-calculate this when the user clicks
		//glm::vec3 P1 = glm::unProject(glm::vec3(screenPoint.x,screenPoint.y,0.1),con_camera->viewMatrix,con_camera->projectionMatrix,glm::vec4(0,0,width,height));

		//get distance from camera to object origin, which is the radius of the rotating sphere that the camera is fixed to
		glm::vec3 vCameraPos = con_camera->GetPos();
		float radius = glm::distance(origin,vCameraPos);

		glm::mat4 m = con_camera->viewMatrix;
		//m[3][0]=dragPoint.x; m[3][1]=dragPoint.y; m[3][2]=dragPoint.z;
		//m[0][3]=1; m[1][3]=1; m[2][3]=1; //m[3][3]=0;
		//m[3][0]=0; m[3][1]=0; m[3][2]=0; m[3][3]=1;
		//drag point in 3d on near clip plane, 0.1=near clip plane, con_camera->viewMatrix is model view matrix below (i.e. view*model)
		glm::vec3 P2 = glm::unProject(glm::vec3(mx,my,vCameraPos.z),m,con_camera->projectionMatrix,glm::vec4(0,0,width,height));
		//P2 is our 3D point projected from the click position on the 2D plane onto the near clip plane in 3D
		//std::cout<<"P2: "<<P2.x<<" "<<P2.y<<" "<<P2.z<<std::endl;
		//P2.z=sqrt(radius*radius-P2.x*P2.x-P2.y*P2.y); //put P2 on the surface of the sphere

		//glm::vec3 deltaP = P2-dragPoint;
		//float dx = P2.x-P1.x, dy=P2.y-P1.y;

		//float Phi = acos(deltaP.y/radius);
		//calculate Theta on base triangle using hypot calculated from sqrt(radius^2 - dy^2) on Phi triangle
		//float hy=sqrt(radius*radius-deltaP.y*deltaP.y);
		//float Theta = asin(deltaP.x/hy);

		//find point radius units along the vector O->P2 which is the new position of the camera
		glm::vec3 v=P2-origin;
		//normalise to radius units
		float l = glm::length(v);
		v.x=v.x*radius/l;
		v.y=v.y*radius/l;
		v.z=v.z*radius/l;
		
		//move camera to new position
		con_camera->SetPos(origin+v);

		//and finally, don't forget to point the camera towards the object that we're orbiting
		//con_camera->LookAt(origin); //doesn't work - which way is up?
		float w = con_camera->viewMatrix[3][3];
		glm::vec3 side(con_camera->viewMatrix[2][0]/w,con_camera->viewMatrix[2][1]/w,con_camera->viewMatrix[2][2]/w);
		glm::vec3 up = glm::cross(v,side);
		con_camera->viewMatrix = glm::lookAt(con_camera->GetPos(),origin,up);
	}
}*/

/**
* OrbitController::CursorPosCallback
* @param window
* @param mx
* @param my
*
*/
void OrbitController::CursorPosCallback(GLFWwindow *window, double mx, double my)
{
	//std::cout<<"OrbitController::CursorPosCallback"<<std::endl;
	//int left_but = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
	//if (left_but==GLFW_PRESS) {
	if (dragging) {
		//std::cout<<"Orbiting...."<<mx<<" "<<my<<std::endl;

		int width,height;
		glfwGetFramebufferSize(window, &width, &height);

		//We know the camera's orbit sphere is being dragged at its surface and we know the radius of that sphere, so we don't actually need
		//to back project to find the point in 3D space and subtract the origin point in 3D. The only question is what scaling to use (i.e. perspective f)?
		glm::vec3 P2(mx,my,0);
		glm::vec3 delta=P2-dragPoint;
		delta.x/=(float)width;
		delta.y/=(float)height;
		//delta.z=1; //z determines scaling
		delta=delta*2.0f; //-1.0f; //set range to +-1 in both axes 

		//get distance from camera to object origin, which is the radius of the rotating sphere that the camera is fixed to
		//glm::vec3 vCameraPos = con_camera->GetCameraPos();
		//float radius = glm::distance(centre,vCameraPos);

		//float Phi = glm::asin(delta.y/radius); //was acos(...)-glm::half_pi<float>();
		//calculate Theta on base triangle using hypot calculated from sqrt(radius^2 - dy^2) on Phi triangle
		//float hy=glm::sqrt(radius*radius-delta.y*delta.y);
		//std::cout<<"hy="<<hy<<" delta.x/hy="<<delta.x/hy<<std::endl;
		//float Theta = glm::asin(delta.x/hy);

		//GLSL asin undefined for |x|>1, so use atan2 for Phi and Theta instead
		//float hy=glm::sqrt(radius*radius-delta.y*delta.y);
		//float Phi = glm::atan<float>(delta.y,hy);
		//float Theta = glm::atan<float>(delta.x,delta.z);
		
		//this actually works better, rotation is a linear factor of distance from click point
		float Phi=-delta.y;
		float Theta=delta.x;

		//std::cout<<"dx="<<delta.x<<" Theta="<<Theta<<" Phi="<<Phi<<" radius="<<radius<<" origin="<<centre.x<<","<<centre.y<<","<<centre.z<<std::endl;
		//using two angles, calculate the new direction that the camera is looking in
		//glm::mat4 newLook = glm::rotate(dragViewMatrix,Theta,glm::vec3(0,1,0)); //ONLY X!!!
		//glm::mat4 newLook = glm::rotate(dragViewMatrix,Phi,glm::vec3(1,0,0)); //ONLY Y!!!
		//both
		//glm::mat4 m1 = glm::rotate(glm::mat4(1),Theta,glm::vec3(0,1,0)); //ONLY X!!!
		//glm::mat4 m2 = glm::rotate(glm::mat4(1),Phi,glm::vec3(1,0,0)); //ONLY Y!!!
		//glm::mat4 m = m2*m1;
		//gtx euler angle YX (??) or yawPitchRoll

		//bearing in mind that we can calculate sin Theta, cos Theta, sin Phi and cos Phi very easily, you could just copy the euler code and speed things up a bit
		glm::mat4 m = glm::eulerAngleYX(Theta,Phi);

		//Perform calculations in world space using the camera matrix as it's easier to visualise than view space.
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
	else if (panning) {
		//right mouse being used to translate the view left/right/up/down
		int width,height;
		glfwGetFramebufferSize(window, &width, &height);
		glm::vec3 P2(mx,my,0);
		glm::vec3 delta=P2-dragPoint;
		delta.x/=(float)width;
		delta.y/=-(float)height;
		delta.z=0;
		delta=delta*2.0f; //set range to +-1 in both axes

		//do the translation in view space as it make more sense - everything is relative to the view
		glm::mat4 mView = dragViewMatrix;
		mView = glm::translate(mView,delta);
		con_camera->viewMatrix = mView;
	}
}


/**
* OrbitController::ScrollCallback
* @param window
* @param xoffset
* @param yoffset	The mouse wheeel button delta to scroll up or down (or zoom in and out in orbit camera sense)
*
* Use mouse wheel to alter the camera distance from the centre (the radius of the orbit).
*/
void OrbitController::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	const double speed = 0.01;

	//std::cout<<"OrbitController::ScrollCallback xoffset="<<xoffset<<" yoffset="<<yoffset<<std::endl;
	
	//complex version where it zooms in a percentage of the distance from the eye to the centre
	glm::vec3 vCameraPos = con_camera->GetCameraPos();
	glm::mat4 mCamera = con_camera->GetCameraMatrix();
	float radius = glm::distance(centre,vCameraPos);
	float delta = -radius*yoffset*speed; //where speed is the percentage i.e. 1/100=0.01
	glm::mat4 mNewCamera = glm::translate(mCamera,glm::vec3(0,0,delta));
	con_camera->SetCameraMatrix(mNewCamera);

	//simple version where we zoom in a fixed amount - this works better
	//glm::mat4 mCamera = con_camera->GetCameraMatrix();
	//float delta = speed*yoffset;
	//mCamera = glm::translate(mCamera,glm::vec3(0,0,-delta));
	//con_camera->SetCameraMatrix(mCamera);
}

/**
* OrbitController::MouseButtonCallback
* @param window
* @param button
* @param action
* @param mods
*
*/
void OrbitController::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	//std::cout<<"OrbitController::MouseButtonCallback"<<std::endl;
	if (button==GLFW_MOUSE_BUTTON_LEFT) {
		if (action==GLFW_PRESS) {
			dragging=true;
			//set click point
			double Px,Py;
			glfwGetCursorPos(window,&Px,&Py);
			dragPoint = glm::vec3(Px,Py,0);
			dragCameraMatrix = con_camera->GetCameraMatrix();
			std::cout<<"Drag Camera: "<<dragCameraMatrix[3][0]<<" "<<dragCameraMatrix[3][1]<<" "<<dragCameraMatrix[3][2]<<std::endl;
			std::cout<<"Centre: "<<centre.x<<" "<<centre.y<<" "<<centre.z<<std::endl;
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
			dragPoint = glm::vec3(Px,Py,0);
			dragViewMatrix = con_camera->viewMatrix;
		}
		else {
			panning=false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
