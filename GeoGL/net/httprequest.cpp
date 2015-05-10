/*
 * httprequest.cpp
 *
 *  Created on: 4 May 2015
 *      Author: richard
 *  Functions relating to http request utilities, for example make an http request using curl.
 */

#include "httprequest.h"

//for the download functions
#include <curl/curl.h>
#include <sstream>
#include <stdexcept>

namespace geogl {
namespace net {

HttpRequest::HttpRequest() {
	// TODO Auto-generated constructor stub

}

HttpRequest::~HttpRequest() {
	// TODO Auto-generated destructor stub
}

//callback function for HttpRequest::Download
size_t callback(void* contents, size_t size, size_t nmemb, void* user) {
	auto chunk = reinterpret_cast<char*>(contents);
	auto buffer = reinterpret_cast<std::vector<char>*>(user);

	size_t priorSize = buffer->size();
	size_t sizeIncrease = size*nmemb;

	buffer->resize(priorSize+sizeIncrease);
	std::copy(chunk,chunk+sizeIncrease,buffer->data()+priorSize);
	return sizeIncrease;
}

/// <summary>
/// Make an http request to the given url.
/// </summary>
std::vector<char> HttpRequest::Download(std::string url, long* responsecode) {
	std::vector<char> data;

	curl_global_init(CURL_GLOBAL_ALL);
	CURL* handle = curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL, url.c_str());
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA, &data);
	curl_easy_setopt(handle,CURLOPT_USERAGENT,"libcurl-agent/1.0");
	CURLcode result = curl_easy_perform(handle);
	if (responsecode!=nullptr)
		curl_easy_getinfo(handle,CURLINFO_RESPONSE_CODE,responsecode);
	curl_easy_cleanup(handle);
	curl_global_cleanup();

	if (result!=CURLE_OK) {
		std::stringstream err;
		err<<"Error downloading "<<url<<": "<<curl_easy_strerror(result);
		throw std::runtime_error(err.str());
	}

	return std::move(data);

}


} /* namespace net */
} /* namespace geogl */
