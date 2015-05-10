/*
 * datacacheworker.cpp
 *
 *  Created on: 21 Oct 2014
 *      Author: richard
 */

#include "datacacheworker.h"
#include "async/messagequeue.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <set>

namespace geogl {
namespace cache {

DataCacheWorkerMsg::DataCacheWorkerMsg(const std::string& src, const std::string& dst) : src(src), dst(dst) {

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataCacheWorker::DataCacheWorker(geogl::async::MessageQueue& msgQ) : doneQueue(msgQ) {
	// TODO Auto-generated constructor stub

}

DataCacheWorker::~DataCacheWorker() {
	// TODO Auto-generated destructor stub
}

void DataCacheWorker::Process(geogl::async::MessageQueueEventArgs& args) {
	std::cout<<"in worker class"<<std::endl;
	//struct DataCacheWorkerMsg* filemsg = (struct DataCacheWorkerMsg*)args.umessage;

	//std::shared_ptr<DataCacheWorkerMsg> filemsg = (std::shared_ptr<DataCacheWorkerMsg>)(args.umessage);


	//void p2 = args.umessage;
	//DataCacheWorkerMsg* p3 = static_cast<std::shared_ptr<DataCacheWorkerMsg*>>args.umessage;

	//geogl::async::WorkerMsg* p = (args.umessage);
	//std::string src = (DataCacheWorkerMsg*)p->src;
	//std::string src = ((DataCacheWorkerMsg*)(args.umessage))->src;
	//const std::shared_ptr<geogl::async::WorkerMsg> msg = args.umessage;
	//std::string test = msg.get().test;
	//auto msg = (args.umessage)->test;
	//std::string test = msg->test;
	//std::shared_ptr<geogl::async::WorkerMsg> p(args.umessage);
	//std::string test = p->test;
	//std::string test = args.umessage->test;
	//std::shared_ptr<geogl::async::WorkerMsg> msg = args.umessage;
	//std::string test = msg->test;
	geogl::cache::DataCacheWorkerMsg* filemsg = dynamic_cast<geogl::cache::DataCacheWorkerMsg*>(args.umessage.get());
	std::cout<<"DataCacheWorker "<<filemsg->src<<" and "<<filemsg->dst<<std::endl;


	//TODO: should probably copy to a temporary file and rename?
	std::ifstream in(filemsg->src,std::ios::in|std::ios::binary|std::ios::ate); //use binary flag as we might be handling textures etc.
	if (!in.is_open()) { args.Success=false; return; } //fail
	std::ifstream::pos_type pos=in.tellg();

	std::vector<char> data(pos);
	in.seekg(0,std::ios::beg);
	in.read(&data[0],pos); //read all data into memory - don't like this much, but then all the files are designed to be small

	//now write it out to the new file
	std::ofstream out(filemsg->dst, std::ios::out | std::ios::binary);
	out.write(&data[0],pos);

	//update file map with the new file we've just added
	//TODO: do you need to synchronise this? The file is copied before it's set so there's no race.
	//_FileIndex.insert(src);
	//_RequestIndex.erase(src); //remove from waiting request list
	std::cout<<"Move complete "<<filemsg->src<<std::endl;

	args.Success=true;
	//and finally, push the message onto the done queue to be picked up by the parent process
	doneQueue.Post(args);

	//this is quite neat as well
	//ifstream f1 ("C:\\me.txt",fstream::binary);
	//ofstream f2 ("C:\\me2.doc",fstream::trunc|fstream::binary);
	//f2<<f1.rdbuf();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataCacheIndexWorker::DataCacheIndexWorker(std::set<std::string>& fileIndex, std::set<std::string>& requestIndex) : _FileIndex(fileIndex), _RequestIndex(requestIndex) {

}

DataCacheIndexWorker::~DataCacheIndexWorker() {
}

void DataCacheIndexWorker::Process(geogl::async::MessageQueueEventArgs& args) {
	//this process the message that comes back after a file has loaded into the cache
	//the purpose is to update the internal file index and request list with the new information
	geogl::cache::DataCacheWorkerMsg* filemsg = dynamic_cast<geogl::cache::DataCacheWorkerMsg*>(args.umessage.get());
	std::cout<<"DataCacheIndexWorkerWorker "<<filemsg->src<<" and "<<filemsg->dst<<std::endl;
	//std::cout<<"UPDATE INDEX HERE!"<<std::endl;

	//std::cout<<"DataCacheIndexWorker CopiedLocalFile"<<std::endl;
	if (!args.Success) return; //TODO: what happens here if the request fails? It just stays on the loading list forever
	//update file maps with the new file we've just added
	//this happens in the main thread, so there are no synchronisation issues
	_FileIndex.insert(filemsg->src);
	_RequestIndex.erase(filemsg->src); //remove from waiting request list
	//std::cout<<"Copied local file complete "<<filemsg->src<<std::endl;
}


} // namespace cache
} // namespace geogl
