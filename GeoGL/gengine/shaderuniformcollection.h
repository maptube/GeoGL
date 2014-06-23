#pragma once

#include "gengine.h"

#include <string>
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
		int FindLocationIndex(const std::string& Name);
		void SetMatrix4fv(const std::string& Name, const glm::mat4& m);
		void SetMatrix4dv(const std::string& Name, const glm::dmat4& m);
		//SetInt, SetFloat etc? we're generally going to be using mat4s though.
		void SetUniform1i(const std::string& Name, const int i);
		void SetUniform1f(const std::string& Name, const float f);
		void SetUniform3fv(const std::string& Name, const glm::vec3& v);
		void SetUniform4fv(const std::string& Name, const glm::vec4& v);
		void SetUniform3dv(const std::string& Name, const glm::dvec3& v);
		void SetUniform4dv(const std::string& Name, const glm::dvec4& v);

		void Debug_Enumerate();
	};

} //namespace gengine
