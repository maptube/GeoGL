
#include "LeapController.h"
//#include "EventManager.h"
#include "Leap.h"
#include "LeapMath.h"

using namespace Leap;

using namespace gengine;

const float LeapController::rateRoll = 0.01f; //rates applied to cursor position for aileron
const float LeapController::ratePitch = 0.01f; //rates applied to cursor position for elevator
const float LeapController::rateYaw = 0.01f; //rates applied to cursor position for rudder

LeapController::LeapController(Camera* camera)
{
	con_camera = camera;
	//Aileron=0;
	//Elevator=0;
	//Speed=0.00001f; //initial speed
	Roll=0;
	Pitch=0;
	Yaw=0;

	//now hook up the mouse events
	//EventManager& em=EventManager::getInstance();
	//em.AddCursorPosEventListener(this);
	//em.AddScrollEventListener(this);
	////em.AddMouseButtonEventListener(this);

	//Controller c;
	//DeviceList devList = c.devices();
	//Frame f = c.frame();
	//controller.addListener(listener);
	controller=new Controller();
	controller->addListener(*this);
}


LeapController::~LeapController(void)
{
	//tidy up after ourselves by removing the static event callback hooks from the EventManager
	//EventManager& em=EventManager::getInstance();
	//em.RemoveCursorPosEventListener(this);
	//em.RemoveScrollEventListener(this);
	////em.RemoveMouseButtonEventListener(this);

	controller->removeListener(*this);
	delete controller;
}

/**
* LeapController::Forward
*
* Moves camera forward along eve vector and performs rotation based on current Aileron and Elevator settings.
* Camera is moved this.Speed units along the eye vector.
*/
void LeapController::Forward()
{
	//rotation
//	glm::mat4 mCam = con_camera->GetCameraMatrix();
//	mCam = glm::rotate(mCam,-Aileron,glm::vec3(0,0,1));
//	mCam = glm::rotate(mCam,Elevator,glm::vec3(1,0,0));

	//move forwards along view
//	mCam = glm::translate(mCam,glm::vec3(0,0,-Speed)); //OpenGL Z -ve is into screen
//	con_camera->SetCameraMatrix(mCam);
}

void LeapController::Backward()
{
	//rotation
//	glm::mat4 mCam = con_camera->GetCameraMatrix();
//	mCam = glm::rotate(mCam,-Aileron,glm::vec3(0,0,1));
//	mCam = glm::rotate(mCam,Elevator,glm::vec3(1,0,0));

	//move forwards along view
//	mCam = glm::translate(mCam,glm::vec3(0,0,Speed)); //OpenGL Z -ve is into screen OPPOSITE of Forward
//	con_camera->SetCameraMatrix(mCam);
}

/// <summary>
/// Simulate flying a multirotor using your hand position over the leap motion for control. We ignore the effects of gravity though,
/// so the quad is basically floating.
/// Your hand determines the roll, pitch and yaw angles of the quad, which moves slowly in the appropriate direction.
/// </summary>
void LeapController::SimulateMultiRotor()
{
	//glm::vec3 xyz = con_camera->GetCameraPos();
	glm::mat4 mCam = con_camera->GetCameraMatrix();
	//mCam[3][0]=0; mCam[3][1]=0; mCam[3][2]=0;
	glm::mat4 m = glm::eulerAngleYXZ(0.0f/*-Yaw*/,Pitch,Roll);
	//glm::mat4 m = glm::eulerAngleYXZ(0.0f,0.0f,0.0f);
	//m = m_cam * m;
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			mCam[i][j]=m[i][j];

	//mCam = glm::rotate(mCam,Roll*rateRoll,glm::vec3(0,0,1));
	//mCam = glm::rotate(mCam,Pitch*ratePitch,glm::vec3(1,0,0));
	//mCam = glm::rotate(mCam,Yaw*rateYaw,glm::vec3(0,1,0));

	//now find direction vector to move along
	glm::vec3 up(mCam[1][0],mCam[1][1],mCam[1][2]); // mCam * (0,1,0)
	//mCam = glm::translate(mCam,glm::vec3(/*up.x/1000*/0,0,up.z/1000)); //no height translation
	mCam[3][0]+=up.x/1000;
	mCam[3][2]+=up.z/1000;
	//std::cout<<"dx="<<up.x/1000<<" dz="<<up.z/1000<<std::endl;

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
//void LeapController::CursorPosCallback(GLFWwindow *window, double mx, double my)
//{
//	int width,height;
//	glfwGetFramebufferSize(window, &width, &height);
//	
//	glm::vec3 P(mx,my,0);
//	P.x/=(float)width;
//	P.y/=(float)height;
//	P=P*2.0f-1.0f; //set range to +-1 in both axes
//
//	Aileron = P.x*rateX;
//	Elevator = P.y*rateY;
//	//std::cout<<"Aileron="<<Aileron<<" Elevator="<<Elevator<<std::endl;
//}
//
//


//////////////////////////////////////////////////////////////////////////////////////////////////

void LeapController::onInit(const Controller& controller) {
	std::cout << "Initialized" << std::endl;
}

void LeapController::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
}

void LeapController::onDisconnect(const Controller& controller) {
	//Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void LeapController::onExit(const Controller& controller) {
	std::cout << "Exited" << std::endl;
}

void LeapController::onFrame(const Controller& controller) {
	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();
	//std::cout << "Frame id: " << frame.id()
	//	<< ", timestamp: " << frame.timestamp()
	//	<< ", hands: " << frame.hands().count()
	//	<< ", fingers: " << frame.fingers().count()
	//	<< ", tools: " << frame.tools().count()
	//	<< ", gestures: " << frame.gestures().count() << std::endl;
	
	if (!frame.hands().isEmpty()) {
		// Get the first hand
		const Hand hand = frame.hands()[0];
		
		// Check if the hand has any fingers
		const FingerList fingers = hand.fingers();
		if (!fingers.isEmpty()) {
			// Calculate the hand's average finger tip position
			Vector avgPos;
			for (int i = 0; i < fingers.count(); ++i) {
				avgPos += fingers[i].tipPosition();
			}
			avgPos /= (float)fingers.count();
			//std::cout << "Hand has " << fingers.count()
			//	<< " fingers, average finger tip position" << avgPos << std::endl;
		}
		
		// Get the hand's sphere radius and palm position
		//std::cout << "Hand sphere radius: " << hand.sphereRadius()
		//	<< " mm, palm position: " << hand.palmPosition() << std::endl;
		
		// Get the hand's normal vector and direction
		const Vector normal = hand.palmNormal();
		const Vector direction = hand.direction();
		
		// Calculate the hand's pitch, roll, and yaw angles
		//std::cout << "Hand pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
		//	<< "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
		//	<< "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
		Roll=normal.roll();
		Pitch=direction.pitch();
		Yaw=direction.yaw();
		//std::cout<<"Roll: "<<Roll*RAD_TO_DEG<<" Pitch: "<<Pitch*RAD_TO_DEG<<" Yaw: "<<Yaw*RAD_TO_DEG<<std::endl;

		//hack - no hand, no rotation
		if (fingers.isEmpty()) {
			Roll=0; Pitch=0; Yaw=0;
		}
	}
	
	// Get gestures
	const GestureList gestures = frame.gestures();
	for (int g = 0; g < gestures.count(); ++g) {
		Gesture gesture = gestures[g];
		
		switch (gesture.type()) {
		case Gesture::TYPE_CIRCLE:
			{
				CircleGesture circle = gesture;
				std::string clockwiseness;
				
				if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
					clockwiseness = "clockwise";
				}
				else {
					clockwiseness = "counterclockwise";
				}
				
				// Calculate angle swept since last frame
				float sweptAngle = 0;
				if (circle.state() != Gesture::STATE_START) {
					CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
					sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
				}
				//std::cout << "Circle id: " << gesture.id()
				//	<< ", state: " << gesture.state()
				//	<< ", progress: " << circle.progress()
				//	<< ", radius: " << circle.radius()
				//	<< ", angle " << sweptAngle * RAD_TO_DEG
				//	<<  ", " << clockwiseness << std::endl;
				break;
			}
		case Gesture::TYPE_SWIPE:
			{
				SwipeGesture swipe = gesture;
				//std::cout << "Swipe id: " << gesture.id()
				//	<< ", state: " << gesture.state()
				//	<< ", direction: " << swipe.direction()
				//	<< ", speed: " << swipe.speed() << std::endl;
				break;
			}
		case Gesture::TYPE_KEY_TAP:
			{
				KeyTapGesture tap = gesture;
				//std::cout << "Key Tap id: " << gesture.id()
				//	<< ", state: " << gesture.state()
				//	<< ", position: " << tap.position()
				//	<< ", direction: " << tap.direction()<< std::endl;
				break;
			}
		case Gesture::TYPE_SCREEN_TAP:
			{
				ScreenTapGesture screentap = gesture;
				//std::cout << "Screen Tap id: " << gesture.id()
				//	<< ", state: " << gesture.state()
				//	<< ", position: " << screentap.position()
				//	<< ", direction: " << screentap.direction()<< std::endl;
				break;
			}
      default:
		  std::cout << "Unknown gesture type." << std::endl;
		  break;
		}
	}
	
	if (!frame.hands().isEmpty() || !gestures.isEmpty()) {
		//std::cout << std::endl;
	}

	SimulateMultiRotor();
}

void LeapController::onFocusGained(const Controller& controller) {
	std::cout << "Focus Gained" << std::endl;
}

void LeapController::onFocusLost(const Controller& controller) {
	std::cout << "Focus Lost" << std::endl;
}
