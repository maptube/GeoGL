/*
 * meshworker.cpp
 *
 *  Created on: 29 Nov 2014
 *      Author: richard
 */

#include "meshworker.h"

#include <memory>

#include "mesh2.h"


namespace geogl {
namespace async {

MeshWorkerMsg::MeshWorkerMsg(const std::string& src) : src(src) {
	mesh=nullptr;
}

MeshWorkerMsg::MeshWorkerMsg(const std::string& src, int tileX, int tileY, int tileZ) : src(src), TileX(tileX), TileY(tileY), TileZ(tileZ) {
	mesh=nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MeshWorker::MeshWorker(geogl::async::MessageQueue& msgQ, gengine::GraphicsContext* GC) : doneQueue(msgQ), workerGC(GC) {
	// TODO Auto-generated constructor stub
}


MeshWorker::~MeshWorker() {
	// TODO Auto-generated destructor stub
}

void MeshWorker::Process(geogl::async::MessageQueueEventArgs& args) {
	std::cout<<"MeshWorker: In MeshWorker"<<std::endl;
	geogl::async::MeshWorkerMsg* meshmsg = dynamic_cast<geogl::async::MeshWorkerMsg*>(args.umessage.get());
	std::cout<<"MeshWorker: loading "<<meshmsg->src<<std::endl;

	//load pre-computed obj file - this should also be in a thread
	Mesh2* mesh = new Mesh2();
	mesh->_VertexFormat=gengine::PositionColourNormal;
	mesh->FromOBJ(meshmsg->src);
//HACK! this needs to be a context method
	glfwMakeContextCurrent(workerGC->window);
	mesh->CreateBuffers();
	glFinish();
	std::unique_ptr<Mesh2> p(mesh);
	meshmsg->mesh = std::move(p);
	std::cout<<"MeshWorker: Mesh Loaded"<<std::endl;

	args.Success=true;
	//and finally, push the message onto the done queue to be picked up by the parent process
	doneQueue.Post(args);
	std::cout<<"MeshWorker: complete "<<std::endl;
}

} // namespace async
} // namespace geogl
