#pragma once

/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */

#include <string>
#include <vector>

#include "ellipsoid.h"
//#include "opengl4.h"
#include "gengine/Camera.h"
#include "BBox.h"

//forward declarations
//class GLFWWindow;
class Object3D;
class Cuboid;
class GeoJSON;
class OrbitController;
class EllipsoidOrbitController;

namespace gengine {
	class GraphicsContext;
	class SceneDataObject;
	class Shader;
}

namespace ABM {
	class Model;
}

////////////////////////////////////////////////////////////////

typedef std::vector<Object3D*> SceneGraphType;

////////////////////////////////////////////////////////////////

/// <summary>
/// Virtual Globe class which wraps up all the functions required to render the globe and draw the data on the window
/// </summary>
class Globe //: public EventListener
{
private:
	Ellipsoid ellipsoid;
	//OpenGLContext openglContext;
	gengine::GraphicsContext* GC; //the context that we can render to
	gengine::SceneDataObject* _sdo; //scene data objects i.e. camera

	std::vector<gengine::Shader*> _Shaders; //Shader programs that have been complied and linked

	//OrbitController* controller;
	EllipsoidOrbitController* controller;

	//std::vector<Object3D*> SceneGraph; //list of object to be rendered on "renderScene"
	SceneGraphType SceneGraph;

	double _lastModelRunTime; //this is the last time that the models in the modelLayers were run
	std::vector<ABM::Model*> modelLayers;
	//NOTE: the whole time synchronisation thing is a bit of a nightmare - do you have a separate time for each layer?
	//How do you cope with a faster than real-time visualisation? Do you code it into each model layer, or just run the
	//clock fast? You could pass the delta time multiplied by some factor.

	void DestroyScene(void);
	void RenderChildren(Object3D* Parent, double nearClip, double farClip);
public:
	gengine::Camera camera;
	Cuboid* debugCube1;
	Cuboid* debugCube2;

	Globe(void);
	~Globe(void);

	void debugPositionCube(int Num, float X, float Y, float Z);

	bool IsRunning(void);
	//GLFWwindow* GetWindow() { return openglContext.window; }
	SceneGraphType* GetSceneGraph() { return &SceneGraph; }

	GeoJSON* LoadLayerGeoJSON(std::string Filename);
	void LoadLayerKML(std::string Filename);
	void LoadLayerShapefile(std::string Filename);

	void AddLayerModel(ABM::Model* model);

	void FitViewToLayers(void);
	glm::mat4 FitViewMatrix(void);
	glm::mat4 FitViewMatrix2(void);
	glm::mat4 FitViewMatrix2(BBox& bbox);
	void LookAt(std::string Name);

	void RenderScene(void);
	void Step(void);


};

