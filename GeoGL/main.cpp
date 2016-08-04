/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */

#include "main.h"
#include "gengine/ogldevice.h"
#include "globe.h"
#include "ellipsoid.h"
//#include "opengl4.h"
//and this is very awkward - need to separate from main code so user software uses globe, not this way around
#include "../usr/model_tubenetwork.h"
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

//extern "C" {
//#include "GLFW/glfw3.h"
//GLFWAPI void glfwTerminate(void);
//GLFWAPI double glfwGetTime(void);
//}

//#define PROGRAM_NAME "GeoGL"

using namespace gengine;

const int FRAMES_PER_SECOND = 60;

//OpenGLContext openglContext; // Our OpenGL Context class



//new entry point for platform independent code
//NOTE: SDL nastiness in #defining main as SDL_main to overload its own initialisation. Also needs C linkage.
//Code is a copy from: http://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_(C_/_SDL)
/*#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[])
{
	openglContext.create30ContextSDL(); // Create a window and an OpenGL context to run in it
	openglContext.setupScene(); // Setup our OpenGL scene
	
	int startTicks=SDL_GetTicks();
	bool running = true;
	SDL_Event evt;
	while (running)
	{
		while(SDL_PollEvent(&evt)) {
			if (evt.type == SDL_QUIT)
            {
				//quit is flagged on a window close
                running=false;
            }
			//check if the event type is a window
			else if (evt.type == SDL_WINDOWEVENT) {
				switch (evt.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						openglContext.reshapeWindow(evt.window.data1, evt.window.data2); // Send the new window size to our OpenGLContext
					break;
				}
			}
			//check if event type is keyboard press
			else if (evt.type == SDL_KEYDOWN){
				if(evt.key.keysym.sym == SDLK_ESCAPE){
					running = false;
				}
			}
		}

		//no messages to process, so render in the idle
		openglContext.renderScene();

		//Cap the frame rate
		int ticksNow = SDL_GetTicks();
		float fps = (ticksNow-startTicks)/1000;
        if ((ticksNow-startTicks) < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - (ticksNow-startTicks) );
        }

	}

	openglContext.destroyContextSDL();
	
	return 0;
}
*/


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

//create a fake version of London
void fakeLondon(Globe& globe) {
	/*SceneGraphType* SG = globe.GetSceneGraph();
	Ellipsoid e; //bit naughty, shouldn't globe do the coord conversion
	Cylinder* eye = new Cylinder(10,100,20);
	glm::translate(eye->modelMatrix,glm::vec3(0,100,0)); //move it up so the base is on the ground
	glm::vec3 eye_pos = e.toVector(-0.119752*3.141592654/180.0,51.503313*3.141592654/180.0,0);
	eye->Rotate(90.0*3.141592654/180.0,glm::vec3(0,1,0)); //tip it on its end
	eye->Rotate(-85*3.141592654/180.0,glm::vec3(1,0,0)); //angle to river
	eye->Rotate(-51.503313*3.141592654/180,glm::vec3(0,0,1)); //azimuthal rotation
	eye->SetPos(eye_pos.x,eye_pos.y,eye_pos.z);
	eye->AttachShader(globe.GetShader(0),true); //presumably shader 0 is the right one?
	SG->push_back(eye);

	Pyramid4* shard = new Pyramid4(100,306,100);
	shard->Rotate(90*3.141592654/180,glm::vec3(1,0,0)); //rotate it so it's pointing up
	glm::translate(shard->modelMatrix,glm::vec3(0,100,0)); //move it up so the base is on the ground
	glm::vec3 shard_pos = e.toVector(-0.086598*3.141592654/180.0,51.504358*3.141592654/180.0,0);
	shard->Rotate(-51.504358*3.141592654/180,glm::vec3(0,0,1)); //azimuthal rotation
	shard->SetPos(shard_pos.x,shard_pos.y,shard_pos.z);
	shard->AttachShader(globe.GetShader(0),true); //presumably shader 0 is the right one?
	SG->push_back(shard);*/
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

	//agent variant testing
	//ABM::Agent* A = new ABM::Agent();
	//A->Set<int>("Test1",4);
	//int Test1 = A->Get<int>("Test1");
	//std::cout<<"Test1="<<Test1<<std::endl;

	//std::cout<<(typeid(std::string).hash_code())<<" "<<(typeid(std::string*).hash_code())<<std::endl;

	//A->Set<bool>("Test2",false);
	//bool Test2 = A->Get<bool>("Test2");
	//std::cout<<"Test2="<<Test2<<std::endl;

	//A->Set<float>("Test3",0.123f);
	//float Test3 = A->Get<float>("Test3");
	//std::cout<<"Test3="<<Test3<<std::endl;

	//std::string str = "test 4 lvar test";
	//ABM::LogoVariant LVar("const string test");
	//std::string Test41 = LVar.Get<std::string>();
	//std::cout<<"Lvar="<<Test41<<std::endl;

	//std::string str2 = "test 4 string test";
	//A->Set<std::string>("Test4","test 4 string test");
	//std::string Test4 = A->Get<std::string>("Test4");
	//std::cout<<"Test4="<<Test4<<std::endl;
	//delete A;

	//GUI Test
	//Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmmm.example");
	//geogl::gui::MainWindow* win = new geogl::gui::MainWindow();
	//geogl::gui::MainWindow win;
	//yes, you probably want gtkglext .. there are c++ bindings to it named gtkglextmm
	//int x = app->run(win);
	//delete win;


	Globe globe;
	//fakeLondon(globe);

	
	//build scene graph
	//OK, enough of the test objects, on to some real data. Let's start with London Underground.
	//globe method
	ModelTubeNetwork* tn = new ModelTubeNetwork(globe.GetSceneGraph());
	tn->Setup(); //who's responsible for this? globe or me?
	globe.AddLayerModel(tn);
	//std::vector<ABM::Agent*> nodes = tn->_agents.Ask("node");
	//std::vector<ABM::Agent*> drivers = tn->_agents.Ask("driver");
	//std::cout<<"TubeModel: node="<<nodes.size()<<std::endl; //280 stations
	//std::cout<<"TubeModel: driver="<<drivers.size()<<std::endl; //370 tubes
	//std::cout<<"TubeModel: Links="<<tn->_links.NumLinks()<<std::endl; //write out the number of links between node - 716

	//Bus Data
	//ModelBusNetwork* bn = new ModelBusNetwork(globe.GetSceneGraph());
	//bn->Setup();
	//globe.AddLayerModel(bn);
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
	//GeoJSON* thames = globe.LoadLayerGeoJSON("../data/TQ_TidalWater_503500_155500.geojson");
	//thames->SetColour(glm::vec3(0.0f,0.0f,1.0f)); //better make it blue

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
	//buildings->SetColour(glm::vec3(1.0f,0.0f,0.0f));
	////buildings->debug_DrawNormals(32.0f); //force mesh to draw per vertex formals as lines
	//globe.GetSceneGraph()->push_back(buildings);

	//London outline in WGS84
	//GeoJSON* london = globe.LoadLayerGeoJSON("../data/London_dt_2001_area_WGS84.geojson");
	//london->SetColour(glm::vec3(1.0f,1.0f,1.0f));


	//globe.FitViewToLayers(); //now all the data has been loaded, initialise the camera to its default position and zoom

	////////////////////////End of scene graph creation
	
	//EventManager& eventmanager = EventManager::getInstance(); //singleton pattern
	//eventmanager.SetWindow(globe.openglContext.window); //initialise event system with OpenGL window handle
	//eventmanager.AddWindowSizeEventListener(&openglContext);
	
	
	//Camera camera;
	//camera.SetupPerspective(openglContext.GetWindowWidth(), openglContext.GetWindowHeight(), 0.01f, 200.0f);  // Create our perspective matrix
	//camera.viewMatrix = openglContext.fitViewMatrix();
	//above was the live block - now in globe
	
	//camera.SetCameraPos(glm::vec3(-1.0f,0.0f,5.0f)); //looking straight at the box
	//camera.LookAt(glm::vec3(-1.0f,0.0f,0.0f));
	//camera.SetCameraPos(glm::vec3(-0.1,50,0));
	//camera.LookAt(glm::vec3(-0.11464,51.46258,0));

	//glm::vec3 vCamera = camera.GetCameraPos();
	//std::cout<<"Camera: "<<vCamera.x<<" "<<vCamera.y<<" "<<vCamera.z<<std::endl;
	
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
	
	//TIMING BLOCK
	//std::ofstream out_lograw("/home/richard/main-stats.txt",std::ios::app);
	//out_lograw<<"TIMING, fps, modeltime us, rendertime us, drivers, birth, death"<<std::endl;

	while (globe.IsRunning()) {
		double startTicks = glfwGetTime();

		glfwPollEvents(); //or glfwWaitEvents(); ?

		//tri._ModelMatrix=glm::rotate(tri._ModelMatrix,0.001f,glm::vec3(0,1,0));

		//todo: need to get the clear colour out!
		//glClearColor(0.4f, 0.6f, 0.9f, 0.0f); // Set the clear color based on Microsoft's CornflowerBlue (default in XNA)
		//GC->Clear();
		//GC->Render(tri,sdo);
		//GC->Render(mesh2->drawObject,sdo);
		//GC->SwapBuffers();

		//model update
		auto modelStartTicks=std::chrono::high_resolution_clock::now();
		globe.Step();
		auto modelEndTicks=std::chrono::high_resolution_clock::now();

		//set viewpoint to first agent
		//V_3_211 or V_3_202 or D_1_24 or D_1_42
		//std::vector<ABM::Agent*> a = tn->_agents.With("name","D_1_42");
		//globe.camera.LookAt(a[0]->GetXYZ());
		//glm::mat4 m = a[0]->_pAgentMesh->modelMatrix;
		//globe.camera.SetCameraMatrix(glm::dmat4(m));

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
		//std::cout<<"TIMING,"<<startTicks<<","<<modelStartTicks<<","<<modelEndTicks<<","<<renderStartTicks<<","<<renderEndTicks<<","<<ticksNow<<std::endl;
		auto modelMS=std::chrono::duration_cast<std::chrono::microseconds>(modelEndTicks-modelStartTicks);
		auto renderMS=std::chrono::duration_cast<std::chrono::microseconds>(renderEndTicks-renderStartTicks);
		//std::vector<ABM::Agent*> drivers = tn->_agents.Ask("driver"); //how long does this take?
		//out_lograw<<AgentTime::ToString(tn->AnimationDT)<<","<<fps<<","<<modelMS.count()<<","<<renderMS.count()<<","<<drivers.size()<<","<<tn->_agents.Birth<<","<<tn->_agents.Death<<std::endl;
	}

	//TIMING BLOCK
	//out_lograw.close();

	return 0;
}
