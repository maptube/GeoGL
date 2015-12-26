/*
 * file.h
 *
 *  Created on: 27 Jul 2015
 *      Author: richard
 */

#ifndef FILE_H_
#define FILE_H_

namespace geogl {
namespace io {

class File {
public:
	File();
	virtual ~File();
	//TODO: add static file.exists here
};

} /* namespace io */
} /* namespace geogl */

#endif /* FILE_H_ */
