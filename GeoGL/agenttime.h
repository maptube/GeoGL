#pragma once

#include <ctime>
#include <string>

class AgentTime
{
public:
	time_t _DT; //year, month, day, hour, min, second
	float _fraction; //fractions of a second

	AgentTime(void);
	~AgentTime(void);

	static AgentTime FromString(const std::string& Text);
	static std::string ToString(const AgentTime& ATime);
	static std::string ToString(const time_t t);

	void Add(const float Seconds);

	AgentTime& operator=(const AgentTime& other);
	void operator+(const float Seconds);
	//void operator+(const double Seconds); ?
	AgentTime& operator+=(const float Seconds);
	bool operator>=(const AgentTime& other);
	bool operator>=(const time_t& other);
};

