
#include "shaderattributecollection.h"
#include "shaderattribute.h"
#include "gengine.h"

#include <iostream>
#include <string>

using namespace std;

namespace gengine {

	ShaderAttributeCollection::ShaderAttributeCollection(void)
	{
	}


	ShaderAttributeCollection::~ShaderAttributeCollection(void)
	{
	}

	void ShaderAttributeCollection::Clear(void)
	{
		_attributes.clear();
	}

	/// <summary>
	/// Add a vertex attribute manually
	/// </summary>
	void ShaderAttributeCollection::AddVertexAttribute(int ShaderID, std::string Name, int Location, GLenum Type)
	{
		ShaderAttribute a(ShaderID,Name,Location,Type);
		_attributes.push_back(a);
	}

	/// <summary>
	/// Find the attribute bound to the shader by the specified name e.g. "in_Position" or "in_colour" from the definitions at the top of the shader.
	/// If the buffers are created before the shader is compiled, then you don't have the location. The buffer is created with its name set to something like "in_Position"
	/// which links the buffer with this shader attribute.
	/// TODO: this isn't a particularly good solution as there should be some sort of hard linking between the attribute and the buffer.
	/// </summary>
	/// <param name="">The name to find</param>
	const ShaderAttribute& ShaderAttributeCollection::Find(const std::string& Name)
	{
		for (vector<ShaderAttribute>::iterator it = _attributes.begin(); it!=_attributes.end(); ++it) {
			ShaderAttribute a = *it;
			if (a._Name==Name) return *it;
		}
		cerr<<"Error: shader attribute \""<<Name<<"\" not found."<<endl;
		return *_attributes.end();
	}

	/// <summary>
	/// Add all the attributes by querying the shader program.
	/// </summary>
	/// <param name="shader_id">The ID of the shader program to query the attributes from.</param>
	void ShaderAttributeCollection::BuildFromShaderProgram(int shader_id)
	{
		int total = -1;
		glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTES, &total );
		for(int i=0; i<total; ++i)  {
			int name_len=-1, num=-1;
			GLenum type = GL_ZERO;
			char name[256];
			glGetActiveAttrib(shader_id, GLuint(i), sizeof(name)-1, &name_len, &num, &type, name);
			name[name_len] = 0;
			GLuint location = glGetAttribLocation(shader_id, name);
			ShaderAttribute a(shader_id,std::string(name),type,location);
			_attributes.push_back(a);
		}

	}

	void ShaderAttributeCollection::Debug_Enumerate()
	{
		cout<<"Shader attributes"<<endl;
		cout<<"Index: Shader ID, Location, Name, Type Enum"<<endl;
		int i=0;
		for (vector<ShaderAttribute>::iterator it = _attributes.begin(); it!=_attributes.end(); ++it) {
			ShaderAttribute a = *it;
			cout<<i<<": "<<a._shaderID<<" "<<a._Location<<" "<<a._Name<<" "<<a._Type<<endl;
			++i;
		}
	}

} //namespace gengine
