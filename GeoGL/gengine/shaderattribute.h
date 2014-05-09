#pragma once
#include <string>
#include "gengine.h"

namespace gengine {

	struct ShaderAttribute
	{
	public:
		//Vertex Attributes have a name, location index for binding and a type
		int _shaderID;
		std::string _Name; //this is the name of the attribute that this is bound to in the shader
		int _Location;
		GLenum _Type;

		ShaderAttribute(void);
		ShaderAttribute(int ShaderID, std::string Name, GLenum _Type, int Location);
		~ShaderAttribute(void);

	};

} //namespace gengine
