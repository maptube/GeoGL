#pragma once

#include <string>
#include "gengine.h"

namespace gengine {

	//Single shader uniform data
	/*struct ShaderUniform
	{
	public:
		int _shaderID;
		std::string _Name; //name of shader uniform
		GLenum _Type; //type of uniform
		unsigned int _Location; //location index which you need for binding

		ShaderUniform(void);
		ShaderUniform(int ShaderID, std::string Name, GLenum Type, unsigned int Location);
		~ShaderUniform(void);
	};*/

	//base class
	class ShaderUniform
	{
	public:
		int _shaderID;
		std::string _Name; //name of shader uniform
		GLenum _Type; //type of uniform
		unsigned int _Location; //location index which you need for binding

		ShaderUniform(void);
		ShaderUniform(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : _shaderID(ShaderID), _Name(Name), _Type(Type), _Location(Location) {};
		~ShaderUniform(void);
		virtual void Bind(void) {};
		//make = operator virtual for each type and then override later
		virtual void operator=(const int& i) {};
		virtual void operator=(const float& f) {};
		virtual void operator=(const glm::vec3& v) {};
		virtual void operator=(const glm::vec4& v) {};
		virtual void operator=(const glm::dvec3& d) {};
		virtual void operator=(const glm::dvec4& d) {};
		virtual void operator=(const glm::mat4& i) {};
		//smapler2D?
	};

	//specialised type child classes
	class ShaderUniformInt : public ShaderUniform
	{
	public:
		ShaderUniformInt(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		int _Value;
		void operator=(const int& v) { _Value=v; };
		virtual void Bind(void) { glUniform1i(_Location, _Value); };
	};

	class ShaderUniformFloat : public ShaderUniform
	{
	public:
		ShaderUniformFloat(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		float _Value;
		void operator=(const float& v) { _Value=v; };
		virtual void Bind(void) { glUniform1f(_Location,_Value); };
	};

	class ShaderUniformVec3 : public ShaderUniform
	{
	public:
		ShaderUniformVec3(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		glm::vec3 _Value;
		void operator=(const glm::vec3& v) { _Value=v; };
		virtual void Bind(void) { glUniform3fv(_Location, 1, &_Value[0]); };
	};

	class ShaderUniformVec4 : public ShaderUniform
	{
	public:
		ShaderUniformVec4(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		glm::vec4 _Value;
		void operator=(const glm::vec4& v) { _Value=v; };
		virtual void Bind(void) { glUniform4fv(_Location, 1, &_Value[0]); };
	};

	class ShaderUniformDVec3 : public ShaderUniform
	{
	public:
		ShaderUniformDVec3(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		glm::dvec3 _Value;
		void operator=(const glm::dvec3& v) { _Value=v; };
		virtual void Bind(void) {
			glm::vec3 fv(_Value); //cast it down to a float vector
			glUniform3fv(_Location, 1, &fv[0]);
		};
	};

	class ShaderUniformDVec4 : public ShaderUniform
	{
	public:
		ShaderUniformDVec4(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		glm::dvec4 _Value;
		void operator=(const glm::dvec4& v) { _Value=v; };
		virtual void Bind(void) {
			glm::vec4 fv(_Value); //cast it down to a float vector
			glUniform4fv(_Location, 1, &fv[0]);
		};
	};

	class ShaderUniformMat4 : public ShaderUniform
	{
	public:
		ShaderUniformMat4(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		glm::mat4 _Value;
		void operator=(const glm::mat4& v) { _Value=v; };
		virtual void Bind(void) {
			glUniformMatrix4fv(_Location, 1, GL_FALSE, &_Value[0][0]); // Send our matrix to the shader
		};
	};

	//how do we handle this exactly? it's linked to the texture units - using an int here
	class ShaderUniformSampler2D : public ShaderUniform
	{
	public:
		ShaderUniformSampler2D(int ShaderID, std::string Name, GLenum Type, unsigned int Location) : ShaderUniform(ShaderID, Name, Type, Location) { };
		int _Value; //GL_SAMPLER_2D
		void operator=(const int& v) { _Value=v; };
		virtual void Bind(void) { glUniform1i(_Location, _Value); };
	};

	//TODO: do we need matrix types here? Mostly they're automatics, so we don't need to store the values.

} //namespace gengine
