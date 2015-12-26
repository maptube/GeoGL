#pragma once

//this builds and maintains a set of lat lon based boxes of geojson content that contain the buildings
//it follows the clipmap terrain idea of moving around the geometry that the viewpoint is over

#include "object3d.h"
#include "mesh2.h"
#include "async/messagequeue.h"
#include "async/worker.h"
#include "async/meshworker.h"

#include <memory>

//forward references
namespace gengine {
	class Camera;
	class SceneDataObject;
	class GraphicsContext;
	class Shader;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BoxContent {
	bool IsEmpty;
	bool IsLoading;
	int TileX,TileY,TileZ;
	std::unique_ptr<Mesh2> mesh;
	BoxContent() { TileX=0; TileY=0; TileZ=0; mesh=nullptr; IsEmpty=true; IsLoading=false; };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////

///<summary>
/// GroundBoxMeshWorker
/// Puts loaded meshes from the done queue into the correct places in the ground box structure (BoxContent)
/// TODO: should really be private and the 9 should NOT be hard coded
///</summary>
class GroundBoxMeshWorker : public geogl::async::Worker {
private:
	BoxContent* _boxes; //array of [9];
	gengine::Shader* _Shader;
public:
	GroundBoxMeshWorker(BoxContent* boxes, gengine::Shader* shader) : _boxes(boxes), _Shader(shader) {}
	void Process(geogl::async::MessageQueueEventArgs& args) {
		//take the payload of the completed message and apply the new mesh to the relevant box
		std::cout<<"GroundBoxMeshWorker: In GroundBoxMeshWorker"<<std::endl;
		geogl::async::MeshWorkerMsg* meshmsg = dynamic_cast<geogl::async::MeshWorkerMsg*>(args.umessage.get());
		std::cout<<"GroundBoxMeshWorker: "<<meshmsg->src<<" "<<meshmsg->TileZ<<" "<<meshmsg->TileX<<" "<<meshmsg->TileY<<std::endl;
		if (!args.Success) return;
		for (int i=0; i<9; i++) {
			if ((_boxes[i].IsEmpty)
				&&(_boxes[i].TileX==meshmsg->TileX)
				&&(_boxes[i].TileY==meshmsg->TileY)
				&&(_boxes[i].TileZ==meshmsg->TileZ)) {
				std::cout<<"GroundBoxMeshWorker: Mesh moved "
						<<_boxes[i].TileZ<<" "<<_boxes[i].TileX<<" "<<_boxes[i].TileY<<std::endl;
				_boxes[i].mesh=std::move(meshmsg->mesh);
				_boxes[i].mesh->AttachShader(_Shader,true);
				_boxes[i].mesh->SetColour(glm::vec3(1.0f,0.0f,0.0f));
				_boxes[i].IsEmpty=false;
				_boxes[i].IsLoading=false;
				std::cout<<"GroundBoxMeshWorker: Mesh moved end "
						<<_boxes[i].TileZ<<" "<<_boxes[i].TileX<<" "<<_boxes[i].TileY<<std::endl;
				//std::cout<<"Mesh: vertices="<<_boxes[i].mesh.get()<<std::endl;
				break;
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

class GroundBox : public Object3D {
private:
	gengine::GraphicsContext* _workerGC; //graphics context for the mesh loader background thread - make this into a shared_ptr?
	gengine::Shader* _Shader; //shader used for rendering
	//message queues for loading meshes in the background
	geogl::async::MessageQueue _requestQueue;
	geogl::async::MessageQueue _doneQueue;
	BoxContent _gndboxes[9];
	Mesh2* DebugMesh(const int TileZ,const int TileX,const int TileY);
protected:
	void ShuffleBoxes(const int TileZ, const int TileX, const int TileY);
	void ProcessLoadedMeshes();
	void UpdateData(const gengine::SceneDataObject& sdo);
public:
	static const int BoxZoomLevel; //zoom level used for all boxes (i.e. switchable between 12 and 13)

	GroundBox(gengine::GraphicsContext* workerGC);
	virtual ~GroundBox();

	void SetShader(gengine::Shader* shader);

	void LonLatToTileXY(glm::dvec3 RadGeodetic3D,int& TileX,int& TileY);
	virtual void Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo);
	virtual bool HasGeometry() { return true; } //yes, this object has geometry associated with it and can be rendered
};

