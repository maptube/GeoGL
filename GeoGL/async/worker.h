/*
 * worker.h
 *
 *  Created on: 19 Oct 2014
 *      Author: richard
 */

#pragma once

#include "messagequeueeventargs.h"

#include <string>

namespace geogl {
namespace async {

//abstract base class for async workers that respond to messages
class Worker {
public:
	Worker();
	virtual ~Worker();

	virtual void Process(geogl::async::MessageQueueEventArgs& args)=0; //abstract, this does the work
};

} // namespace async
} // namespace geogl


