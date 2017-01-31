/*
 * httprequest.h
 *
 *  Created on: 4 May 2015
 *      Author: richard
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace geogl {
namespace net {

struct HttpResponse {
public:
	long ResponseCode;
	std::unordered_map<std::string,std::string> Headers;
	std::vector<char> ResponseData;
};

class HttpRequest {
public:

	HttpRequest();
	virtual ~HttpRequest();

	//static std::vector<char> Download(std::string url, long* responsecode);
	static HttpResponse Download(std::string url);
protected:
	static void ParseHeaders(std::vector<char> strHeaders, HttpResponse& Response);
};

} // namespace net
} // namespace geogl

