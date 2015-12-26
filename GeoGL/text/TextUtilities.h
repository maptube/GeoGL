/*
 * TextUtilities.h
 *
 *  Created on: 14 Aug 2015
 *      Author: richard
 */

#pragma once

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

namespace geogl {
namespace text {

/// <summary>
/// Header class designed for text manipulation utilities e.g. whitespace removal
/// </summary>
class TextUtilities {
public:
	//TextUtilities();
	//virtual ~TextUtilities();

	/// <summary>
	/// trim whitespace from start
	/// </summary>
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(),s.end(), std::not1(std::ptr_fun<int,int>(std::isspace))));
		return s;
	}

	/// <summary>
	/// trim whitespace from end
	/// </summary>
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(),s.rend(), std::not1(std::ptr_fun<int,int>(std::isspace))).base(),s.end());
		return s;
	}

	/// <summary>
	/// trim whitespace from both ends
	/// </summary>
	static inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}
};

} // namespace text
} // namespace geogl

