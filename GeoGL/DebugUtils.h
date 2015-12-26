#pragma once
#include <iostream>
#include <string>
#include <sstream>


//Debug is a reserved class name (why?)
class DebugUtils
{
public:
	DebugUtils(void);
	~DebugUtils(void);
	void operator << (std::string str);
	static void WriteString(std::string str);
	static void WriteFloat(float f);
	static void WriteInt(int i);
	static void WriteLine();
};

