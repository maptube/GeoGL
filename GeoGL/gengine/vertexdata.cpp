
#include "vertexdata.h"

#include <vector>

#include "shaderattributecollection.h"
#include "shaderattribute.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "ogldevice.h"

using namespace std;

namespace gengine {

	VertexData::VertexData(void)
	{
		_ib=NULL; //set index buffer to null in case we don't need to use it
	}


	VertexData::~VertexData(void)
	{
		//maybe check everything is unbound?
	}

	/// <summary>
	/// Bind vertex and index data stored in this object
	/// </summary>
	void VertexData::bind(ShaderAttributeCollection& attributes)
	{
		/*
		//old code
		//generic indexed triangles buffer render
		glBindVertexArray(vaoID);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,iboID);
		//glBindBuffer(GL_ARRAY_BUFFER,vcboID);
		glDrawElements(GL_TRIANGLES,NumElements,GL_UNSIGNED_INT/ *GL_UNSIGNED_BYTE* /,(void*)0);
		//glBindVertexArray(0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		*/

		//TODO: need to do this!
		//glBindAttribLocation(shader_id, v_inPosition, "in_Position"); // Bind a constant attribute location for positions of vertices
		//glBindAttribLocation(shader_id, v_inColor, "in_Color"); // Bind another constant attribute location, this time for color

		if (OGLDevice::hasProgrammableShaders)
		{
			//you don't actually need to use the vertexarrayobject as you can bind everything needed manually like this:
			//bind all the vertex buffers - probably vertices and colours
			for (vector<VertexBuffer*>::iterator vbIT=_vb.begin(); vbIT!=_vb.end(); ++vbIT) {
				//glBindVertexArray((*vbIT)->_vboID);
				//const std::string name = (*vbIT)->_AttributeName;
				//const ShaderAttribute& a = attributes.Find(name);
				////glBindAttribLocation(a._shaderID,a._Location,a._Name.c_str()); //do you need this - it's setup?
				////glEnableVertexAttribArray(a._Location); //also setup?
				//glBindBuffer(GL_ARRAY_BUFFER,a._Location);
				//glVertexAttribPointer(a._Location,3,GL_FLOAT,GL_FALSE,0,0);
				//glEnableVertexAttribArray(a._Location);

				//What we do here is bind the buffer (via its buffer ID, vboID) to an array buffer, set the vertex attribute pointer which hooks up
				//the currently bound buffer to the vertex attribute that we specify, then make sure we enable the vertex attribute array location
				//number in the shader.
				glBindBuffer((*vbIT)->_Target/*GL_ARRAY_BUFFER*/,(*vbIT)->_vboID); //this is the vertex buffer id, NOT the location in the shader
				const std::string name = (*vbIT)->_AttributeName; //lookup shader attribute data from the name stored in the vertex buffer object
				const ShaderAttribute& a = attributes.Find(name);
				glVertexAttribPointer(a._Location,3,GL_FLOAT,GL_FALSE,0,0); //now, first attrib IS the location in the shader
				glEnableVertexAttribArray(a._Location);
			}
			//bind the zero or one index buffer (you don't need glBindAttribLocation for the index buffer)
			if (_ib!=NULL) {
				//glBindVertexArray(_ib->_iboID);
				//don't glEnableVertexAttribArray for an index
				glBindBuffer(_ib->_Target,_ib->_iboID);
			}
		}
		else {
			//fallback - take the first vertex buffer and bind it as a vertex array buffer
			//TODO: need a better way of doing this - where is the buffer type?
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT,0,_vb[0]->_mem_vertexbuffer);
			//glEnableClientState(GL_COLOR_ARRAY);
			//glColorPointer(3,GL_FLOAT,0,mem_colourbuffer);
		}
	}

	/// <summary>
	/// Unbind buffers. As they can all bound to different targets, we need to go through each one and unbind the target separately.
	/// Only really need to do this once for each type (i.e. VertexArray and Colours are probably GL_ARRAY_BUFFER and Index is probably GL_ELEMENT_ARRAY_BUFFER).
	/// </summary>
	void VertexData::unbind(ShaderAttributeCollection& attributes)
	{
		if (OGLDevice::hasProgrammableShaders)
		{
			//unbind all types of buffer (target) used
			for (vector<VertexBuffer*>::iterator vbIT=_vb.begin(); vbIT!=_vb.end(); ++vbIT) {
				glBindBuffer((*vbIT)->_Target,0);
			}
			//bind the zero or one index buffer (you don't need glBindAttribLocation for the index buffer)
			if (_ib!=NULL) {
				glBindBuffer(_ib->_Target,0);
			}
		}
		else {
			//fallback - unbind if no shaders
		}
	}

} //namespace gengine
