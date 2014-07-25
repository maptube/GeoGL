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
}

class Mesh2;

class TiledEarth : public Object3D
{
protected:
	static const int LODDepth;
	static const int LODWidthSegments;
	static const int LODHeightSegments;
	Mesh2* _root;
public:
	Ellipsoid _ellipsoid;

	TiledEarth(void);
	~TiledEarth(void);

	Mesh2* MakePatch(int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon);
	Mesh2* BuildChunkedLOD(int Depth, int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon);
	void Render(float Tau,gengine::Camera* camera);
	//void UpdateView(gengine::Camera* cam);
};

