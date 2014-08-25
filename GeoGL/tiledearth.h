#pragma once

#include "object3d.h"
#include "mesh2.h"
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
	class IndexBuffer;
}

class Mesh2;

//TiledEarthNode is a wrapper for a Mesh2 child object containing a renderable patch. Min/Max and scaling coords have been added for rendering.
class TiledEarthNode : public Mesh2
{
public:
	//double MinLat, MaxLat, MinLon, MaxLon;
	//double ScaleLat, ScaleLon;
	//texture offset and scale in texture coordinates
	glm::vec2 TextureOffset;
	glm::vec2 TextureScale;
	//TODO: could add the delta value here as well
	//ALSO, add the tile XYZ here
};

class TiledEarth : public Object3D
{
protected:
	static const int LODDepth;
	static const int LODWidthSegments;
	static const int LODHeightSegments;
	int _MeshCount;
	float _MinDelta; //this is the delta value at the maximum level of detail
	TiledEarthNode* _root; //this is a wrapper for a Mesh2
	int _NumElements; //number of face elements used for rendering
	gengine::IndexBuffer* _ib; //index buffer shared by all meshes
	gengine::IndexBuffer* CreateIndexBuffer(const int WidthSegments,const int HeightSegments);
	glm::vec2 ComputeTextureCoordinate(glm::vec3 P);
	void RenderLOD(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo,TiledEarthNode* mesh,float K,float Delta);
public:
	Ellipsoid _ellipsoid;
	double _Tau; //error tolerance factor for rendering
	std::string _TextureBaseName; //the base name for the texture that we replace the XYZ in e.g. land_ocean_ice_QUAD_{Z}_{X}_{Y}.jpg

	TiledEarth(void);
	~TiledEarth(void);

	virtual void Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo);
	virtual bool HasGeometry() { return true; } //yes, this object has geometry associated with it and can be rendered
	virtual void AttachShader(gengine::Shader* pShader, bool Recursive); //apply shader bind to root
	void AttachTexture(unsigned int TextureUnitNum, gengine::Texture2D* Texture);

	TiledEarthNode* MakePatch(int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon);
	TiledEarthNode* BuildChunkedLOD(int Depth, int TileZ, int TileX, int TileY, int WidthSegments, int HeightSegments, double MinLat, double MinLon, double MaxLat, double MaxLon);
};

