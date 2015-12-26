/*
 * messagequeueeventargs.h
 *
 *  Created on: 19 Oct 2014
 *      Author: richard
 */

#pragma once
#include "workermsg.h"

#include <memory>
#include <functional>

//forward declarations
//namespace geogl {
//	namespace async {
//		class WorkerMsg;
//	}
//}

//callbacks: use lambdas
//http://www.nullptr.me/2011/10/12/c11-lambda-having-fun-with-brackets/#.UJmXu8XA9Z8

namespace geogl {
namespace async {


class MessageQueueEventArgs {
public:
	MessageQueueEventArgs(WorkerMsg* message);
	virtual ~MessageQueueEventArgs();
	std::shared_ptr<geogl::async::WorkerMsg> umessage;
	//void (*callback)(MessageQueueEventArgs&); //callback function taking argument MessageQueueEventArgs&
	//std::function<void(MessageQueueEventArgs&)> callback;
	bool Success;
};

} // namespace async
} // namespace geogl
