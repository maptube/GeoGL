
#include "FlyController.h"
#include "gengine/events/EventManager.h"

const float FlyController::rateX = 0.0001f; //rates applied to cursor position for aileron
const float FlyController::rateY = 0.0001f; //rates applied to cursor position for elevator

using namespace gengine;
using namespace events;

FlyController::FlyController(Camera* camera)
{
	con_camera = camera;
	Aileron=0;
	Elevator=0;
	Speed=0.00001f; //initial speed

	//now hook up the mouse events
	EventManager& em=EventManager::getInstance();
	em.AddCursorPosEventListener(this);
	em.AddScrollEventListener(this);
	//em.AddMouseButtonEventListener(this);
}


FlyController::~FlyController(void)
{
	//tidy up after ourselves by removing the static event callback hooks from the EventManager
	EventManager& em=EventManager::getInstance();
	em.RemoveCursorPosEventListener(this);
	em.RemoveScrollEventListener(this);
	//em.RemoveMouseButtonEventListener(this);
}


/**
* FlyController::Forward
*
* Moves camera forward along eve vector and performs rotation based on current Aileron and Elevator settings.
* Camera is moved this.Speed units along the eye vector.
*/
void FlyController::Forward()
{
	//rotation
	glm::mat4 mCam = con_camera->GetCameraMatrix();
	mCam = glm::rotate(mCam,-Aileron,glm::vec3(0,0,1));
	mCam = glm::rotate(mCam,Elevator,glm::vec3(1,0,0));

	//move forwards along view
	mCam = glm::translate(mCam,glm::vec3(0,0,-Speed)); //OpenGL Z -ve is into screen
	con_camera->SetCameraMatrix(mCam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
* FlyController:CursorPosCallback
* @param window
* @param mx
* @param my
*
*/
void FlyController::CursorPosCallback(GLFWwindow *window, double mx, double my)
{
	int width,height;
	glfwGetFramebufferSize(window, &width, &height);
	
	glm::vec3 P(mx,my,0);
	P.x/=(float)width;
	P.y/=(float)height;
	P=P*2.0f-1.0f; //set range to +-1 in both axes

	Aileron = P.x*rateX;
	Elevator = P.y*rateY;
	//std::cout<<"Aileron="<<Aileron<<" Elevator="<<Elevator<<std::endl;
}

void FlyController::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
}

void FlyController::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////