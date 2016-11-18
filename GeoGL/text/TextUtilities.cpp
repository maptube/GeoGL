/*
 * TextUtilities.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: richard
 */

#include "TextUtilities.h"

#include <string>
#include <vector>

namespace geogl {
namespace text {

//TextUtilities::TextUtilities() {
//	// TODO Auto-generated constructor stub
//
//}

//TextUtilities::~TextUtilities() {
//	// TODO Auto-generated destructor stub
//}

	std::vector<std::string> TextUtilities::split(const std::string& str, const std::string& delim)
	{
		std::vector<std::string> tokens;
		size_t prev = 0, pos = 0;
		do
		{
			pos = str.find(delim, prev);
			if (pos == std::string::npos) pos = str.length();
			std::string token = str.substr(prev, pos - prev);
			if (!token.empty()) tokens.push_back(token);
			prev = pos + delim.length();
		} while (pos < str.length() && prev < str.length());
		return tokens;
	}

} // namespace text
} // namespace geogl
