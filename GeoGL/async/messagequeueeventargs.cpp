/*
 * messagequeueeventargs.cpp
 *
 *  Created on: 19 Oct 2014
 *      Author: richard
 */

#include "messagequeueeventargs.h"

#include <memory>

//forward declarations
namespace geogl {
	namespace async {
		class WorkerMsg;
	}
}

namespace geogl {
namespace async {

MessageQueueEventArgs::MessageQueueEventArgs(WorkerMsg* message) : umessage(std::shared_ptr<geogl::async::WorkerMsg>(message)), Success(false) {
	//DO NOT use std::make_shared<geogl::async::WorkerMsg>(message) on umessage above, as it doesn't work. It tries to make the pointer from a reference to the existing one.
	// TODO Auto-generated constructor stub
}

MessageQueueEventArgs::~MessageQueueEventArgs() {
	// TODO Auto-generated destructor stub
	//delete message;
}

} // namespace async
} // namespace geogl
