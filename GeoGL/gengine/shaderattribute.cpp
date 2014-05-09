
#include "shaderattribute.h"

namespace gengine {

ShaderAttribute::ShaderAttribute(void)
{
}

ShaderAttribute::ShaderAttribute(int ShaderID, std::string Name, GLenum Type, int Location)
{
	_shaderID = ShaderID;
	_Name = Name;
	_Location = Location;
	_Type = Type;
}


ShaderAttribute::~ShaderAttribute(void)
{
}

}