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

//forward declarations
//class GLFWWindow;
class Object3D;
class GeoJSON;

namespace gengine {
	class GraphicsContext;
	class SceneDataObject;
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

	//std::vector<Object3D*> SceneGraph; //list of object to be rendered on "renderScene"
	SceneGraphType SceneGraph;

	std::vector<ABM::Model*> modelLayers;

	void DestroyScene(void);
public:
	gengine::Camera camera;

	Globe(void);
	~Globe(void);

	bool IsRunning(void);
	//GLFWwindow* GetWindow() { return openglContext.window; }
	//SceneGraphType* GetSceneGraph() { return &openglContext.SceneGraph; }

	GeoJSON* LoadLayerGeoJSON(std::string Filename);
	void LoadLayerKML(std::string Filename);
	void LoadLayerShapefile(std::string Filename);

	void AddLayerModel(ABM::Model* model);

	void FitViewToLayers(void);

	void RenderScene(void);


};

