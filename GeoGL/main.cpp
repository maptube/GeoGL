/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all non-commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */

#include "main.h"
#include "gengine/ogldevice.h"
#include "globe.h"
#include "ellipsoid.h"
//#include "opengl4.h"
//and this is very awkward - need to separate from main code so user software uses globe, not this way around
#include "../usr/model_tubenetwork.h"
#include "../usr/model_ml_tubenetwork.h"
#include "../usr/model_busnetwork.h"
#include "../usr/model_correlate.h"
#include "../usr/model_networkrail.h"
#include "../usr/model_wxairq.h"
#include "../usr/model_traveltowork.h"
#include "netgraphgeometry.h"
#include "geojson.h"
#include "GroundBox.h"
#include "gengine/events/EventManager.h"
#include "OrbitController.h"
#include "FlyController.h"
//#include "LeapController.h"
#include "gengine/graphicscontext.h"
#include "gengine/vertexbuffer.h"
#include "gengine/indexbuffer.h"
#include "gengine/renderstate.h"
#include "gengine/vertexarrayobject.h"
#include "gengine/drawobject.h"
#include "gengine/scenedataobject.h"
#include "gengine/vertexdata.h"
#include "gengine/shaderattributecollection.h"
#include "gengine/primitivetypes.h"
#include "mesh2.h"
#include "sphere.h"
#include "cuboid.h"
#include "cylinder.h"
#include "pyramid4.h"
#include "turtle.h"
//#include "gui/mainwindow.h"
//#include "gtkmm/main.h"

#include "abm/Agent.h"
#include "abm/LogoVariantOwns.h"
#include "abm/sensor/geofence.h"

#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>


//#define PROGRAM_NAME "GeoGL"

using namespace gengine;

const int FRAMES_PER_SECOND = 60;




//////////////////////////////////////////////////////////////////////
//GLFW code
//Why are we using GLFW in preference to SDL? Mainly because it's geared
//towards OpenGL, while SDL is a more general library that doesn't integrate
//so well with OpenGL even if it does provide some very nice cross
//platform functions.
//////////////////////////////////////////////////////////////////////
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}



//alternative main function for GLFW
int main(int argc, char *argv[])
{
	OGLDevice::Initialise(); //device initialisation

	//GraphicsContext* GC = OGLDevice::XCreateWindow(512,512);
	//VertexBuffer* vb=OGLDevice::CreateVertexBuffer("in_Position",ArrayBuffer,StaticDraw,3*4*sizeof(float));
	//float mem_vertices[] = {
	//	0,0,0,
	//	0,1,0,
	//	1,1,0,
	//	1,0,0
	//};
	//vb->CopyFromMemory(mem_vertices);
	//VertexBuffer* vc=OGLDevice::CreateVertexBuffer("in_Color",ArrayBuffer,StaticDraw,3*4*sizeof(float));
	//float mem_colours[] = {
	//	1.0, 0.0, 0.0,
	//	0.0, 1.0, 0.0,
	//	0.0, 0.0, 1.0,
	//	1.0, 1.0, 1.0
	//};
	//vc->CopyFromMemory(mem_colours);
	//IndexBuffer* ib=OGLDevice::CreateIndexBuffer(ElementArrayBuffer,StaticDraw,3*2*sizeof(unsigned int));
	//unsigned int mem_indices[] = {
	//	0,1,2,
	//	0,2,3
	//};
	//ib->CopyFromMemory(mem_indices);
	////int v_inPosition=0;
	////int v_inColor=1;
	////VertexArrayObject* vao = new VertexArrayObject();
	////vao->AddVertexAttribute(v_inPosition,"in_Position");
	////vao->AddVertexAttribute(v_inColor,"in_Color");
	//
	//VertexData* vertexData=new VertexData();
	////manual push of vertex attributes
	////vertexData->_vertexattributes->AddVertexAttribute("v_inPosition",v_inPosition,"");
	////vertexData->_vertexattributes->AddVertexAttribute("v_inColor",v_inColor,"");
	//
	//vertexData->_vb.push_back(vb); //push the vertex buffer
	//vertexData->_vb.push_back(vc); //push the colour buffer
	//vertexData->_ib=ib; //set the index buffer
	//vertexData->_NumElements=6; //6; //3=one triangle, 6=indexed square
	//RenderState* rs = new RenderState();
	//Shader* shader = new Shader("shader.vert", "shader.frag");
	////TODO: get shader uniforms and vertex attributes from here by querying the shader
	////the draw object is everything you need to draw an object
	//DrawObject tri;
	//tri._PrimType=ptTriangles;
	//tri._rs=rs;
	////tri._vao=vao;
	//tri._vertexData=vertexData;
	//tri._ShaderProgram=shader;
	//glm::mat4 mm=glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-2));
	//tri._ModelMatrix = mm;
	////TODO: need to delete these later

	

	//now scene specifics
	//Camera camera;
	//camera.SetupPerspective(512,512,1,100);
	//SceneDataObject sdo;
	//sdo._camera=&camera;

	//profile code
	//LARGE_INTEGER frequency;
	//LARGE_INTEGER t1,t2;
	//QueryPerformanceFrequency(&frequency);

	//Mesh2* spheres[300];
	//QueryPerformanceCounter(&t1);
	//double t1,t2;
	//t1 = glfwGetTime();
	//for (int i=0; i<300; ++i) {
	//	Mesh2* sphere = new Sphere(1.0,10,10);
	//	//Mesh2* sphere = new Cuboid(1,1,1);
	//	//Mesh2* sphere = new Turtle(1);
	//	//sphere->SetColour(glm::vec3(0,0,0));
	//	spheres[i]=sphere;
	//}
	//t2 = glfwGetTime();
	//std::cout<<(t2-t1)*1000.0<<" ms."<<std::endl;
	//QueryPerformanceCounter(&t2);
	//double elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
 //   std::cout << elapsedTime << " ms.\n";
	//for (int i=0; i<300; ++i) delete spheres[i];
	//end of profile code



	//GUI Test
	//Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmmm.example");
	//geogl::gui::MainWindow* win = new geogl::gui::MainWindow();
	//geogl::gui::MainWindow win;
	//yes, you probably want gtkglext .. there are c++ bindings to it named gtkglextmm
	//int x = app->run(win);
	//delete win;


	Globe globe;
	
	//build scene graph
	//OK, enough of the test objects, on to some real data. Let's start with London Underground.
	//globe method
	//ModelTubeNetwork* tn = new ModelTubeNetwork(globe.GetSceneGraph());
	//tn->Setup(); //who's responsible for this? globe or me?
	//globe.AddLayerModel(tn);
	//std::vector<ABM::Agent*> nodes = tn->_agents.Ask("node");
	//std::vector<ABM::Agent*> drivers = tn->_agents.Ask("driver");
	//std::cout<<"TubeModel: node="<<nodes.size()<<std::endl; //280 stations
	//std::cout<<"TubeModel: driver="<<drivers.size()<<std::endl; //370 tubes
	//std::cout<<"TubeModel: Links="<<tn->_links.NumLinks()<<std::endl; //write out the number of links between node - 716
	//
	//ModelMLTubeNetwork* tmln = new ModelMLTubeNetwork(globe.GetSceneGraph());
	//tmln->Setup(); //who's responsible for this? globe or me?
	//globe.AddLayerModel(tmln);

	//Bus Data
	ModelBusNetwork* bn = new ModelBusNetwork(globe.GetSceneGraph());
	bn->Setup();
	globe.AddLayerModel(bn);
	//ABM::sensor::GeoFence* MY1 = ABM::sensor::GeoFence::CreateCuboid(bn,glm::dvec3(-0.154611,51.522530,100),glm::dvec3(0.01,0.01,200));

	//Network Rail Data
	//ModelNetworkRail* rn = new ModelNetworkRail(globe.GetSceneGraph());
	//rn->Setup();
	//globe.AddLayerModel(rn);

	//correlation data
	//ModelCorrelate* cormodel = new ModelCorrelate(globe.GetSceneGraph());
	//cormodel->Setup();
	//globe.AddLayerModel(cormodel);

	//WX and AirQuality
	//ModelWXAirQ* wxairq = new ModelWXAirQ(globe.GetSceneGraph());
	//wxairq->Setup();
	//globe.AddLayerModel(wxairq);

	//Travel to work model
	//ModelTravelToWork* traveltowork = new ModelTravelToWork(globe.GetSceneGraph());
	//traveltowork->Setup();
	//globe.AddLayerModel(traveltowork);

	//World in WGS84
	//GeoJSON* geoj = new GeoJSON();
	//geoj->LoadFile("..\\GeoGL\\data\\TM_WORLD_BORDERS_SIMPL-0.3_WGS84.geojson");
	//std::cout<<"After geojson finaliser"<<std::endl;
	//GeoJSON* geoj = globe.LoadLayerGeoJSON("../data/TM_WORLD_BORDERS_SIMPL-0.3_WGS84.geojson");
	
	//Thames in WGS84
	GeoJSON* thames = globe.LoadLayerGeoJSON("../data/TQ_TidalWater_503500_155500.geojson");
	thames->SetColour(glm::vec3(0.0f,0.0f,1.0f)); //better make it blue

	//Buildings in WGS84
	//GeoJSON* buildings = globe.LoadLayerGeoJSON("../data/TQ_Building_530000_180000_WGS84.geojson");
	//GeoJSON* buildings = globe.LoadLayerGeoJSON("../cache/12_2045_2634.geojson");
	//GeoJSON* buildings = globe.LoadLayerGeoJSON("../cache/14_8188_10537.geojson");
	//buildings->SetColour(glm::vec3(0.5f,0.5f,0.5f));
	//buildings->SetColour(glm::vec3(1.0f,0.0f,0.0f));

	//Buildings from OBJ
	//Mesh2* buildings = new Mesh2();
	//buildings->_VertexFormat=PositionColourNormal;
	//buildings->FromOBJ("../data/vectortiles/14_8188_10537.obj"/*"../cache/14_8188_10537.obj"*/);
	//buildings->CreateBuffers();
	//buildings->AttachShader(globe.GetShader(3),true); //this is the vertex colour normal shader for geojson
	//buildings->SetColour(glm::vec3(1.0f,1.0f,1.0f));
	////buildings->debug_DrawNormals(32.0f); //force mesh to draw per vertex formals as lines
	//globe.GetSceneGraph()->push_back(buildings);

	//London outline in WGS84
	//GeoJSON* london = globe.LoadLayerGeoJSON("../data/London_dt_2001_area_WGS84.geojson");
	//london->SetColour(glm::vec3(1.0f,1.0f,1.0f));


	globe.FitViewToLayers(); //now all the data has been loaded, initialise the camera to its default position and zoom

	////////////////////////End of scene graph creation
	
	
	//this was the live block
	//OrbitController controller(&camera);
	//OrbitController controller(&globe.camera);
	//controller.centre=glm::vec3(-1.0f,0.0f,0.0f);
	//controller.centre=glm::vec3(-0.11464,51.46258,0); //Brixton

	//float radius = glm::distance(controller.origin,camera.GetPos());
	//FlyController controller(&camera);

	//LeapController controller(&camera);

	//glfwSetWindowSizeCallback(openglContext.window, windowsize_callback); 
	//glfwSetCursorPosCallback(openglContext.window, cursorpos_callback);
	//glfwSetMouseButtonCallback(openglContext.window, mousebutton_callback);
	//glfwSetScrollCallback(openglContext.window, scroll_callback);
	//glfwSetKeyCallback(openglContext.window, key_callback);

	//globe.LookAt("LondonUnderground");
	//globe.LookAt(/*"_AGENTS_"*/"_LINKS_"); //generic name of agents in scene
	//globe.LookAt("_AGENTS");
	

	while (globe.IsRunning()) {
		double startTicks = glfwGetTime();

		glfwPollEvents(); //or glfwWaitEvents(); ?

		//model update
		auto modelStartTicks=std::chrono::high_resolution_clock::now();
		globe.Step();
		auto modelEndTicks=std::chrono::high_resolution_clock::now();


		//rendering
		auto renderStartTicks=std::chrono::high_resolution_clock::now();
		globe.RenderScene();
		auto renderEndTicks=std::chrono::high_resolution_clock::now();

		//while( Gtk::Main::events_pending() )
		//Gtk::Main::iteration();

		//Cap the frame rate? would need to call poll events here
		double ticksNow = glfwGetTime();
		double fps = 1/(ticksNow-startTicks);
		globe._debugFPS=fps;
		//std::string strTime = AgentTime::ToString(tn->AnimationDT);
		//globe._debugMessage = strTime;

		//TIMING BLOCK
		auto modelMS=std::chrono::duration_cast<std::chrono::microseconds>(modelEndTicks-modelStartTicks);
		auto renderMS=std::chrono::duration_cast<std::chrono::microseconds>(renderEndTicks-renderStartTicks);
	}

	return 0;
}
