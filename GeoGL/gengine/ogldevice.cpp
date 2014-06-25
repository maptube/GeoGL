/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */

#include "ogldevice.h"
#include "shader.h"
#include "graphicscontext.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "texture2d.h"
#include "gltexturetypes.h"
#include "vertexarrayobject.h"
#include "image/jpeg.h"

#include <iostream>
#include <string>


namespace gengine {

////////////////////////////////////////////////////////////////////////////////////////////

//static member initialisation
	bool OGLDevice::hasProgrammableShaders=false;
	int OGLDevice::VersionMajor=0;
	int OGLDevice::VersionMinor=0;
	RenderState OGLDevice::CurrentRenderState = RenderState();

////////////////////////////////////////////////////////////////////////////////////////////

	//OGLDevice::OGLDevice(void)
	//{
	//	std::cout<<"OGLDevice constructor"<<std::endl;
	//	//if (!glfwInit())
	//	//	exit(EXIT_FAILURE);
	//}


	//OGLDevice::~OGLDevice(void)
	//{
	//}

	/// <summary>
	/// Perform necessary glfw initialisation
	/// </summary>
	void OGLDevice::Initialise()
	{
		if (!glfwInit())
			exit(EXIT_FAILURE);

		//need to query the device state so we know what's set initially
		QueryRenderState();
	}

	void OGLDevice::Destroy()
	{
		//TODO:
		//Perform any cleanup
		glfwTerminate();
	}

	/// <summary>
	/// Create an opengl window
	/// </summary>
	GraphicsContext* OGLDevice::XCreateWindow(int Width, int Height)
	{
		//GLFWOpenWindowHint
		GLFWwindow* window = glfwCreateWindow(Width, Height, "GeoGL", NULL, NULL);
		if (!window) {
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(window);

		//you need to have created a window and OpenGL context before you tyr and initialise GLEW
		GLenum error = glewInit(); // Enable GLEW
		if (error != GLEW_OK) // If GLEW fails
			return NULL; //and destroy window?

		//GLFW initialisation should have requested the latest version of OpenGL without all the GLEW messing about before

		int glVersion[2] = {-1, -1}; // Set some default values for the version
		glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using
		glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using
		VersionMajor=glVersion[0];
		VersionMinor=glVersion[1];
		std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	
		//todo: additional version checking here using GLEW?

		const GLubyte *glVersionString = glGetString(GL_VERSION); // Get the version of OpenGL we are using
		std::cout<<"OpenGL version string: "<<glVersionString<<std::endl;

		//OpenGL parameters
		int DepthBits;
		glGetIntegerv(GL_DEPTH_BITS,&DepthBits);
		std::cout<<"Z Buffer depth bits = "<<DepthBits<<" (should be 24)"<<std::endl;
		int MaxCombinedTexUnits, MaxTexUnits; //this is guaranteed>=16
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB,&MaxCombinedTexUnits); //this returns the total number of textures that OGL can handle
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&MaxTexUnits); //this returns the total number of textures that the fragment shader can access
		std::cout<<"Max combined texture units = "<<MaxCombinedTexUnits<<" Max texture units = "<<MaxTexUnits<<std::endl;
	
		//check for shader support - todo: make use of the version number
		const GLubyte *glShaderVersionString = glGetString(GL_SHADING_LANGUAGE_VERSION);
		hasProgrammableShaders=(glShaderVersionString!=NULL);
//HACK!	For testing fallback mode if you've got a real graphics card
//	hasProgrammableShaders = false;


		return new GraphicsContext(window);
	}

	/// <summary>
	/// Create a shader program from a fragment and vertex shader file.
	/// If device does not support hardware shaders then do nothing.
	/// </summary>
	/// <param name="VertexFilename"></param>
	/// <param name="FragmentFilename"></param>
	Shader* OGLDevice::CreateShaderProgram(std::string VertexFilename, std::string FragmentFilename)
	{
		//old code
		//Shader* ShaderProgram=NULL;
		//if (hasProgrammableShaders) {
		//	ShaderProgram = new Shader(VertexFilename.c_str(),FragmentFilename.c_str());
		//}
		//return ShaderProgram;

		//new code - shaders can be created even if hasProgrammableShaders==false as the gengine library handles the fallback.
		//We need the attributes and uniforms to be defined even if no shader program is used.
		return new Shader(VertexFilename.c_str(),FragmentFilename.c_str());
	}

	/// <summary>
	/// Create a vertex buffer for use with OpenGL
	/// </summary>
	/// <param name="AttributeName">Name of shader attribute variable that this buffer is bound to</param>
	/// <param name="Target">
	/// Type of buffer to create: GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER,GL_UNIFORM_BUFFER
	/// </param>
	/// <param name="Usage">Access type of buffer: StreamDraw = GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY</param>
	/// <param name="NumBytes">Size of buffer in bytes</param>
	VertexBuffer* OGLDevice::CreateVertexBuffer(const std::string& AttributeName, BufferTarget Target, BufferUsage Usage, unsigned int NumBytes)
	{
		return new VertexBuffer(AttributeName, Target, Usage, NumBytes);
	}

	/// <summary>
	/// Create an index buffer for used with OpenGL. Technically, some of the options you can pass to this are illegal for an index buffer
	/// as OpenGL has one create buffer function.
	/// TODO: filter the options so it really is an index buffer and you can't do anything stupid.
	/// </summary>
	/// <param name="Target">
	/// Type of buffer to create: GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER,GL_UNIFORM_BUFFER
	/// </param>
	/// <param name="Usage">Access type of buffer: StreamDraw = GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY</param>
	/// <param name="NumBytes">Size of buffer in bytes</param>
	IndexBuffer* OGLDevice::CreateIndexBuffer(BufferTarget Target, BufferUsage Usage, unsigned int NumBytes)
	{
		return new IndexBuffer(Target, Usage, NumBytes);
	}

	/// <summary>
	/// Create a new, blank, 2D texture
	/// </summary>
	/// <param name="Width">Width of texture image in pixels</param>
	/// <param name="Height">Height of texture image in pixels</param>
	/// <param name="PixelFormat">Bit format used by one pixel for colours and alpha</param>
	/// <returns>The blank texture</returns>
	Texture2D* OGLDevice::CreateTexture2D(const int Width, const int Height, const TexturePixelFormat PixelFormat)
	{
		return new Texture2D(Width,Height,TexTarget2D,PixelFormat);
	}

	/// <summary>Creates a 2D texture from an image in a file</summary>
	/// <param name="Filename">The file to load into the texture we've going to create</param>
	/// <returns>The new texture created from the image file specified</returns>
	Texture2D* OGLDevice::CreateTexture2DFromFile(const std::string& Filename)
	{
		//OK, how do you do this?
		//for now, assume a jpeg file and switch this to detect the format and call specialised texture format loaders e.g. jpeg, png, gif
		image::Jpeg jpeg = image::Jpeg(Filename);
		//then put the file into the texture
		Texture2D* tex = new Texture2D(jpeg._width,jpeg._height,TexTarget2D,TexPixelRGBA); //is this right for the format? and what about endian?
		//copy jpeg data into tex image
		tex->CopyFromMemory((unsigned char*)jpeg._raw_buffer);
		
		return tex;
	}

	/// <summary>
	/// Sets the OpenGL global rendering states.
	/// </summary>
	/// <param name="rs">The global state to set</param>
	void OGLDevice::SetRenderState(const RenderState& rs) {
		//PrimitiveRestart?

		//face culling
		if (CurrentRenderState._FaceCulling!=rs._FaceCulling) {
			if (rs._FaceCulling._Enabled) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
			glCullFace(rs._FaceCulling._FaceTest);
			glFrontFace(rs._FaceCulling._WindingOrder);
		}

		//RasterisationMode
		if (CurrentRenderState._RasterisationMode!=rs._RasterisationMode) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, rs._RasterisationMode._UnpackAlignment);
		}

		//scissor test
		if (CurrentRenderState._ScissorTest!=rs._ScissorTest) {
			if (rs._ScissorTest._Enabled) glEnable(GL_SCISSOR_TEST);
			else glDisable(GL_SCISSOR_TEST);
			glScissor(rs._ScissorTest._Left,rs._ScissorTest._Bottom,rs._ScissorTest._Width,rs._ScissorTest._Height);
		}

		//StencilTest
		
		//depth test
		if (CurrentRenderState._DepthTest!=rs._DepthTest) {
			if (rs._DepthTest._Enabled) glEnable(GL_DEPTH_TEST);
			else glDisable(GL_DEPTH_TEST);
			glDepthFunc(rs._DepthTest._Function);
		}
		
		//DepthRange
		
		//Blending
		if (CurrentRenderState._BlendingMode!=rs._BlendingMode) {
			if (rs._BlendingMode._Enabled) glEnable(GL_BLEND);
			else glDisable(GL_BLEND);
			glBlendFunc(rs._BlendingMode._SFactor,rs._BlendingMode._DFactor);
		}

		//ColorMask

		//depth mask
		if (CurrentRenderState._DepthMask!=rs._DepthMask)
			glDepthMask(rs._DepthMask);

		//set current state here
		CurrentRenderState = rs;
	}

	/// <summary>
	/// Get the current render state directly from the device - this is going to be a slow operation
	/// TODO: check that these are complete and correct! It's not immediately obvious how you get some of these values back.
	/// </summary>
	void OGLDevice::QueryRenderState(void) {
		RenderState rs;
		//GLboolean b; //it's an unsigned char, so incompatible with bool type
		//GLenum e;
		GLint i;
		GLint iv4[4];
		
		//Face Culling
		rs._FaceCulling._Enabled = (glIsEnabled(GL_CULL_FACE)!=0);
		glGetIntegerv(GL_CULL_FACE_MODE,&i);
		rs._FaceCulling._FaceTest=(FaceCullingTest)i;
		glGetIntegerv(GL_FRONT_FACE,&i);
		rs._FaceCulling._WindingOrder=(WindingOrder)i;

		//Rasterisation Mode
		glGetIntegerv(GL_UNPACK_ALIGNMENT,&i); //why does this return massively high values when it should only return 1,2,4,8?
		rs._RasterisationMode._UnpackAlignment = (unsigned int)i;

		//scissor test
		rs._ScissorTest._Enabled = (glIsEnabled(GL_SCISSOR_TEST)!=0);
		glGetIntegerv(GL_SCISSOR_BOX,&iv4[0]);
		rs._ScissorTest._Left=iv4[0];
		rs._ScissorTest._Bottom=iv4[1];
		rs._ScissorTest._Width=iv4[2];
		rs._ScissorTest._Height=iv4[3];

		//depth test
		rs._DepthTest._Enabled=(glIsEnabled(GL_DEPTH_TEST)!=0);
		glGetIntegerv(GL_DEPTH_FUNC,&i);
		rs._DepthTest._Function=(DepthTestFunction)i;

		//blending mode
		rs._BlendingMode._Enabled=(glIsEnabled(GL_BLEND)!=0);
		glGetIntegerv(GL_BLEND_SRC,&i);
		rs._BlendingMode._SFactor=(BlendFunction)i;
		glGetIntegerv(GL_BLEND_DST,&i);
		rs._BlendingMode._DFactor=(BlendFunction)i;

		//depth mask
		rs._DepthMask = (glIsEnabled(GL_DEPTH_WRITEMASK)!=0);

		CurrentRenderState = rs; //update internal copy with the current state of the hardware
	}


} //namepsace gengine
