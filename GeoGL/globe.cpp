/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */


#include "globe.h"
#include "gengine/ogldevice.h"
#include "gengine/graphicscontext.h"
#include "gengine/scenedataobject.h"
#include "gengine/shader.h"
//#include "opengl4.h"
#include "object3d.h"
#include "sphere.h"
#include "gengine/events/EventManager.h"

#include "geojson.h"

using namespace std;

using namespace gengine;

Globe::Globe(void)
{
	ellipsoid = Ellipsoid(); //bit naughty, but the default is the WGS 84 definition

	int WindowWidth=512,WindowHeight=512;
	GC = OGLDevice::XCreateWindow(WindowWidth,WindowHeight); //create graphics context that we can render to

	//start graphics context
	//if (!openglContext.create30ContextGLFW()) { // Create a window and an OpenGL context to run in it
	//	std::cerr<<"Error creating OpenGL context - abort"<<std::endl;
	//	exit(-1);
	//}
	//openglContext.setupScene(); // Setup our OpenGL scene
	
	Shader* shader = new Shader("shader.vert", "shader.frag");

	//add sphere representing the earth
	Sphere* sphere=new Sphere(ellipsoid.A(),ellipsoid.B(),ellipsoid.C(),20,20);
	sphere->AttachShader(shader);
	//openglContext.SceneGraph.push_back(sphere);
	SceneGraph.push_back(sphere);

	//set up the camera
	//camera.SetupPerspective(openglContext.GetWindowWidth(), openglContext.GetWindowHeight(), 0.01f, 200.0f);  // Create our perspective matrix
	camera.SetupPerspective(WindowWidth, WindowHeight, 1.0f, 27000000.0f);
	//camera.viewMatrix = openglContext.fitViewMatrix(); //not much point in doing this as there's nothing there yet
	camera.SetCameraPos(glm::vec3(0.0f,0.0f,18000000.0f));
	//glm::vec3 vCam = camera.GetCameraPos();
	//std::cout<<"Globe Setup: camera ("<<vCam.x<<","<<vCam.y<<","<<vCam.z<<")"<<std::endl;
	_sdo = new SceneDataObject();
	_sdo->_camera=&camera;

	//set up events manager
	//EventManager& eventmanager = EventManager::getInstance(); //singleton pattern
	//eventmanager.SetWindow(window); //initialise event system with OpenGL window handle
	//eventmanager.AddWindowSizeEventListener(this);
}


Globe::~Globe(void)
{
	//destroy opengl context
	//TODO: I really hope this destroys all the buffers..... ??????
	//openglContext.destroyContextGLFW();
	//glfwTerminate();

	DestroyScene();
	delete _sdo;

	delete GC; //destroy graphics context and window
	OGLDevice::Destroy();
}

/// <summary>
/// Return false if glfwWindowShouldClose(GC->window)
/// </summary>
/// <returns>True if the application should terminate due to the fact that the globe window has been closed by the user</returns>
bool Globe::IsRunning(void) {
	return !glfwWindowShouldClose(GC->window);
}

/// <summary>
/// Destroy all the objects in the scene and their associated buffers
/// </summary>
void Globe::DestroyScene(void) {
	for (vector<Object3D*>::iterator sceneIT=SceneGraph.begin(); sceneIT!=SceneGraph.end(); ++sceneIT) {
		delete (*sceneIT);
	}
	SceneGraph.clear();
}

/// <summary>
/// Load data from a GeoJSON file and add it to the globe's scene graph for rendering.
/// This is really a utility function as all it does is create a GeoJSON object, load the file and push the object
/// onto the scene graph and return it.
/// </summary>
/// <param name="Filename">The geojson file to load</param>
/// <returns>The GeoJSON object that has been added to the scene graph</returns>
GeoJSON* Globe::LoadLayerGeoJSON(std::string Filename)
{
	//create a geojson object, load and add to scene graph
	GeoJSON* geoj = new GeoJSON();
	geoj->LoadFile(Filename);
	//make it a random colour?
	//thames->SetColour(glm::vec3(0.0f,0.0f,1.0f)); //better make it blue
//	openglContext.SceneGraph.push_back(geoj);
	return geoj;
}

void Globe::LoadLayerKML(std::string Filename)
{
	//TODO:
}

void Globe::LoadLayerShapefile(std::string Filename)
{
	//TODO:
}

/// <summary>
/// Add an ABM Model to the globe as a data layer that animates.
/// TODO: this is a real kludge as the model needs access to the scene graph in its constructor, which it gets from
/// the globe's GetSceneGraph. Then the model adds itself as a layer (which it doesn't really need to do as it's got
/// direct access to the scene graph). An ideal solution would be to just create the model and push it as a layer, not
/// worrying about any scene graph problems which are then handled by the globe.
/// </summary>
/// <param name="model">The model to push</param>
void Globe::AddLayerModel(ABM::Model* model)
{
	modelLayers.push_back(model);
}

/// <summary>
/// Change the camera position and zoom so that all the data fits into the view
/// </summary>
void Globe::FitViewToLayers(void)
{
	//camera.viewMatrix = openglContext.fitViewMatrix();
}

/// <summary>
/// Render the scene using the open GL context and the currently selected camera
/// </summary>
void Globe::RenderScene(void)
{
	//glm::vec3 vCam = camera.GetCameraPos();
	//std::cout<<"Render: camera ("<<vCam.x<<","<<vCam.y<<","<<vCam.z<<")"<<std::endl;
	//openglContext.renderScene(&camera);

	//set render state
	//bind shaders and variables and buffers
	//setup matrices
	//camera?
	//fallback?

	//TODO: need to get this gl reference out!!!
	glClearColor(0.4f, 0.6f, 0.9f, 0.0f); // Set the clear color based on Microsoft's CornflowerBlue (default in XNA)
	GC->Clear();

	//this assumes all the matrices are right

	//go through all scene objects and render each in turn (modelMatrix should be identity really?)
	for (vector<Object3D*>::iterator sceneIT=SceneGraph.begin(); sceneIT!=SceneGraph.end(); ++sceneIT) {
		//(*sceneIT)->Render(ShaderId,modelMatrix);
		Object3D* o3d=(*sceneIT);
		if (o3d->HasGeometry()) {
			const DrawObject& dobj = o3d->GetDrawObject();
			GC->Render(dobj,*_sdo);
		}
	}

	GC->SwapBuffers();
}



