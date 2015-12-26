/*
 * messagequeue.cpp
 *
 *  Created on: 19 Oct 2014
 *      Author: richard
 */

#include "messagequeue.h"
#include "worker.h"
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>

namespace geogl {
namespace async {

/**
 * This is a multiple use queue that can either be run asynchronously on a separate thread, or called manually
 * using ProcessMessages.
 */

MessageQueue::MessageQueue() {
	// TODO Auto-generated constructor stub

}

MessageQueue::~MessageQueue() {
	//TODO: you could do this through a unique pointer
	//delete workers created through MessageReceived.push_back(new Worker())
	for (std::vector<Worker*>::iterator it = MessageReceived.begin(); it!=MessageReceived.end(); ++it) {
		delete (*it);
	}
}

/// <summary>
/// Start the message queue processing thread in the background
/// </summary>
void MessageQueue::StartBackgroundThread(void) {
	//I've implemented this as a pool, but only created one as I will probably experiment with creating a number of background threads.
	std::unique_ptr<std::thread> t(new std::thread(&MessageQueue::Run,this));
	_ThreadPool.push_back(std::move(t));
}

/// <summary>
/// Thread runner function
/// TODO: you could improve this by copying everything off the queue in one lock
/// </summary>
void MessageQueue::Run(void) {
	while (true) { //not stopping?
		while (_queue.size()>0) {
			//lock
			std::unique_lock<std::mutex> lck { _mutexQueue };
			MessageQueueEventArgs args = _queue.front();
			_queue.pop_front();
			//unlock
			lck.unlock();
			ProcessMessage(args);
		}
		//sleep
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

}

/// <summary>
/// Process all messages on the queue.
/// TODO: could do with merging this with the Run procedure - ALSO, write a get current queue function that returns thread safe contents in one go
/// </summary>
void MessageQueue::ProcessQueue(void) {
	while (_queue.size()>0) {
		//lock
		std::unique_lock<std::mutex> lck { _mutexQueue };
		MessageQueueEventArgs args = _queue.front();
		_queue.pop_front();
		//unlock
		lck.unlock();
		ProcessMessage(args);
	}
}

/// <summary>
/// Post a message for processing in the background thread
/// </summary>
void MessageQueue::Post(MessageQueueEventArgs args) {
	//lock
	std::unique_lock<std::mutex> lck { _mutexQueue };
	_queue.push_back(args);
	//unlock (implicitly as lck goes out of scope)
}

/// <summary>
/// Do the actual processing on a single message by calling all the event workers with the arguments
/// </summary>
void MessageQueue::ProcessMessage(MessageQueueEventArgs args) {
	for (std::vector<Worker*>::iterator it = MessageReceived.begin(); it!=MessageReceived.end(); ++it) {
		(*it)->Process(args);
		//if (args.callback!=nullptr) {
		//	//MessageQueueEventArgs* args2 = new MessageQueueEventArgs(args);
		//	//args.callback((MessageQueueEventArgs&)*args2);
		//	args.callback(args);
		//}
	}

}

void MessageQueue::Terminate() {
	//Post(Stop,nullptr);
}

void MessageQueue::Stop(void* object) {

}

} // namespace async
} // namespace geogl
