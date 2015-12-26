/*
 * messagequeue.h
 *
 *  Created on: 19 Oct 2014
 *      Author: richard
 */

//TODO: this isn't complete and not all the functions work e.g. running, stopping and starting
//also, need to get rid of the callback as it doesn't work

#pragma once

#include "messagequeueeventargs.h"

#include <vector>
#include <deque>
#include <thread>
#include <memory>
#include <mutex>

//forward declarations
namespace geogl {
	namespace async {
		class Worker;
	}
}

namespace geogl {
namespace async {

enum MessageQueueState {
	running, stopping, stopped
};

//TODO: need locking of the message queue
class MessageQueue {
private:
	std::mutex _mutexQueue; //mutex for the _queue object
	//this is the queue being managed - new messages go on the back, with processing front to back in order
	std::deque<MessageQueueEventArgs> _queue;
	std::vector<std::unique_ptr<std::thread>> _ThreadPool; //pool of background threads doing the actual work - probably==1
	MessageQueueState _state;
	void Run(void);
	void Stop(void* object);
public:
	MessageQueue();
	virtual ~MessageQueue();

	std::vector<Worker*> MessageReceived; //this is the event list, workers get called to process message events

	void StartBackgroundThread(void);
	void Post(MessageQueueEventArgs args);
	void ProcessMessage(MessageQueueEventArgs args);
	//void TerminateAndWait();
	void Terminate();

	void ProcessQueue();
};

} // namespace async
} // namespace geogl
