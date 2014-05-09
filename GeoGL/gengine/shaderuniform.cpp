
#include "shaderuniform.h"
#include "gengine.h"

namespace gengine {

	ShaderUniform::ShaderUniform(void)
	{
	}

	ShaderUniform::ShaderUniform(int ShaderID, std::string Name, GLenum Type, unsigned int Location)
	{
		_shaderID=ShaderID;
		_Name=Name;
		_Type=Type;
		_Location=Location;
	}


	ShaderUniform::~ShaderUniform(void)
	{
	}

} //namespace gengine
