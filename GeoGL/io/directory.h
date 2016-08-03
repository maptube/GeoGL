/*
 * directory.h
 *
 *  Created on: 27 Jul 2015
 *      Author: richard
 */

#pragma once

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <string>

namespace geogl {
namespace io {

class Directory {
public:
	Directory();
	virtual ~Directory();

	static std::string MatchFilePattern(std::string BaseDir, std::string pattern);
};

} // namespace io
} // namespace geogl
