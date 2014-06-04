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
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "ogldevice.h"
#include "shaderuniformcollection.h"
#include "shaderuniform.h"
#include "Camera.h"
#include "shader.h"
#include "glbuffertypes.h"

namespace gengine {

	using namespace events;

	GraphicsContext::GraphicsContext(GLFWwindow *pWindow)
	{
		window=pWindow;

		//initialise FreeType text rendering library
		if(FT_Init_FreeType(&ft)) {
			std::cerr<<"Could not initialise freetype library"<<std::endl;
		}
		
		//shader for the font rendering
		_FontShader = OGLDevice::CreateShaderProgram("fontshader.vert","fontshader.frag");
		//vertex data and buffer for box required to put the texture for font rendering in
		//should I really be creating a full draw object?
		_FontVertexData = new VertexData();
		VertexBuffer* vbo = new VertexBuffer("coord",ArrayBuffer,DynamicDraw,16*sizeof(GLfloat));
		_FontVertexData->_vb.push_back(vbo);
		_FontVertexData->_NumElements=4; //does this even do anything?
		_FontVertexData->_ComponentsPerVertex=4; //This is needed - set to X,Y,S,T per vertex for 2D with texture coords
	}


	GraphicsContext::~GraphicsContext(void)
	{
		delete _FontShader;
		delete _FontVertexData;
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
	/// Clear only the Z buffer. Required for multi-frustum rendering.
	/// </summary>
	void GraphicsContext::ClearZ(void)
	{
		int width, height;
		glfwGetWindowSize(window,&width,&height);
		glViewport(0, 0, width, height); // Set the viewport size to fill the window
		glClearDepth(1.0); //this is really a global state
		glClear(GL_DEPTH_BUFFER_BIT); // Clear required buffers
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
		obj._ShaderProgram->_shaderUniforms->SetMatrix4dv("projectionMatrix",sceneobj._camera->projectionMatrix); //was 4fv originally
		obj._ShaderProgram->_shaderUniforms->SetMatrix4dv("viewMatrix",sceneobj._camera->viewMatrix); //was 4fv originally
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
		//glLoadMatrixf(&sceneobj._camera->projectionMatrix[0][0]);
		glLoadMatrixd(&sceneobj._camera->projectionMatrix[0][0]); //interesting to know what this actually does as GPU support for doubles is very new
		glMatrixMode(GL_MODELVIEW);
		//need to multiply view by model matrix like the shader does and then load that into the model view opengl state
		//convert float ModelMatrix to a double
		glm::dmat4 dModelMatrix = glm::dmat4(obj._ModelMatrix);
		//glm::mat4 ModelViewMatrix = sceneobj._camera->viewMatrix * obj._ModelMatrix;
		glm::dmat4 ModelViewMatrix = sceneobj._camera->viewMatrix * dModelMatrix;
		//glLoadMatrixf(&ModelViewMatrix[0][0]);
		glLoadMatrixd(&ModelViewMatrix[0][0]);

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

	/// <summary>
	/// Read the value of the Z buffer at window coordinates (X,Y). Z is returned as a device coordinate between 0..1
	/// <summary>
	/// <param name="X">X coordinate to read</param>
	/// <param name="Y">Y coordinate to read</param>
	/// <returns>The value of the Z buffer at (X,Y) in normalised device coordinates [0..1]</returns>
	float GraphicsContext::ReadZBuffer(float X, float Y)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		int winX = (int)X;
		int winY = (int)((float)viewport[3]-Y);
		float winZ;
		glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		return winZ;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Font Handling
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	///Load a font which we can render to the OpenGL context.
	/// </summary>
	/// <param name="FontName">The name of the font file e.g. FreeSans.ttf (needs to be either relative to program or fully qualified)</param>
	/// <param name="FontSize">The font size in points e.g. 48</param>
	/// <returns></returns>
	FT_Face GraphicsContext::LoadFont(const std::string& FontName, const unsigned int FontSize)
	{
		FT_Face face;
		if(FT_New_Face(ft, FontName.c_str(), 0, &face)) {
			std::cerr<<"Could not open font "<<FontName<<std::endl;
		}
		else {
			FT_Set_Pixel_Sizes(face, 0, FontSize);
		}
		return face;
	}
	
	/// <summary>
	/// Render text to window using freetype, see:
	/// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
	/// </summary>
	/// <param name="FontFace"></param>
	/// <param name="text"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="sx"></param>
	/// <param name="sy"></param>
	void GraphicsContext::RenderText(const FT_Face FontFace, const char *text, float x, float y, float sx, float sy)
	{
		//TODO: we need a vbo for the coords, a uniform_tex ("tex" in glsl), attribute_coord ("coord" in glsl)
		const char *p;
		FT_GlyphSlot g = FontFace->glyph;

		RenderState rs; //set up a render state that has _FaceCulling._Enabled=false
		rs._DepthTest._Enabled=false;
		rs._FaceCulling._Enabled=false;
		OGLDevice::SetRenderState(rs);

		//TODO: this needs to be a colour passed in the function
		_FontShader->_shaderUniforms->SetUniform4fv("color",glm::vec4(1,0,0,1));

		/* Create a texture that will be used to hold one "glyph" */
		GLuint tex;
		
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		_FontShader->_shaderUniforms->SetUniform1i("tex",0);
		
		/* We require 1 byte alignment when uploading texture data */
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		/* Clamping to edges is important to prevent artifacts when scaling */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		/* Linear filtering usually looks best for text */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		for(p = text; *p; p++) {
			if(FT_Load_Char(FontFace, *p, FT_LOAD_RENDER))
				continue;
			
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_ALPHA,
				g->bitmap.width,
				g->bitmap.rows,
				0,
				GL_ALPHA,
				GL_UNSIGNED_BYTE,
				g->bitmap.buffer
			);
			
			float x2 = x + g->bitmap_left * sx;
			float y2 = -y - g->bitmap_top * sy;
			float w = g->bitmap.width * sx;
			float h = g->bitmap.rows * sy;
			
			GLfloat box[4][4] = {
				{x2,   -y2,   0, 0},
				{x2+w, -y2,   1, 0},
				{x2,   -y2-h, 0, 1},
				{x2+w, -y2-h, 1, 1},
			};
			
			_FontVertexData->_vb[0]->CopyFromMemory(&box[0][0]);
			_FontVertexData->bind(*_FontShader->_shaderAttributes); //todo: can you move the bind outside the loop - it's all the same buffer?
			//finally, we actually get to do some drawing...
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			_FontVertexData->unbind(*_FontShader->_shaderAttributes);
			
			x += (g->advance.x >> 6) * sx;
			y += (g->advance.y >> 6) * sy;
		}

		glDeleteTextures(1, &tex);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//End of Font Handling
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////




} //namespace gengine
