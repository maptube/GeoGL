#pragma once

namespace gengine {

//forward definitions
class Camera;

//Data about a scene that needs to be passed in to the rendering code. For example, camera, view and projection.
class SceneDataObject
{
public:
	SceneDataObject(void);
	~SceneDataObject(void);

	Camera* _camera;

};

}