
#include "LeapController.h"
//#include "EventManager.h"
//#include "Leap.h"
//#include "LeapMath.h"
#include "ellipsoid.h"

//using namespace Leap;

using namespace gengine;

/*
const double LeapController::rateRoll = 0.01; //rates applied to cursor position for aileron
const double LeapController::ratePitch = 0.01; //rates applied to cursor position for elevator
const double LeapController::rateYaw = 0.01; //rates applied to cursor position for rudder
*/

/*
LeapController::LeapController(Camera* camera, GLFWwindow* window)
{
	con_camera = camera;
	con_window = window;
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
*/

/*
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
*/

/**
* LeapController::Forward
*
* Moves camera forward along eve vector and performs rotation based on current Aileron and Elevator settings.
* Camera is moved this.Speed units along the eye vector.
*/
/*
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
*/

/*
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
*/

//use a normalised leap y value to control camera distance to ellipsoid in a non-linear way
//TODO: need a better name for this
/*
void LeapController::EllipsoidDistance(Ellipsoid& e,const float LeapY)
{
	//code copied from ellipsoid orbit controller
	const float scrollSpeed = 0.01f;
	const double nullzone=0.1; //null zone in the middle where zoom is clamped to zero (in raw normalised leap coordinates)
	if (glm::abs(LeapY)<nullzone) return;

	//complex version where it zooms in a percentage of the distance from the eye to the centre
	glm::dvec3 vCameraPos = con_camera->GetCameraPos();
	glm::dmat4 mCamera = con_camera->GetCameraMatrix();
	double h = e.heightAboveSurfaceAtPoint(vCameraPos);
	double delta = -h*LeapY*scrollSpeed; //where speed is the percentage i.e. 1/100=0.01
	//std::cout<<"delta="<<delta<<std::endl;
	glm::dmat4 mNewCamera = glm::translate(mCamera,glm::dvec3(0,0,delta));
	con_camera->SetCameraMatrix(mNewCamera);
}
*/

/// <summary>
/// Use left/right, forward/back hand movements to spin the globe, mimicking the right click pan function of the EllipsoidOrbitController.
/// The only difference is that there is no intersection point on the ellipsoid, the camera is just moved at a speed relative to the
/// hand distance from the intersection box.
/// TODO: it might be worth changing the sensitivity as you get closer to the ellipsoid.
/// NOTE: this is mainly just a fudge to get a navigation system that is actually controllable
/// <summary>
/// <param name="e"></param>
/// <param name="LeapX">Normalised left/right pan speed (-0.5 to 0.5)</param>
/// <param name="LeapZ">Normalised forward/back pan speed (-0.5 to 0.5)</param>
/*
void LeapController::EllipsoidSpin(Ellipsoid& e, const float LeapX, const float LeapZ)
{
	//The centre of rotation (initial point) is taken as the line from the camera to the origin (sphere centre)
	//The two angles of rotation are relative to the camera right and up vectors
	const double speed=0.000000002; //speed multiplier - this times LeapX or Z gives you the angle in radians to rotate by
	const double nullzone=0.1; //null zone in the middle where rotation is clamped to zero (in raw normalised leap coordinates)

	glm::dvec3 vCameraPos = con_camera->GetCameraPos();
	double h = e.heightAboveSurfaceAtPoint(vCameraPos);
	//double h = glm::distance(vCameraPos,glm::dvec3(0,0,0)); //distance from camera to ellipsoid centre (origin)
	
	//don't like this, the orbital speed is too slow if you spin by keeping the distance constant
	//delta is the angle multiplier for this height above the surface so that movement always moves the same distance over the ground
	//speed=10000 for this to work and h is distance from origin
	//double delta = glm::atan(speed/h); //so angular speed decreases with height (actually, you're trying to keep h tan theta constant)

	//fudge the speed delta
	double delta = speed*h;
	if (delta>0.01) delta=0.01;
	//std::cout<<"delta: "<<delta<<std::endl;

	//I've added the minus to both angles so that visually we're moving the sphere with our hand, which seems a lot more intuitive
	//calculate speed and handle the null zone in the middle
	double ax=0,ay=0;
	if (glm::abs(LeapX)>=nullzone)
		ax=-delta*LeapX;
	if (glm::abs(LeapZ)>=nullzone)
		ay=-delta*LeapZ;

	//now on to the rotations
	glm::dmat4 mCam = con_camera->GetCameraMatrix();
	glm::dmat4 m(1);
	m = glm::rotate(m,ax,glm::dvec3(mCam[1])); //x rotation relative to camera right vector
	m = glm::rotate(m,ay,glm::dvec3(mCam[0])); //y rotation relative to camera up vector
	mCam = m * mCam;
	con_camera->SetCameraMatrix(mCam);
}
*/

/// <summary>
/// Rotate around the X axis (aileron roll)
/// </summary>
/*
void LeapController::RotateX(const double ax)
{
	glm::dmat4 mCam = con_camera->GetCameraMatrix();
	glm::dmat4 m(1);
	m = glm::rotate(m,ax,glm::dvec3(mCam[2]));
	mCam = m * mCam;
	con_camera->SetCameraMatrix(mCam);
}
*/

/// <summary>
/// Simulate flying a multirotor using your hand position over the leap motion for control. We ignore the effects of gravity though,
/// so the quad is basically floating.
/// Your hand determines the roll, pitch and yaw angles of the quad, which moves slowly in the appropriate direction.
/// </summary>
/*
void LeapController::SimulateMultiRotor()
{
	//glm::vec3 xyz = con_camera->GetCameraPos();
	glm::dmat4 mCam = con_camera->GetCameraMatrix();
	//mCam[3][0]=0; mCam[3][1]=0; mCam[3][2]=0;
	glm::dmat4 m = glm::eulerAngleYXZ(0.0/ *-Yaw* /,Pitch,Roll);
	//glm::mat4 m = glm::eulerAngleYXZ(0.0f,0.0f,0.0f);
	//m = m_cam * m;
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			mCam[i][j]=m[i][j];

	//mCam = glm::rotate(mCam,Roll*rateRoll,glm::vec3(0,0,1));
	//mCam = glm::rotate(mCam,Pitch*ratePitch,glm::vec3(1,0,0));
	//mCam = glm::rotate(mCam,Yaw*rateYaw,glm::vec3(0,1,0));

	//now find direction vector to move along
	glm::dvec3 up(mCam[1][0],mCam[1][1],mCam[1][2]); // mCam * (0,1,0)
	//mCam = glm::translate(mCam,glm::vec3( / * up.x/1000 * / 0,0,up.z/1000)); //no height translation
	mCam[3][0]+=up.x/1000;
	mCam[3][2]+=up.z/1000;
	//std::cout<<"dx="<<up.x/1000<<" dz="<<up.z/1000<<std::endl;

	con_camera->SetCameraMatrix(mCam);
}
*/

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

/*
void LeapController::onInit(const Controller& controller) {
	//std::cout << "Leap Initialized" << std::endl;
}
*/

/*
void LeapController::onConnect(const Controller& controller) {
	//std::cout << "Leap Connected" << std::endl;
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
}
*/

/*
void LeapController::onDisconnect(const Controller& controller) {
	//Note: not dispatched when running in a debugger.
	//std::cout << "Leap Disconnected" << std::endl;
}
*/

/*
void LeapController::onExit(const Controller& controller) {
	//std::cout << "Leap Exited" << std::endl;
}
*/

/*
void LeapController::onFrame(const Controller& controller) {
	GeoNavigate(controller); return;

	testPoint(controller);
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
*/

/// <summary>
/// Navigate on a Globe.
/// Open hand=navigate, make a fist to stop.
/// Up/down zooms in and out of globe (radius).
/// Movement of hand moves over the ground.
/// </summary>
/*
void LeapController::GeoNavigate(const Controller& controller) {
	Ellipsoid e; //HACK, need to pass this in
	const Frame frame = controller.frame();
	//do nothing unless at least one hand is detected
	if (frame.hands().isEmpty()) return;
	const Leap::Hand firstHand = frame.hands()[0];
	//check fingers for open/closed hand
	const FingerList fingers = firstHand.fingers();
	if (fingers.isEmpty()) return;
	//at this point we have a hand with fingers, so we can move the viewpoint around
	//const Leap::Vector pos = fingers[0].tipPosition();
	//std::cout<<"Leap: Y="<<pos.y<<std::endl;
	//std::cout<<"Leap: Palm Y="<<firstHand.stabilizedPalmPosition().y<<std::endl;
	//firstHand.

	Leap::InteractionBox box=frame.interactionBox();
	if (!box.isValid()) return;
	Leap::Vector pos=box.normalizePoint(fingers[0].tipPosition(),true);
	//std::cout<<"Leap: y="<<pos.y<<std::endl;
	EllipsoidDistance(e,0.5f-pos.y);
	EllipsoidSpin(e,pos.x-0.5f,pos.z-0.5f);

	//now detect a circle gesture to do the X rotation
	const GestureList gestures = frame.gestures();
	for (int g = 0; g < gestures.count(); ++g) {
		Gesture gesture = gestures[g];
		
		switch (gesture.type()) {
		case Gesture::TYPE_CIRCLE:
			{
				CircleGesture circle = gesture;
				double clockwise=-1.0; //-1=anticloskwise, +1=clockwise
				if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
					clockwise = 1.0;
				}
				
				// Calculate angle swept since last frame
				float sweptAngle = 0;
				if (circle.state() != Gesture::STATE_START) {
					CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
					//sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
					RotateX(clockwise*circle.progress()*2*PI*0.001); //0.001 is fudge for speed
				}
				break;
			}
		}
	}
}
*/

/*
void LeapController::testPoint(const Controller& controller) {
	const Frame frame = controller.frame();
	// do nothing unless hands are detected
	if (frame.hands().isEmpty()) return;
	// first detected hand
	const Leap::Hand firstHand = frame.hands()[0];
	// first pointable object (finger or tool)
	const Leap::PointableList pointables = firstHand.pointables();
	if (pointables.isEmpty()) return;
	const Leap::Pointable firstPointable = pointables[0];

	// get list of detected screens
	const Leap::ScreenList screens = controller.locatedScreens();
	// make sure we have a detected screen
	if (screens.isEmpty()) return;
	const Leap::Screen screen = screens[0];

	// get x, y coordinates on the first screen
	const Leap::Vector intersection = screen.intersect(
		firstPointable,
		true, // normalize
		1.0f // clampRatio
	);

	if (!intersection.isValid()) return;
	// print intersection coordinates
	std::cout << "You are pointing at (" <<
		intersection.x << ", " <<
		intersection.y << ", " <<
		intersection.z << ")" << std::endl;

	//move the mouse to the position being pointed at on the screen - need position of window on display
	//(leap is global, desktop coords?) and point on the leap detected screen
	int fullx,fully,w,h,xpos,ypos;
	//GLFWmonitor* monitor = glfwGetWindowMonitor(con_window);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	glfwGetMonitorPhysicalSize(monitor,&fullx,&fully);
	//fullx=2560; fully=1440; //hack, why is above returning the wrong values?
	glfwGetWindowSize(con_window,&w,&h);
	glfwGetWindowPos(con_window,&xpos,&ypos);
	double wx=intersection.x*(double)fullx-(double)xpos;
	double wy=(1.0-intersection.y)*(double)fully-(double)ypos;
	//clamp to window
	if (wx<0) wx=0;
	else if (wx>w) wx=w;
	if (wy<0) wy=0;
	else if (wy>h) wy=h;

	//and set the mouse position
	glfwSetCursorPos(con_window,wx,wy);
}
*/

/*
void LeapController::onFocusGained(const Controller& controller) {
	std::cout << "Leap Focus Gained" << std::endl;
}
*/

/*
void LeapController::onFocusLost(const Controller& controller) {
	std::cout << "Leap Focus Lost" << std::endl;
}
*/
