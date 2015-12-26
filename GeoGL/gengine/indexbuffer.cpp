/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */


#include "indexbuffer.h"
#include "ogldevice.h"
#include "gengine.h"

namespace gengine {

	IndexBuffer::IndexBuffer(void)
	{
	}

	IndexBuffer::IndexBuffer(BufferTarget Target, BufferUsage Usage, unsigned int NumBytes)
	{
		//TODO: probably need a vertex type i.e. xyz, normal, colour, texture coords?

		_mem_indexbuffer=NULL;

		//set members that define this buffer's type and use
		_SizeBytes=NumBytes;
		_Target=Target;
		_Usage=Usage;

		if (OGLDevice::hasProgrammableShaders) {
			glGenBuffers(1, &_iboID); // Generate our Index Buffer Object
		}

		//be careful with the sizes here, as the buffers need real memory sizes, not numbers of points or indices
		//glBindBuffer(_Target, _iboID); // Bind our Index Buffer Object
		//glBufferData(GL_ARRAY_BUFFER, NumVertices*3 * sizeof(GLfloat), &buf_vertices[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
		//glVertexAttribPointer((GLuint)Shader::v_inPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	}


	IndexBuffer::~IndexBuffer(void)
	{
		//TODO: check this really works, gen buffer with an existing id

		if (OGLDevice::hasProgrammableShaders) {
			glGenBuffers(1, &_iboID); // Delete our Vertex Buffer Object
		}
		else {
			delete [] _mem_indexbuffer;
		}
	}

	/// <summary>
	/// This does a glBufferData call on this buffer
	/// </summary>
	void IndexBuffer::CopyFromMemory(unsigned int* mem_indexdata)
	{
		if (OGLDevice::hasProgrammableShaders) {
			glBindBuffer(_Target, _iboID); // Bind our Index Buffer Object
			//glBufferData(GL_ARRAY_BUFFER, NumVertices*3 * sizeof(GLfloat), &buf_vertices[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
			//so target=GL_ARRAY_BUFFER and Usage=GL_STATIC_DRAW
			glBufferData(_Target, _SizeBytes, mem_indexdata, _Usage); // Set the size and data of our VBO and set it to STATIC_DRAW
		}
		else {
			//fallback if no hardware buffer support
			unsigned int SizeInts = _SizeBytes>>2; //_SizeBytes/sizeof(int) ???
			_mem_indexbuffer = new GLuint[SizeInts];
			//TODO: this isn't the fastest way of doing this - use a block copy!
			for (unsigned int i=0; i<SizeInts; i++)
				_mem_indexbuffer[i]=mem_indexdata[i];
		}
	}

//Am I ever going to need to do this? Get the data back from the buffer.
//void IndexBuffer::CopyToMemory(float* mem_vertexdata)

} //namespace gengine
