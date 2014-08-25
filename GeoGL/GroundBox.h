#pragma once

//this is basically a copy of the TiledEarth class, except that it builds a quadtree hierarchy that sits on the ground

#include "object3d.h"
#include "mesh2.h"

//forward references
namespace gengine {
	class Camera;
	class SceneDataObject;
	class GraphicsContext;
}

class GroundBox : public Object3D {
private:
	Mesh2* _meshes[9];
protected:
public:
	GroundBox();
	virtual ~GroundBox();

	void LonLatToTileXY(glm::dvec3 RadGeodetic3D,int& TileX,int& TileY);
	virtual void Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo);
	virtual bool HasGeometry() { return true; } //yes, this object has geometry associated with it and can be rendered
};

