#pragma once

#include <string>
#include "gengine.h"

namespace gengine {

	//Single shader uniform data
	struct ShaderUniform
	{
	public:
		int _shaderID;
		std::string _Name; //name of shader uniform
		GLenum _Type; //type of uniform
		unsigned int _Location; //location index which you need for binding

		ShaderUniform(void);
		ShaderUniform(int ShaderID, std::string Name, GLenum Type, unsigned int Location);
		~ShaderUniform(void);
	};

} //namespace gengine
