#pragma once

//#include "main.h"
#include "glbuffertypes.h"

namespace gengine {

//A vertex buffer is like an index buffer, except that it stores floats
//If there is support for hardware vertex buffers on the GPU then they are used, otherwise
//the buffer is created in CPU memory. No code alterations should be needed to make this work.
class VertexBuffer
{
public:
	VertexBuffer(void);
	VertexBuffer(const std::string& AttributeName, BufferTarget Target, BufferUsage Usage, unsigned int NumBytes);
	~VertexBuffer(void);

	void CopyFromMemory(float* mem_vertexdata);
	//void CopyToMemory();
	std::string _AttributeName; //this is the name of the attribute that this buffer gets bound to in the shader
	GLuint _SizeBytes;
	GLuint _vboID; //our Vertex Buffer Object
	GLfloat* _mem_vertexbuffer; //In memory buffer used if no support for hardware vertex buffers
	BufferTarget _Target;
	BufferUsage _Usage;
};

//vertex array contains VertexBufferAttributes and IndexBuffer
//mesh contains VertexAttributeCollection, IndicesBase, PrimitiveType and WindingOrder
//They do context.Draw(PrimitiveType.Triangles, _drawState, _sceneState)
//sceneState contains camera?

}