/*
 * datacacheworker.h
 *
 *  Created on: 21 Oct 2014
 *      Author: richard
 */

#pragma once

#include "async/worker.h"
#include "async/messagequeueeventargs.h"
#include "async/messagequeue.h"

#include <string>
#include <set>

namespace geogl {
namespace cache {

//message passed to the worker to specify the files to move and the queue to push the done message onto
class DataCacheWorkerMsg : public geogl::async::WorkerMsg {
public:
	std::string src;
	std::string dst;
	DataCacheWorkerMsg(const std::string& src, const std::string& dst);
};

//worker for obtaining files from a remote location and putting them into the local cache
class DataCacheWorker : public geogl::async::Worker {
public:
	std::string _CacheDir;
	geogl::async::MessageQueue& doneQueue;
	//DataCacheWorker() {};
	DataCacheWorker(geogl::async::MessageQueue& msgQ);
	virtual ~DataCacheWorker();
	virtual void Process(geogl::async::MessageQueueEventArgs& args);
};

//worker for putting files into the index after they have been loaded by the other worker
//this runs on the same thread as the DataCache
class DataCacheIndexWorker : public geogl::async::Worker {
public:
	std::set<std::string>& _FileIndex;
	std::set<std::string>& _RequestIndex;
	DataCacheIndexWorker(std::set<std::string>& fileIndex, std::set<std::string>& requestIndex);
	virtual ~DataCacheIndexWorker();
	virtual void Process(geogl::async::MessageQueueEventArgs& args);
};

} // namespace cache
} // namespace geogl


