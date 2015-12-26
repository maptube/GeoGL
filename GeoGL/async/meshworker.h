/*
 * meshworker.h
 *
 *  Created on: 29 Nov 2014
 *      Author: richard
 */

#pragma once

#include "async/worker.h"
#include "async/messagequeueeventargs.h"
#include "async/messagequeue.h"

#include "mesh2.h"

#include <iostream>
#include <memory>

namespace geogl {
namespace async {

class MeshWorkerMsg : public geogl::async::WorkerMsg {
public:
	std::string src;
	int TileX, TileY, TileZ; //used to identify tiled meshes if required
	std::unique_ptr<Mesh2> mesh;
	//we need to fill in a mesh structure from an input file here
	MeshWorkerMsg(const std::string& src);
	MeshWorkerMsg(const std::string& src, int tileX, int tileY, int tileZ);
};

/////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// worker class for background loading of meshes - gets passed a MeshWorkerMsg, loads the mesh and passes it back
/// </summary>
class MeshWorker : public geogl::async::Worker {
public:
	geogl::async::MessageQueue& doneQueue;
	gengine::GraphicsContext* workerGC; //make this into a shared_ptr?
	//MeshWorker() {};
	MeshWorker(geogl::async::MessageQueue& msgQ, gengine::GraphicsContext* GC);
	virtual ~MeshWorker();
	virtual void Process(geogl::async::MessageQueueEventArgs& args);
};

} // namespace async
} // namespace geogl
