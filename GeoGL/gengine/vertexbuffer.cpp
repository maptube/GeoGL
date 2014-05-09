
#include "vertexbuffer.h"
#include "ogldevice.h"
#include "gengine.h"

namespace gengine {

	VertexBuffer::VertexBuffer(void)
	{
	}

	/// <summary>
	///
	/// </summary>
	/// <param name="AttributeName">The name of the shader attribute that this buffer gets bound to</param>
	/// <param name="Target"></param>
	/// <param name="Usage"></param>
	/// <param name="NumBytes"></param>
	VertexBuffer::VertexBuffer(const std::string& AttributeName, BufferTarget Target, BufferUsage Usage, unsigned int NumBytes)
	{
		//TODO: probably need a vertex type i.e. xyz, normal, colour, texture coords?

		_mem_vertexbuffer=NULL;

		//set members that define this buffer's type and use
		_AttributeName=AttributeName;
		_SizeBytes=NumBytes;
		_Target=Target;
		_Usage=Usage;

		if (OGLDevice::hasProgrammableShaders) {
			glGenBuffers(1, &_vboID); // Generate our Vertex Buffer Object
			//glBindBuffer(_Target, _vboID); 
			std::cout<<"Created vertex buffer with id: "<<_vboID<<std::endl;
		}

		//be careful with the sizes here, as the buffers need real memory sizes, not numbers of points or indices
		//glBindBuffer(_Target, _vboID); // Bind our Vertex Buffer Object
		//glBufferData(GL_ARRAY_BUFFER, NumVertices*3 * sizeof(GLfloat), &buf_vertices[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
		//glVertexAttribPointer((GLuint)Shader::v_inPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	}


	VertexBuffer::~VertexBuffer(void)
	{
		//TODO: check this really works, gen buffer with an existing id
		if (OGLDevice::hasProgrammableShaders) {
			glGenBuffers(1, &_vboID); // Delete our Vertex Buffer Object
		}
		else {
			delete [] _mem_vertexbuffer;
		}
	}

	/// <summary>
	/// This does a glBufferData call on this buffer
	/// </summary>
	void VertexBuffer::CopyFromMemory(float* mem_vertexdata)
	{
		if (OGLDevice::hasProgrammableShaders) {
			glBindBuffer(_Target, _vboID); // Bind our Vertex Buffer Object
			//glBufferData(GL_ARRAY_BUFFER, NumVertices*3 * sizeof(GLfloat), &buf_vertices[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
			//so target=GL_ARRAY_BUFFER and Usage=GL_STATIC_DRAW
			glBufferData(_Target, _SizeBytes, mem_vertexdata, _Usage); // Set the size and data of our VBO and set it to STATIC_DRAW
			glBindBuffer(_Target,0); //unbind our buffer
		}
		else {
			//fallback if no hardware buffer support
			unsigned int SizeFloats = _SizeBytes>>2; //_SizeBytes/sizeof(float) ???
			_mem_vertexbuffer = new float[SizeFloats];
			//TODO: this isn't the fastest way of doing this - use a block copy!
			for (int i=0; i<SizeFloats; i++)
				_mem_vertexbuffer[i]=mem_vertexdata[i];
		}
	}

	//Am I ever going to need to do this? Get the data back from the buffer.
	//void VertexBuffer::CopyToMemory(float* mem_vertexdata)

} //namespace gengine
