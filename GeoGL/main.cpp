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
#include "opengl4.h"
#include "model_tubenetwork.h"
#include "netgraphgeometry.h"
#include "geojson.h"
#include "gengine/events/EventManager.h"
#include "OrbitController.h"
#include "FlyController.h"
#include "LeapController.h"
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

//static void windowsize_callback(GLFWwindow *window, int w, int h)
//{
//	openglContext.reshapeWindow(w, h); // Send the new window size to our OpenGLContext
//}

//static void mousebutton_callback(GLFWwindow *window, int button, int action, int mods)
//{
//	//std::cout<<button<<" "<<action<<" "<<mods<<std::endl;
//}

//static void cursorpos_callback(GLFWwindow *window, double mx, double my)
//{
//	//cursor position means the mouse in GLFW
//	//std::cout<<mx<<","<<my<<std::endl;
//}

//static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
//{
//	//std::cout<<"scroll "<<xoffset<<" "<<yoffset<<std::endl;
//}

//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}

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

	Globe globe;
	
	
	//openglContext.setupScene(); // Setup our OpenGL scene
	//build scene graph
	//OK, enough of the test objects, on to some real data. Let's start with London Underground.
	//globe method
	//ModelTubeNetwork* tn = new ModelTubeNetwork(globe.GetSceneGraph());
	//tn->Setup(); //who's responsible for this? globe or me?
	//globe.AddLayerModel(tn);
	
	////tn->load(
	////	"..\\GeoGL\\data\\tube-network.json",
	////	"..\\GeoGL\\data\\station-codes.csv");
	////NetGraphGeometry* lu=new NetGraphGeometry(tn->tube_graph,tn->tube_stations,0.00025f,10);
	////openglContext.SceneGraph.push_back(lu); //add relative to origin
	//Object3D* lu = tn->GenerateMesh();
	//openglContext.SceneGraph.push_back(lu);
	////NetGraphGeometry* lu = tn->GenerateLineMesh('D');
	////openglContext.SceneGraph.push_back(lu);
	////NetGraphGeometry* lu2 = tn->GenerateLineMesh('N');
	////openglContext.SceneGraph.push_back(lu2);

	//World in WGS84
	//GeoJSON* geoj = new GeoJSON();
	//geoj->LoadFile("..\\GeoGL\\data\\TM_WORLD_BORDERS_SIMPL-0.3_WGS84.geojson");
	//std::cout<<"After geojson finaliser"<<std::endl;
	//GeoJSON* geoj = globe.LoadLayerGeoJSON("..\\data\\TM_WORLD_BORDERS_SIMPL-0.3_WGS84.geojson");
	
	//Thames in WGS84
	//GeoJSON* thames = new GeoJSON();
	//thames->LoadFile("..\\GeoGL\\data\\TQ_TidalWater_503500_155500.geojson");
	//thames->SetColour(glm::vec3(0.0f,0.0f,1.0f)); //better make it blue
	//GeoJSON* thames = globe.LoadLayerGeoJSON("..\\data\\TQ_TidalWater_503500_155500.geojson");
	//thames->SetColour(glm::vec3(0.0f,0.0f,1.0f)); //better make it blue

	//London outline in WGS84
	//GeoJSON* london = new GeoJSON();
	//london->LoadFile("..\\GeoGL\\data\\London_dt_2001_area_WGS84.geojson");
	//openglContext.SceneGraph.push_back(london);
	//GeoJSON* london = globe.LoadLayerGeoJSON("..\\GeoGL\\data\\London_dt_2001_area_WGS84.geojson");

	globe.FitViewToLayers(); //now all the data has been loaded, initialise the camera to its default position and zoom

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
	
	//float a=0;
	float startTicks = glfwGetTime();
	while (globe.IsRunning()) {
		glfwPollEvents(); //or glfwWaitEvents(); ?

		//tri._ModelMatrix=glm::rotate(tri._ModelMatrix,0.001f,glm::vec3(0,1,0));

		//todo: need to get the clear colour out!
		//glClearColor(0.4f, 0.6f, 0.9f, 0.0f); // Set the clear color based on Microsoft's CornflowerBlue (default in XNA)
		//GC->Clear();
		//GC->Render(tri,sdo);
		//GC->Render(mesh2->drawObject,sdo);
		//GC->SwapBuffers();

		//model update
		//tn->Step();

		//rendering
		//camera.viewMatrix = openglContext.fitViewMatrix();
		//controller.Forward();
		//openglContext.SceneGraph.at(0)->Rotate(0.0001f,glm::vec3(0.0f,1.0f,0.0f)); //@50FPS, this is 0.29 degrees per second
		
		//openglContext.renderScene(&camera);
		globe.RenderScene();

		//camera.viewMatrix[3][0]=10*glm::sin<float>(a);
		//camera.viewMatrix[3][1]=0;
		//camera.viewMatrix[3][2]=10*glm::cos<float>(a);
		//camera.LookAt(glm::vec3(-1.0f,0,0));
		//a+=0.0003;
		//glm::vec3 cp = camera.GetCameraPos();
		//cp.x-=0.001;
		//camera.SetCameraPos(cp);

		//glm::vec3 centre(-1.0f,0,0);
		//glm::mat4 mCam = camera.GetCameraMatrix();
		////mCam = glm::translate(mCam,-centre); //translate camera so it's centred on the origin
		//mCam[3][0]-=centre.x; mCam[3][1]-=centre.y; mCam[3][2]-=centre.z;
		//glm::mat4 m2 = glm::eulerAngleYX(0.001f,0.0f); //Yaw rotation
		//mCam = m2 * mCam;
		////mCam=glm::translate(mCam,centre); //translate camera back again
		//mCam[3][0]+=centre.x; mCam[3][1]+=centre.y; mCam[3][2]+=centre.z;
		//camera.SetCameraMatrix(mCam);

		//Cap the frame rate
		//int ticksNow = glfwGetTime();
		//float fps = (ticksNow-startTicks)/1000;
        //if ((ticksNow-startTicks) < 1000 / FRAMES_PER_SECOND )
        //{
		//	//NO SUCH THING glfwSleep(( 1000 / FRAMES_PER_SECOND ) - (ticksNow-startTicks) );
        //}
	}
	//openglContext.destroyContextGLFW();
	//glfwTerminate();
	return 0;
}
