/*
 * workermsg.h
 *
 *  Created on: 31 Oct 2014
 *      Author: richard
 */

#pragma once


namespace geogl {
namespace async {

//empty class for messages used by workers to inherit from
class WorkerMsg {
public:
	WorkerMsg();
	//copy constructor needed by make_shared - not used as this must be wrong below: return other?
	//WorkerMsg(WorkerMsg*& other) {};
	virtual ~WorkerMsg();
};

} // namespace async
} // namespace geogl

