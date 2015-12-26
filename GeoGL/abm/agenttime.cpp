
#include "agenttime.h"
#include <ctime>
#include <sstream>
#include <iomanip>

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
/// Convert from string in format DD/MM/YYYY HH:MM:SS
/// </summary>
AgentTime AgentTime::FromString2(const std::string& Text)
{
	std::tm t = {0};

	stringstream ss(Text);
	std::string elem;
	//not exactly elegant, but it works
	std::getline(ss,elem,'/');
	t.tm_mday = std::stoi(elem);
	std::getline(ss,elem,'/');
	t.tm_mon = std::stoi(elem)-1; //January is zero
	std::getline(ss,elem,' ');
	t.tm_year = std::stoi(elem)-1900; //2014 should come out as 114
	std::getline(ss,elem,':');
	t.tm_hour = std::stoi(elem);
	std::getline(ss,elem,':');
	t.tm_min = std::stoi(elem);
	std::getline(ss,elem);
	t.tm_sec = std::stoi(elem);

	AgentTime at;
	at._DT = std::mktime(&t);
	at._fraction=0;
	return at;

}

/// <summary>
/// Convert an agent time into a string of the form yyyymmdd hh:mm:ss with leading zeroes
/// </summary>
std::string AgentTime::ToString(const AgentTime& ATime)
{
	std::tm* t;
	t = std::gmtime(&ATime._DT);

	std::stringstream Result("");
	int Year = t->tm_year+1900;
	Result<<(t->tm_year+1900)<<setfill('0')<<setw(2)<<(t->tm_mon+1)<<setfill('0')<<setw(2)<<(t->tm_mday)<<" "<<setfill('0')<<setw(2)<<(t->tm_hour)<<":"<<setfill('0')<<setw(2)<<(t->tm_min)<<":"<<setfill('0')<<setw(2)<<(t->tm_sec);

	return Result.str();
}

/// <summary>
/// Convert an agent time into a file path of the form yyyy/mm/dd
/// NOTE: there are no leading zeroes on mm or dd
/// </summary>
std::string AgentTime::ToFilePath(const AgentTime& ATime)
{
	struct tm *tmp = localtime(&ATime._DT); //is local right here? should it be gmtime as above?
	std::stringstream ss;
	//dir part (note dirs don't have a leading zero)
	ss<<(tmp->tm_year+1900)<<"/"<<(tmp->tm_mon+1)<<"/"<<(tmp->tm_mday);
	std::string DirPart;
	ss>>DirPart;
	return DirPart;
}

std::string AgentTime::ToStringYYYYMMDD_hhmmss(const AgentTime& ATime)
{
	struct tm *tmp = localtime(&ATime._DT); //is local right here? should it be gmtime as above?
	std::stringstream ss;
	//this is a rubbish way of formatting text
	ss<<(tmp->tm_year+1900)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_mon+1)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_mday)
			<<"_"<<std::setfill('0')<<std::setw(2)<<(tmp->tm_hour)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_min)<<std::setfill('0')<<std::setw(2)<<(tmp->tm_sec);
	std::string TimeCode;
	ss>>TimeCode;
	return TimeCode;
}

/// <summary>
/// Get the hour, minute and second from the agent time.
/// </summary>
void AgentTime::GetTimeOfDay(int& Hour, int& Minute, int& Second)
{
	struct tm *tmp = localtime(&_DT); //is local right here? should it be gmtime as above?
	Hour = tmp->tm_hour;
	Minute = tmp->tm_min;
	Second = tmp->tm_sec;
}

void AgentTime::Add(const float Seconds)
{
	int Quotient = (int)floor(Seconds+_fraction); //number of seconds requested plus accumulated fraction so far, which we use to get the whole number of seconds
	float fraction = Seconds-Quotient;

	_DT+=Quotient; //add on the whole seconds
	_fraction+=fraction; //and the fraction accumulated
}

/// <summary>
/// Return T1-T2 in seconds
/// </summary>
float AgentTime::DifferenceSeconds(const AgentTime& T1, const AgentTime& T2)
{
	float secs=T1._DT-T2._DT;
	float fraction = T1._fraction - T2._fraction; //need to check this is right

	return secs+fraction;
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
