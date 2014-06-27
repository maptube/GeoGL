#pragma once

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

#include <string>

#include "renderstate.h"

#include "glbuffertypes.h"
#include "gltexturetypes.h"

namespace gengine {

	//forward declarations
	class Shader;
	class GraphicsContext;
	class VertexArrayObject;
	class VertexBuffer;
	class IndexBuffer;
	class Texture2D;
	//enum BufferTarget;
	//enum BufferUsage;

	/// <summary>
	/// All functions which are part of the GPU Hardware
	/// </summary>
	class OGLDevice
	{
	public:
		//OGLDevice(void);
		//~OGLDevice(void);

		static bool hasProgrammableShaders; //set to false for OpenGL1 fallback
		static int VersionMajor; //OpenGL version numbers
		static int VersionMinor;

		static void Initialise(void); //initialises glfw
		static void Destroy(void);

		static GraphicsContext* XCreateWindow(int Width,int Height);
		static GraphicsContext* CreateStereoWindow();
		static Shader* CreateShaderProgram(std::string VertexFilename, std::string FragmentFilename);
		static VertexBuffer* CreateVertexBuffer(const std::string& AttributeName, BufferTarget Target, BufferUsage Usage, unsigned int NumBytes);
		static IndexBuffer* CreateIndexBuffer(BufferTarget Target, BufferUsage Usage, unsigned int NumBytes);
		static Texture2D* CreateTexture2D(const int Width, const int Height, const TexturePixelFormat PixelFormat);
		static Texture2D* CreateTexture2DFromFile(const std::string& Filename);

		static void SetRenderState(const RenderState& rs);
		static void QueryRenderState(void);
	private:
		static RenderState CurrentRenderState; //current state of device globals
	};

} //namespace gengine
