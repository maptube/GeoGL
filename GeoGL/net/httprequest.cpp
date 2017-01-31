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
#include <iostream>
#include <unordered_map>

#include "text/TextUtilities.h"

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
/// Parse the block of characters which comprise the headers and populate the header string map.
/// Headers are in the form name : value \r\n
/// </summary>
void HttpRequest::ParseHeaders(std::vector<char> strHeaders, HttpResponse& response) {

	response.Headers = {};

	std::string line;
	for (auto it : strHeaders)
	{
		char ch = it;
		if (ch=='\r') {
			std::string name,value;
			std::size_t pos = line.find_first_of(':');
			if (pos!=std::string::npos) {
				name = line.substr(0,pos);
				value = line.substr(pos+1,line.length()-pos-1);
			}
			else {
				name = line;
				value = "";
			}
			name = geogl::text::TextUtilities::trim(name);
			value = geogl::text::TextUtilities::trim(value);
			if (name.length()>0) {
				response.Headers.insert({name,value});
				//std::cout<<"Http Header: "<<name<<" ===== "<<value<<"       Original: "<<line<<std::endl;
			}
			line="";
		}
		else {
			if (ch>=32) line=line+ch;
		}
	}
	//They're always terminated with an end of line character, so the final header is always added i.e. line.length()==0 at this point

	//for debugging
	//for (auto it: response.Headers) {
	//	std::cout<<"Saved Header: "<<it.first<<" : "<<it.second<<std::endl;
	//}

}

/// <summary>
/// Make an http request to the given url.
/// </summary>
HttpResponse HttpRequest::Download(std::string url) {
	std::vector<char> strHeaders;
	//std::vector<char> data;
	HttpResponse response;

	curl_global_init(CURL_GLOBAL_ALL);
	CURL* handle = curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL, url.c_str());
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA, &response.ResponseData);
	curl_easy_setopt(handle,CURLOPT_USERAGENT,"libcurl-agent/1.0");
	curl_easy_setopt(handle, CURLOPT_HEADERDATA, &strHeaders);
	CURLcode result = curl_easy_perform(handle);
	curl_easy_getinfo(handle,CURLINFO_RESPONSE_CODE,&response.ResponseCode);
	curl_easy_cleanup(handle);
	curl_global_cleanup();

	if (result!=CURLE_OK) {
		std::stringstream err;
		err<<"Error downloading "<<url<<": "<<curl_easy_strerror(result);
		throw std::runtime_error(err.str());
	}

	HttpRequest::ParseHeaders(strHeaders,response);

	//return std::move(data);
	return std::move(response);

}


} /* namespace net */
} /* namespace geogl */
