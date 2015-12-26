/*
 * httprequest.h
 *
 *  Created on: 4 May 2015
 *      Author: richard
 */

#pragma once

#include <string>
#include <vector>

namespace geogl {
namespace net {

class HttpRequest {
public:
	HttpRequest();
	virtual ~HttpRequest();

	static std::vector<char> Download(std::string url, long* responsecode);
};

} // namespace net
} // namespace geogl

