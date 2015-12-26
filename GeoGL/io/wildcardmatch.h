/*
 * WildcardMatch.h
 *
 *  Created on: 25 Jul 2015
 *      Author: richard
 *  Match a string using * and ? wildcard operators (*=multiple chars, ?=one single char).
 */

//TODO: tidy this up!

#include <iostream>
#include <string>


namespace geogl {
namespace io {

class WildcardMatch {
private:
	char WildCharMultiple; //probably '*'
	char WildCharSingle; //probably '?'
	std::string WildcardPattern;
protected:
	bool match(const char *pat, const char *str)
	{
		if (*pat=='\0') return !*str;
		else
			if (*pat==WildCharMultiple)
				return match(pat+1,str)||(*str && match(pat,str+1));
			else
				if (*pat==WildCharSingle)
					return *str && (match(pat+1,str+1)||match(pat+1,str));
		return (*str==*pat)&&match(pat+1,str+1);
	}
public:
	WildcardMatch(void)
	{
		WildCharMultiple='*';
		WildCharSingle='?';
	}
	WildcardMatch(const char* pattern)
	{
		WildCharMultiple='*';
		WildCharSingle='?';
		WildcardPattern = pattern;
	}

	WildcardMatch(const WildcardMatch& other)
	{
		WildCharMultiple=other.WildCharMultiple;
		WildCharSingle=other.WildCharSingle;
		//WildcardPattern = other.WildcardPattern;
		//_candidate=other._candidate;
	}

	WildcardMatch operator=(const WildcardMatch& other)
	{
		WildCharMultiple=other.WildCharMultiple;
		WildCharSingle=other.WildCharSingle;
		//WildcardPattern = other.WildcardPattern;
		//_candidate=other._candidate;
		return *this;
	}

	//getter/setters
	char GetMultipleSymbol(void)
	{
		return WildCharMultiple;
	}
	char GetSingleSymbol(void)
	{
		return WildCharSingle;
	}
	void SetMultipleSymbol(char ch)
	{
		WildCharMultiple=ch;
	}
	void SetSingleSymbol(char ch)
	{
		WildCharSingle=ch;
	}

	//main functionality

	//using a stored pattern
	bool match(std::string Text) {
		return match(WildcardPattern.c_str(),Text.c_str());
	}

	//using a pattern passed to the match procedure
	bool match(std::string Pattern, std::string Text) {
		return match(Pattern.c_str(),Text.c_str());
	}

	//how about a static version? inefficient if you need the class created

	/*static bool match(std::string Pattern, std::string Text) {
		//todo: smart pointers
		WildcardMatch* wm = new WildcardMatch();
		bool Result = wm->match(Pattern,Text);
		delete wm;
		return Result;
	}*/

};

} //namespace io
} //namespace geogl
