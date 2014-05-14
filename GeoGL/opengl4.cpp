#include "opengl4.h"
//#include <SDL_opengl.h>
//#include "cuboid.h"
//#include "cylinder.h"
//#include "sphere.h"
//#include "meshcube.h"
//#include "tubegeometry.h"
//#include "trefoilknot.h"
//#include "turtle.h"
//#include "sierpinskipyramid.h"
//#include "netgraphgeometry.h"
//#include "DebugUtils.h"

#include "math.h"

using namespace std;

using namespace gengine;

//http://www.openglbook.com/the-book
//http://www.swiftless.com/tutorials/opengl4/5-opengl-4-vao-color.html
//http://www.opengl.org/archives/resources/faq/technical/transformations.htm
//http://en.wikibooks.org/wiki/OpenGL_Programming
//this is very good
//http://antongerdelan.net/opengl/
//http://antongerdelan.net/opengl/shaders.html
//http://antongerdelan.net/opengl/vertexbuffers.html

//static member initialisation
bool OpenGLContext::hasProgrammableShaders=false;
int OpenGLContext::VersionMajor=0;
int OpenGLContext::VersionMinor=0;


//////////////////////////////////////////////////////////////////////////////////////////////////////

/* A simple function that prints a message, the error code returned by SDL,
 * and quits the application */
/*
void sdldie(const char *msg)
{
	printf("%s: %s\n", msg, SDL_GetError());
	SDL_Quit();
	exit(1);
}
*/

/*
void checkSDLError(int line = -1)
{
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
#endif
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
	Default constructor for the OpenGLContext class. At this stage it does nothing
	but you can put anything you want here.
*/
OpenGLContext::OpenGLContext(void) {

}

/**
	Constructor for the OpenGLContext class which will create a context given a windows HWND.
*/
OpenGLContext::OpenGLContext(HWND hwnd) {
	create30Context(hwnd); // Create a context given a HWND
}

/**
	Destructor for our OpenGLContext class which will clean up our rendering context
	and release the device context from the current window.
*/
OpenGLContext::~OpenGLContext(void) {
	//wglMakeCurrent(hdc, 0); // Remove the rendering context from our device context
    //wglDeleteContext(hrc); // Delete our rendering context

    //ReleaseDC(hwnd, hdc); // Release the device context from our window
}

/**
* WindowSizeCallback
* Event processing system. Raised by EventListeners if you hook the event up.
*/
void OpenGLContext::WindowSizeCallback(GLFWwindow* window, int w, int h) {
	reshapeWindow(w,h);
}

/**
	create30Context creates an OpenGL context and attaches it to the window provided by
	the HWND. This method currently creates an OpenGL 3.2 context by default, but will default
	to an OpenGL 2.1 capable context if the OpenGL 3.2 context cannot be created.
*/
bool OpenGLContext::create30Context(HWND hwnd) {
/*
	this->hwnd = hwnd; // Set the HWND for our window

	hdc = GetDC(hwnd); // Get the device context for our window

	PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd); // Check if our PFD is valid and get a pixel format back
	if (nPixelFormat == 0) // If it fails
			return false;

	BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD
	if (!bResult) // If it fails
		return false;

	HGLRC tempOpenGLContext = wglCreateContext(hdc); // Create an OpenGL 2.1 context for our device context
	wglMakeCurrent(hdc, tempOpenGLContext); // Make the OpenGL 2.1 context current and active

	GLenum error = glewInit(); // Enable GLEW
	if (error != GLEW_OK) // If GLEW fails
		return false;
	//if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
	//exit(1); // or handle the error in a nicer way
	//AFAIK the way to go is: When you've detected the OpenGL version via
	//glGetString(GL_VERSION)
	//and the version is at least 2.0, then you can use
	//glGetString(GL_SHADING_LANGUAGE_VERSION)
	//to get the GLSL version. For OpenGL below 2.0 you have to check for the extension
	//ARB_SHADING_LANGUAGE_100
	//what obviously means that GLSL 1.0.0 is available if the extension is resolved, else no GLSL is available at all.

	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3
		WGL_CONTEXT_MINOR_VERSION_ARB, 2, // Set the MINOR version of OpenGL to 2
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1) { // If the OpenGL 3.x context creation extension is available
		hrc = wglCreateContextAttribsARB(hdc, NULL, attributes); // Create and OpenGL 3.x context based on the given attributes
		wglMakeCurrent(NULL, NULL); // Remove the temporary context from being active
		wglDeleteContext(tempOpenGLContext); // Delete the temporary OpenGL 2.1 context
		wglMakeCurrent(hdc, hrc); // Make our OpenGL 3.0 context current

		int glVersion[2] = {-1, -1}; // Set some default values for the version
		glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using
		glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using
		VersionMajor=glVersion[0];
		VersionMinor=glVersion[1];
		std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	}
	else {
		hrc = tempOpenGLContext; // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context
		//fallback to detect OpenGL versions prior to 3.x
//http://glew.sourceforge.net/basic.html
		//From glew.h: 1_1, 1_2, 1_2_1, 1_3, 1_4, 1_5, 2_0, 2_1,
		//3_0, 3_1, 3_2, 3_3, 4_0, 4_1, 4_2, 4_3, 4_4;
		if (GLEW_VERSION_3_0) { //shouldn't happen
			VersionMajor=3; VersionMinor=0;
		}
		else if (GLEW_VERSION_2_0) {
			VersionMajor=2; VersionMinor=0;
			//use glShaderVersionString to get shader support
		}
		else if (GLEW_VERSION_1_5) { //a lot of early systems have this level of support at least (?)
			VersionMajor=1; VersionMinor=5;
		}
		else if (GLEW_VERSION_1_1) {
			VersionMajor=1; VersionMinor=1;
		}
		else {
			VersionMajor=0; VersionMinor=0;
			//no support - exit, or should we have already crashed by now?
		}
		//glGetString?(GL_VENDOR?);
		//glGetString?(GL_RENDERER?);
		const GLubyte *glRenderer = glGetString(GL_RENDERER);
		std::cout << "OpenGL Renderer: "<<glRenderer<<endl;
		//if glRenderer contains "MESA" then it's probably a software renderer, except Linux which has MESA accelerated GPU code
	}

	const GLubyte *glVersionString = glGetString(GL_VERSION); // Get the version of OpenGL we are using
	cout<<"OpenGL version string: "<<glVersionString<<endl;
	
	//check for shader support - todo: make use of the version number
	const GLubyte *glShaderVersionString = glGetString(GL_SHADING_LANGUAGE_VERSION);
	hasProgrammableShaders=(glShaderVersionString!=NULL);

	//enable z buffer, back face culling and set front faces to CCW
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
*/
	return true; // We have successfully created a context, return true
}

/**
* Context creation for SDL and GLEW initialisation.
* At the moment I'm having to request OpenGL 4.1 directly as it doesn't allow the forward compatibility setting.
*/
bool OpenGLContext::create30ContextSDL() {
/*	//note SDL attribute SDL_GL_STEREO
	//See this for stereo 3D example
	//http://horde3d.org/wiki/index.php5?title=Tutorial_-_Stereo_rendering

	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize SDL's Video subsystem
		sdldie("Unable to initialize SDL"); // Or die on error
	
	//Request opengl 3.2 context. CHANGED this to request 4.1
	//SDL doesn't have the ability to choose which profile at this time of writing,
	//but it should default to the core profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4/ *3* /);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1/ *2* /);
	//SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); //is this needed?
	//SDL_GL_SetAttribute(SDL_GL_STEREO, 1); //kills window creation - needs to be fullscreen window and quad buffered

	//Turn on double buffering with a 24bit Z buffer.
	//You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	//Create our window centered at 512x512 resolution
	windowWidth=512; windowHeight=512;
	mainwindow = SDL_CreateWindow("GeoGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!mainwindow) // Die if creation failed
		sdldie("Unable to create window");
	
	checkSDLError(__LINE__);
	
	// Create our opengl context and attach it to our window
	maincontext = SDL_GL_CreateContext(mainwindow);
	checkSDLError(__LINE__);

	int glVersion[2] = {-1, -1}; // Set some default values for the version
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using
	VersionMajor=glVersion[0];
	VersionMinor=glVersion[1];
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	
	// This makes our buffer swap syncronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1); //this seems to kill the frame rate 

	//you have to initialise GLEW after SDL's OpenGL initialisation
	GLenum error = glewInit(); // Enable GLEW
	if (error != GLEW_OK) // If GLEW fails
		return false;
	
	//todo: additional version checking here using GLEW?

	const GLubyte *glVersionString = glGetString(GL_VERSION); // Get the version of OpenGL we are using
	cout<<"OpenGL version string: "<<glVersionString<<endl;
	
	//check for shader support - todo: make use of the version number
	const GLubyte *glShaderVersionString = glGetString(GL_SHADING_LANGUAGE_VERSION);
	hasProgrammableShaders=(glShaderVersionString!=NULL);

	//enable z buffer, back face culling and set front faces to CCW
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
*/
	return true; // We have successfully created a context, return true
}

void OpenGLContext::destroyContextSDL() {
/*	// Delete our opengl context, destroy our window, and shutdown SDL
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();
*/
}

void OpenGLContext::destroyContextGLFW() {
	glfwDestroyWindow(window);
}

bool OpenGLContext::create30ContextGLFW() {
	//See this for stereo 3D example
	//http://horde3d.org/wiki/index.php5?title=Tutorial_-_Stereo_rendering


	//glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	
	windowWidth=512; windowHeight=512;
	window = glfwCreateWindow(windowWidth, windowHeight, "GeoGL", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	//you need to have created a window and OpenGL context before you tyr and initialise GLEW
	GLenum error = glewInit(); // Enable GLEW
	if (error != GLEW_OK) // If GLEW fails
		return false;
	
	//GLFW initialisation should have requested the latest version of OpenGL without all the GLEW messing about before

	int glVersion[2] = {-1, -1}; // Set some default values for the version
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using
	VersionMajor=glVersion[0];
	VersionMinor=glVersion[1];
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	
	//todo: additional version checking here using GLEW?

	const GLubyte *glVersionString = glGetString(GL_VERSION); // Get the version of OpenGL we are using
	cout<<"OpenGL version string: "<<glVersionString<<endl;
	
	//check for shader support - todo: make use of the version number
	const GLubyte *glShaderVersionString = glGetString(GL_SHADING_LANGUAGE_VERSION);
	hasProgrammableShaders=(glShaderVersionString!=NULL);

	//enable z buffer, back face culling and set front faces to CCW
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	return true; // We have successfully created a context, return true
}

/**
* Free all the objects in the scene graph
*/
void OpenGLContext::destroyScene() {
	for (vector<Object3D*>::iterator sceneIT=SceneGraph.begin(); sceneIT!=SceneGraph.end(); ++sceneIT) {
		delete (*sceneIT);
	}
	SceneGraph.clear();
}

/**
	setupScene will contain anything we need to setup before we render
*/
void OpenGLContext::setupScene(void) {
	glClearColor(0.4f, 0.6f, 0.9f, 0.0f); // Set the clear color based on Microsoft's CornflowerBlue (default in XNA)

	if (hasProgrammableShaders)
		shader = new gengine::Shader("shader.vert", "shader.frag"); // Create our shader by loading our vertex and fragment shader

	//you can also get the width and height for the window like this:
	//glfwGetFramebufferSize(openglContext.window, &width, &height);
	//projectionMatrix is now in Camera 
	//projectionMatrix = glm::perspective(60.0f*glm::pi<float>()/180.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);  // Create our perspective matrix

	//create tube data instead
	//debug
	//TubeNetwork* tn = new TubeNetwork();
	//tn->load();
	//createFromGraph(tn->tube_graph,tn->tube_stations);
	//delete tn;
	//end debug

	//create a simple scene graph with objects in it
//	Cuboid* cuboid = new Cuboid(0.02f,0.01f,0.01f);
	//cuboid->SetPos(-1.0,0,0);
//	cuboid->SetPos(-0.11464,51.46258,0); //Brixton
//	SceneGraph.push_back(cuboid);
	//Cylinder* cyl = new Cylinder(1,0.5,12);
	//cuboid->AddChild(cyl);
	//Cuboid* cuboid2 = new Cuboid(1,1,1);
	//cuboid2->SetPos(2.0,0,0); //translate back to origin relative to parent (cuboid)
	//cuboid->AddChild(cuboid2);

	//TubeGeometry* tube = new TubeGeometry();
	//add points
	//vector<glm::vec3> spline;
	//spiral
	//for (float t=0; t<30; t++) {
	//	glm::vec3 P;
	//	P.x=0.5*cos(t/2); P.y=t/10-0.5; P.z=0.5*sin(t/2);
	//	spline.push_back(P);
	//}
	//tube->AddSpline(spline);
	//tube->GenerateMesh(0.2,25);
	//cuboid->AddChild(tube);
	
	//trefoil knot
	//TrefoilKnot* knot = new TrefoilKnot(0.2,10);
	//cuboid->AddChild(knot);
	

	//Sphere* sphere = new Sphere(0.5,10,10);
	//sphere->SetPos(1.0,0,0);
	//cuboid->AddChild(sphere);
	
	//MeshCube* mcube = new MeshCube();
	//mcube->SetPos(1.0,0,0);
	//cuboid->AddChild(mcube);

	//turtle
	//Turtle* turtle = new Turtle();
	//cuboid->AddChild(turtle);

	//Sierpinski Pyramid - depth=5 will max out the creation process
	//SierpinskiPyramid* sp = new SierpinskiPyramid(1.0f,5);
	//cuboid->AddChild(sp);
	//cout<<"Pyramid Count="<<sp->PyramidCount<<endl;

	//OK, enough of the test objects, on to some real data
	//TubeNetwork* tn = new TubeNetwork();
	//tn->load();
	//NetGraphGeometry* lu=new NetGraphGeometry(tn->tube_graph,tn->tube_stations,0.00025f,10);
	////cuboid->AddChild(lu);
	//SceneGraph.push_back(lu); //add relative to origin
	//delete tn?
}

/**
	reshapeWindow is called every time our window is resized, and it sets our windowWidth and windowHeight
	so that we can set our viewport size.
*/
void OpenGLContext::reshapeWindow(int w, int h) {
	windowWidth = w; // Set the window width
	windowHeight = h; // Set the window height
}

/**
* Fit a view matrix to the data in the current scene so that it all fits within the returned view
*/
glm::mat4 OpenGLContext::fitViewMatrix() {

	float my_near = 1.0f; //0.1f; //near and far are #defined somewhere
	float my_far = 27000000.0f; //100.0f;

	//walk the scene and union all the boxes
	BBox box;
	for (vector<Object3D*>::iterator sceneIT=SceneGraph.begin(); sceneIT!=SceneGraph.end(); ++sceneIT) {
		Object3D* o3d = *sceneIT;
		box.Union(o3d->GetGeometryBounds()); //this should return the bounds for the object and all its children
		//OutputDebugStringA("Node: ");
	}
	std::cout<<"View Box: "<<box.min.x<<","<<box.min.y<<"   "<<box.max.x<<","<<box.max.y<<std::endl;

	float x1=box.min.x,
		x2=box.max.x,
		y1=box.min.y,
		y2=box.max.y,
		z1=box.min.z,
		z2=box.max.z;
	//find centre of x, y and z axes which is the centre on the earth sphere (z is height)
	float cx=(x1+x2)/2;
	float cy=(y1+y2)/2;
	float cz=(z1+z2)/2;
	glm::vec3 vc(cx,cy,cz); //vector from origin to point we want to look at on surface (centre)
	//calculate d distance from object, which guarantees all the max dimension of the scene box is within the width of the viewport
	float size = max(max(x2-x1,y2-y1),z2-z1);
	float fov = (float)windowWidth/2/tan(30.0*glm::pi<float>()/180.0);
	float d = size*fov/(float)windowWidth;
	////point the viewpoint from the origin to the centre of the objects
	//glm::mat4 eye_mat = glm::lookAt(glm::vec3(0,0,0),vc,glm::vec3(0,1,0)); //look from position 5 z back from object
	////move the viewpoint along its eye direction so that the distance from view (origin) to vc is d
	//glm::vec3 view_p = vc - glm::vec3(0,0,-d) * glm::mat3(eye_mat); //this is the position the viewpoint needs to be
	//view = glm::translate(view,view_p); //translate view to its new location

	//alternative, point along Z axis straight towards object, note view initialised to mat4(1) first
	glm::mat4 view(1);
	view = glm::translate(view, glm::vec3(-cx,-cy,-cz-d));

	return view;
}

/**
	renderScene will contain all our rendering code.

	The first thing we are going to do is set our viewport size to fill the entire window. 

	Next up we are going to clear our COLOR, DEPTH and STENCIL buffers to avoid overlapping
	of renders.

	Any of your other rendering code will go here.

	Finally we are going to swap buffers.
*/
void OpenGLContext::renderScene(Camera* camera) {
 //   glViewport(0, 0, windowWidth, windowHeight); // Set the viewport size to fill the window
 //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers

	////viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.f)); // Create our view matrix
	////modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));  // Create our model matrix
	////modelMatrix = glm::scale(glm::mat4(0.1f), glm::vec3(0.5f));  // Create our model matrix
	////modelMatrix = glm::rotate(modelMatrix,rotateAngle,glm::vec3(0.0f,1.0f,0.0f));
	////viewMatrix = fitViewMatrix();
	//
	////SceneGraph.at(0)->Rotate(0.0001f,glm::vec3(0.0f,1.0f,0.0f)); //@50FPS, this is 0.29 degrees per second

	//unsigned int ShaderId = 0;
	//if (hasProgrammableShaders) {
	//	shader->bind(); // Bind our shader
	//	ShaderId = shader->id();

	//	int projectionMatrixLocation = glGetUniformLocation(shader->id(), "projectionMatrix"); // Get the location of our projection matrix in the shader
	//	int viewMatrixLocation = glGetUniformLocation(shader->id(), "viewMatrix"); // Get the location of our view matrix in the shader
	//	int modelMatrixLocation = glGetUniformLocation(shader->id(), "modelMatrix"); // Get the location of our model matrix in the shader

	//	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &camera->projectionMatrix[0][0]); // Send our projection matrix to the shader
	//	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]); // Send our view matrix to the shader
	//	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader
	//}
	//else {
	//	//fallback - no shaders and have to push matrices onto a stack
	//	//I'm going to push the projection and handle the model view myself
	//	glMatrixMode(GL_PROJECTION);
	//	glLoadMatrixf(&camera->projectionMatrix[0][0]);
	//	//TODO: don't need this? it gets changed for each model
	//	//glMatrixMode(GL_MODELVIEW);
	//	//glLoadMatrixf(&viewMatrix[0][0]); //OK, this is just the view matrix
	//	//todo: fix this! Might have to change viewMatrix for lighting calculations? Works fine for flat shading
	//	modelMatrix=camera->viewMatrix; //modelmatrix is (1) at this point anyway and we need to setup a chain of matrix muls for objects 
	//}

	////go through all scene objects and render each in turn (modelMatrix should be identity really?)
	//for (vector<Object3D*>::iterator sceneIT=SceneGraph.begin(); sceneIT!=SceneGraph.end(); ++sceneIT) {
	//	(*sceneIT)->Render(ShaderId,modelMatrix);
	//}

	//if (hasProgrammableShaders)
	//	shader->unbind(); // Unbind our shader

	//glfwSwapBuffers(window);
	////SDL_GL_SwapWindow(mainwindow); // Swap buffers so we can see our rendering
	////SwapBuffers(hdc); // Swap buffers so we can see our rendering
}


