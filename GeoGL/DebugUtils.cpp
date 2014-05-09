
#include "DebugUtils.h"
#include <Windows.h>


DebugUtils::DebugUtils(void)
{
}


DebugUtils::~DebugUtils(void)
{
}

void DebugUtils::operator<<(std::string str)
{
	//do something.....
}

void DebugUtils::WriteString(std::string str)
{
	OutputDebugStringA(str.c_str());
}

void DebugUtils::WriteFloat(float f)
{
	std::ostringstream ss;
	ss<<f;
	OutputDebugStringA(ss.str().c_str());
}

void DebugUtils::WriteInt(int i)
{
	std::ostringstream ss;
	ss<<i;
	OutputDebugStringA(ss.str().c_str());
}

void DebugUtils::WriteLine()
{
	OutputDebugStringA("\n");
}
