/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */


#include "gengine.h"
#include "graphicscontext.h"
#include "renderstate.h"
#include "drawobject.h"
#include "scenedataobject.h"
#include "shader.h"
#include "vertexdata.h"
#include "indexbuffer.h"
#include "ogldevice.h"
#include "shaderuniformcollection.h"
#include "shaderuniform.h"
#include "Camera.h"

namespace gengine {

	using namespace events;

	GraphicsContext::GraphicsContext(GLFWwindow *pWindow)
	{
		window=pWindow;
	}


	GraphicsContext::~GraphicsContext(void)
	{
		glfwDestroyWindow(window);
	}

	/// <summary>
	/// Called before starting to render to this context. In other words it's the initialisation.
	/// </summary>
	void GraphicsContext::Clear(void)
	{
		//TODO: really need to pass some of this information into the function
		int width, height;
		glfwGetWindowSize(window,&width,&height);
		glViewport(0, 0, width, height); // Set the viewport size to fill the window
		glClearDepth(1.0); //this is really a global state
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers
	}

	/// <summary>
	/// Render (something?) to the graphics context (window)
	/// HOW???
	/// TODO: this is a very naive implementation without any optimisation. In other words, the shaders only need to be bound if they change and uniforms only get done once.
	/// </summary>
	void GraphicsContext::Render(const DrawObject& obj, const SceneDataObject& sceneobj /* DrawElements? DrawObjects? */)
	{
		//TODO: MUST have some way of setting the initial renderstate so we know what state we are starting in
		//also todo: shader sorting and renderstate sorting

		//sync the global render state
		//attach shaders
		//bind uniforms
		//bind buffers
		//render
		//if indexed or not? DrawArrays, DrawElements?
	
		if (!OGLDevice::hasProgrammableShaders) {
			RenderFallback(obj,sceneobj);
			return;
		}

		obj._ShaderProgram->bind(); // Bind our shader

		//get shader program id and use it to bind uniforms
		//unsigned int ShaderId = obj._ShaderProgram->id();
		//int projectionMatrixLocation = glGetUniformLocation(ShaderId, "projectionMatrix"); // Get the location of our projection matrix in the shader
		//int viewMatrixLocation = glGetUniformLocation(ShaderId, "viewMatrix"); // Get the location of our view matrix in the shader
		//int modelMatrixLocation = glGetUniformLocation(ShaderId, "modelMatrix"); // Get the location of our model matrix in the shader
	
		//hack view and camera
		//glm::mat4 projectionMatrix;
		//glm::mat4 viewMatrix;
		//glm::mat4 modelMatrix;
		//viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.f)); // Create our view matrix
		//modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));  // Create our model matrix
		////modelMatrix = glm::scale(glm::mat4(0.1f), glm::vec3(0.5f));  // Create our model matrix
		////modelMatrix = glm::rotate(modelMatrix,rotateAngle,glm::vec3(0.0f,1.0f,0.0f));
		//glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]); // Send our projection matrix to the shader
		//glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]); // Send our view matrix to the shader
		//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);


		//glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &camera->projectionMatrix[0][0]); // Send our projection matrix to the shader
		//glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]); // Send our view matrix to the shader
		//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader

		//bind automatic uniforms
		obj._ShaderProgram->_shaderUniforms->SetMatrix4fv("projectionMatrix",sceneobj._camera->projectionMatrix);
		obj._ShaderProgram->_shaderUniforms->SetMatrix4fv("viewMatrix",sceneobj._camera->viewMatrix);
		obj._ShaderProgram->_shaderUniforms->SetMatrix4fv("modelMatrix",obj._ModelMatrix);

		//bind vertex arrays
		obj._vertexData->bind(*obj._ShaderProgram->_shaderAttributes);
		//set device render state globals
		OGLDevice::SetRenderState(*obj._rs);
	
		//finally, render!!!!!!
		if (obj._vertexData->_ib!=NULL)
		{
			//if an index buffer exists then render using the index buffer and DrawElements
			glDrawElements(obj._PrimType,obj._vertexData->_NumElements,GL_UNSIGNED_INT/*GL_UNSIGNED_BYTE*/,(void*)0);
		}
		else {
			//render using arrays if you don't have an index buffer
			glDrawArrays(obj._PrimType,0,obj._vertexData->_NumElements);
		}

		obj._vertexData->unbind(*obj._ShaderProgram->_shaderAttributes); //unbind vertex, colour and index buffers where used

		obj._ShaderProgram->unbind(); // Unbind our shader
	}

	/// <summary>
	/// Fallback for when there are no shaders to use for rendering.
	/// </summary>
	void GraphicsContext::RenderFallback(const DrawObject& obj, const SceneDataObject& sceneobj)
	{
		//fallback if no shaders supported
		//TODO: this should check existing state and only change if needed
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&sceneobj._camera->projectionMatrix[0][0]);
		glMatrixMode(GL_MODELVIEW);
		//need to multiply view by model matrix like the shader does and then load that into the model view opengl state
		glm::mat4 ModelViewMatrix = sceneobj._camera->viewMatrix * obj._ModelMatrix;
		glLoadMatrixf(&ModelViewMatrix[0][0]);

		obj._vertexData->bind(*obj._ShaderProgram->_shaderAttributes); //DO YOU ACTUALLY NEED TO DO THIS?
		//set device render state globals
		OGLDevice::SetRenderState(*obj._rs);

		if (obj._vertexData->_ib!=NULL)
		{
			//render using an index buffer and vertex buffer
			glDrawElements(obj._PrimType,obj._vertexData->_NumElements,GL_UNSIGNED_INT,obj._vertexData->_ib->_mem_indexbuffer);
		}
		else {
			//render using only a primitive type and vertex buffer e.g. tristrip or trifan
			glDrawArrays(obj._PrimType,0,obj._vertexData->_NumElements);
		}
	}

	/// <summary>
	/// Swap frame buffers
	/// When I get around to the stereo 3D rendering, it will probably be a quad buffer
	/// </summary>
	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(window);
	}

	/// <summary>
	/// WindowSizeCallback
	/// Event processing system. Raised by EventListeners if you hook the event up.
	/// </summary>
	void GraphicsContext::WindowSizeCallback(GLFWwindow* window, int w, int h)
	{
		//TODO: I'm sure this is supposed to do something??????

		//reshapeWindow(w,h);
		//which does the following vvvvvvvvvvvvvvvvvvvvvv
		//windowWidth = w; // Set the window width
		//windowHeight = h; // Set the window height
	}

} //namespace gengine
