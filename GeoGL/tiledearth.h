#pragma once

#include "object3d.h"
//#include "mesh2.h"
#include "ellipsoid.h"

//Earth model for geometry and texture data based on tiling the Earth to a threshold level of detail for a view volume
//Basically, it's a mesh object that can remesh itself
//But only the child objects are drawable

//forward references
namespace gengine {
	class Camera;
	class SceneDataObject;
	class GraphicsContext;
	class Shader;
}

class Mesh2;

class TiledEarth : public Object3D
{
protected:
	static const int LODDepth;
	static const int LODWidthSegments;
	static const int LODHeightSegments;
	int _MeshCount;
	Mesh2* _root;
	void RenderLOD(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo,Mesh2* mesh,float K,float Delta);
public:
	Ellipsoid _ellipsoid;
	double _Tau; //error tolerance factor for rendering

	TiledEarth(void);
	~TiledEarth(void);

	virtual void Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo);
	virtual bool HasGeometry() { return true; } //yes, this object has geometry associated with it and can be rendered
	virtual void AttachShader(gengine::Shader* pShader, bool Recursive); //apply shader bind to root

	Mesh2* MakePatch(int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon);
	Mesh2* BuildChunkedLOD(int Depth, int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon);
};

