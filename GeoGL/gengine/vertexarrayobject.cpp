
#include "vertexarrayobject.h"

namespace gengine {

	VertexArrayObject::VertexArrayObject(void)
	{
		glGenVertexArrays(1, &_vaoID); // Create our Vertex Array Object - doesn't work if GL version 1.5 fallback
	}


	VertexArrayObject::~VertexArrayObject(void)
	{
		glDeleteVertexArrays(1, &_vaoID); //free the VAO object
	}

	/// <summary>
	/// Need to bind vertex arrays to the VertexArrayObject.
	/// Add vertex array attribute. The index is the index used in the shader when they are created and the name is the uniform that it is bound to.
	/// </summary>
	/// <param name="Index"></param>
	/// <param name="Name"></param>
	void VertexArrayObject::AddVertexAttribute(unsigned int Index, std::string Name)
	{
		_Attributes.insert(std::pair<int,std::string>(Index,Name));
	
		glBindVertexArray(_vaoID); // Bind our Vertex Array Object so we can use it
		glVertexAttribPointer((GLuint)Index, 3, GL_FLOAT, GL_FALSE, 0, NULL); //THIS IS A HACK! need to pass this in
		glEnableVertexAttribArray((GLuint)Index);

		//glVertexAttribPointer((GLuint)Shader::v_inPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
		//glVertexAttribPointer((GLuint)Shader::v_inColor, 3, GL_FLOAT, GL_FALSE, 0, NULL); // (note enable later)

		//glEnableVertexAttribArray(Shader::v_inPosition); // Enable our vertex buffer in our Vertex Array Object
		//glEnableVertexAttribArray(Shader::v_inColor); // Enable our colour buffer in our Vertex Array Object

		glBindVertexArray(0); // Disable our Vertex Array Object
	}

} //namespace gengine
