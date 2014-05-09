
#include "shaderuniformcollection.h"
#include "shaderuniform.h"
//#include "gengine.h"

#include <string>

using namespace std;

namespace gengine {

	ShaderUniformCollection::ShaderUniformCollection(void)
	{
	}


	ShaderUniformCollection::~ShaderUniformCollection(void)
	{
	}

	/// <summary>
	/// Clear all the information stored.
	/// </summary>
	void ShaderUniformCollection::Clear(void)
	{
		_uniforms.clear();
	}

	/// <summary>
	/// Query the shaders for all the uniform names and locations
	/// </summary>
	/// <param name="shader_id"></param>
	void ShaderUniformCollection::BuildFromShaderProgram(int shader_id)
	{
		int total = -1;
		glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &total );
		for(int i=0; i<total; ++i)  {
			int name_len=-1, num=-1;
			GLenum type = GL_ZERO;
			char name[256];
			glGetActiveUniform(shader_id, GLuint(i), sizeof(name)-1, &name_len, &num, &type, name);
			name[name_len] = 0;
			GLuint location = glGetUniformLocation(shader_id, name);
			ShaderUniform u(shader_id,std::string(name),type,location);
			_uniforms.push_back(u);
		}

	}

	/// <summary>
	/// DEBUG: write out all the shader uniforms in the collection to the console
	/// </summary>
	void ShaderUniformCollection::Debug_Enumerate()
	{
		cout<<"Shader uniforms"<<endl;
		cout<<"Index: Shader ID, Location, Name, Type Enum"<<endl;
		int i=0;
		for (vector<ShaderUniform>::iterator it = _uniforms.begin(); it!=_uniforms.end(); ++it) {
			ShaderUniform u = *it;
			cout<<i<<": "<<u._shaderID<<" "<<u._Location<<" "<<u._Name<<" "<<u._Type<<endl;
			++i;
		}
	}

	/// <summary>
	/// Send a matrix to the shader using its name
	/// </summary>
	void ShaderUniformCollection::SetMatrix4fv(const std::string& Name, glm::mat4& m)
	{
		//TODO: not exactly elegant, but there aren't going to be many uniforms and a hash is probably overkill?
		bool Found=false;
		for (vector<ShaderUniform>::iterator it = _uniforms.begin(); it!=_uniforms.end(); ++it) {
			ShaderUniform u = *it;
			if (u._Name==Name) {
				Found=true;
				glUniformMatrix4fv(u._Location, 1, GL_FALSE, &m[0][0]); // Send our matrix to the shader
				break;
			}
		}
		if (!Found) cerr<<"Error: shader uniform \""<<Name<<"\" not found"<<endl;
		//should I be checking for invalid names?
	}

} //namespace gengine
