
#include "agenttime.h"
#include <ctime>
#include <sstream>

#include <math.h>

using namespace std;

/// class to provide continuous time functions on top of the time_t structure which is only accurate to the nearest second
AgentTime::AgentTime(void)
{
}


AgentTime::~AgentTime(void)
{
}

/// <summary>
/// convert from string in format YYYYMMDD_HHMMSS
/// </summary>
AgentTime AgentTime::FromString(const std::string& Text)
{
	std::tm t = {0};
	//stringstream ss(DTCode);
	//stringstream ss("201406");
	//ss>>std::get_time(&t,"%Y%m"); //TODO: check leading zeroes
	//strptime(DTCode, "%Y%m%d_%H%M%S", &t); //UNDEFINED
	//OK, none of the above functions for parsing time work, so do it the hard way
	//TODO: check I've got all of this right
	t.tm_year = std::atoi(Text.substr(0,4).c_str())-1900; //2014 should come out as 114
	t.tm_mon = std::atoi(Text.substr(4,2).c_str())-1; //january is zero
	t.tm_mday = std::atoi(Text.substr(6,2).c_str());
	//underscore
	t.tm_hour = std::atoi(Text.substr(9,2).c_str());
	t.tm_min = std::atoi(Text.substr(11,2).c_str());
	t.tm_sec = std::atoi(Text.substr(13,2).c_str());
	
	AgentTime at;
	at._DT = std::mktime(&t);
	at._fraction=0;
	return at;
}

/// <summary>
/// Convert an agent time into a string
/// </summary>
std::string AgentTime::ToString(const AgentTime& ATime)
{
	std::tm* t;
	t = std::gmtime(&ATime._DT);

	std::stringstream Result("");
	int Year = t->tm_year+1900;
	Result<<(t->tm_year+1900)<<(t->tm_mon+1)<<(t->tm_mday)<<" "<<(t->tm_hour)<<":"<<(t->tm_min)<<":"<<(t->tm_sec);

	return Result.str();
}

void AgentTime::Add(const float Seconds)
{
	int Quotient = (int)floor(Seconds+_fraction); //number of seconds requested plus accumulated fraction so far, which we use to get the whole number of seconds
	float fraction = Seconds-Quotient;

	_DT+=Quotient; //add on the whole seconds
	_fraction+=fraction; //and the fraction accumulated
}

/// <summary>
/// Overload to write out a string from a time_t directly
/// </summary>
std::string AgentTime::ToString(const time_t t)
{
	AgentTime at;
	at._DT=t;
	at._fraction=0;
	return AgentTime::ToString(at);
}

/// <summary>
/// Assignment operator
/// <summary>
AgentTime& AgentTime::operator=(const AgentTime& other)
{
	_DT=other._DT;
	_fraction=other._fraction;
	return *this;
}

AgentTime& AgentTime::operator+=(const float Seconds)
{
	Add(Seconds);
	return *this;
}

/// <summary>
/// Add a fractional number of seconds to the time structure.
/// </summary>
void AgentTime::operator+(const float Seconds)
{
	//int Quotient = (int)floor(Seconds+_fraction); //number of seconds requested plus accumulated fraction so far, which we use to get the whole number of seconds
	//float fraction = Seconds-Quotient;
//
	//_DT+=Quotient; //add on the whole seconds
	//_fraction = fraction; //and the fraction accumulated
	Add(Seconds);
}

bool AgentTime::operator>=(const AgentTime& other)
{
	if (_DT>other._DT) return true;
	if (_DT<other._DT) return false;
	return (_fraction>=other._fraction);
}

bool AgentTime::operator>=(const time_t& other)
{
	//comparing Agent time >= time_t (which is an Agent time with fraction=0)
	return (_DT>=other); //as if they're equal, the _fraction is always>=0
}
