#pragma once

#include <ctime>
#include <string>

//TODO: this should really be in the ABM namespace

class AgentTime
{
public:
	time_t _DT; //year, month, day, hour, min, second
	float _fraction; //fractions of a second

	AgentTime(void);
	~AgentTime(void);

	static AgentTime Now();
	static AgentTime FromString(const std::string& Text);
	static AgentTime FromString2(const std::string& Text);
	static std::string ToString(const AgentTime& ATime);
	static std::string ToString(const time_t t);
	static std::string ToStringYYYYMMDD_hhmmss(const AgentTime& ATime);
	static std::string ToFilePath(const AgentTime& ATime);

	void GetTimeOfDay(int& Hour, int& Minute, int& Second);
	void GetYearMonthDay(int& Year, int& Month, int& Day);
	int GetDayOfWeek();

	void Add(const float Seconds);
	static float DifferenceSeconds(const AgentTime& T1, const AgentTime& T2);

	AgentTime& operator=(const AgentTime& other);
	void operator+(const float Seconds);
	//void operator+(const double Seconds); ?
	AgentTime& operator+=(const float Seconds);
	bool operator>=(const AgentTime& other);
	bool operator>=(const time_t& other);
};

