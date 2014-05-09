#pragma once

#include "gengine.h"

#include <vector>

namespace gengine {

	//forward declarations
	struct ShaderUniform;

	//collection of uniforms which are bound to shaders for drawing
	class ShaderUniformCollection
	{
	public:
		std::vector<ShaderUniform> _uniforms;

		ShaderUniformCollection(void);
		~ShaderUniformCollection(void);
		void Clear(void);
		//Add?
		void BuildFromShaderProgram(int shader_id);
		void SetMatrix4fv(const std::string& Name, glm::mat4& m);
		//SetInt, SetFloat etc? we're generally going to be using mat4s though.

		void Debug_Enumerate();
	};

} //namespace gengine
