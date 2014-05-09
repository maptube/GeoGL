#pragma once

#include <vector>
#include <string>
#include "gengine.h"

namespace gengine {

//forward references
struct ShaderAttribute;

//stores vertex attributes which are bound to shaders for drawing
class ShaderAttributeCollection
{
public:
	//Vertex Attributes have a name, location index for binding and a type
	std::vector<ShaderAttribute> _attributes;

	ShaderAttributeCollection(void);
	~ShaderAttributeCollection(void);
	void Clear(void);

	void AddVertexAttribute(int ShaderID, std::string Name, int Location, GLenum Type); //don't strictly need this as it's automatic from BuildFromShaderProgram
	const ShaderAttribute& Find(const std::string& Name);

	void BuildFromShaderProgram(int shader_id);

	void Debug_Enumerate();
};

}