#pragma once
#include "gengine.h"

namespace gengine {

	//forward references
	//class VertexArrayObject;
	class ShaderUniform;
	class ShaderUniformCollection;
	struct ShaderAttribute;
	class ShaderAttributeCollection;

	/**
		Shader is a class designed to allow us to load and use a GLSL shader program in
		our OpenGL application. It allows us to provide filenames for the vertex and 
		fragment shaders, and then creates the shader. It also lets us bind and 
		unbind the GLSL shader program as required.
	*/
	class Shader {
	public:
		//buffer number constants for the vertex and colour varying attribute bindings
		//TODO: both the two following consts need to come out!
		//static const int v_inPosition=0;
		//static const int v_inColor=1;

		ShaderUniformCollection* _shaderUniforms;
		ShaderAttributeCollection* _shaderAttributes;

		Shader(); // Default constructor
		Shader(const char *vsFile, const char *fsFile); // Constructor for creating a shader from two shader filenames
		~Shader(); // Destructor
    
		void init(const char *vsFile, const char *fsFile); // Initialize our shader file if we have to
    
		void bind(); // Bind our GLSL shader program
		void unbind(); // Unbind our GLSL shader program
    
		unsigned int id(); // Get the identifier for our program
    
	private:
		unsigned int shader_id; // The shader program identifier
		unsigned int shader_vp; // The vertex shader identifier
		unsigned int shader_fp; // The fragment shader identifier
    
		bool inited; // Whether or not we have initialized the shader
	};

} //namespace gengine
