
#include "DebugUtils.h"
//#include <Windows.h>
//TODO: you could write a cross platform version using windows calls to log to the vc2010
//console, while #define LINUX makes it write to cout
//This probably isn't necessary as both can write to cout, so this debug module is generally
//unnecessary, but keep it in the project for anything complicated we need in the future.

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
	//OutputDebugStringA(str.c_str());
}

void DebugUtils::WriteFloat(float f)
{
	//std::ostringstream ss;
	//ss<<f;
	//OutputDebugStringA(ss.str().c_str());
}

void DebugUtils::WriteInt(int i)
{
	//std::ostringstream ss;
	//ss<<i;
	//OutputDebugStringA(ss.str().c_str());
}

void DebugUtils::WriteLine()
{
	//OutputDebugStringA("\n");
}
