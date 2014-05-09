#pragma once

namespace gengine {

//forward declarations
class RenderState;
class Shader;
//class VertexArrayObject;
class VertexData;
enum PrimitiveType;

/// <summary>
/// DrawObject wraps up everything needed to draw an object to a GraphicsContext. In other words, vertex buffers, shaders, attributes and render state
/// </summary>
class DrawObject {
public:
	RenderState* _rs;
	Shader* _ShaderProgram;
	PrimitiveType _PrimType;
	//VertexArrayObject* _vao;
	VertexData* _vertexData;
	//VertexArray va;
	//IndexArray ia;
	glm::mat4 _ModelMatrix;
};

}