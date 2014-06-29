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

#include "renderstate.h"
#include "events/EventListener.h"

#include <iostream>
#include <string>

//forward references
struct GLFWwindow;

namespace gengine {

	//forward references
	//class RenderState;
	class DrawObject;
	class SceneDataObject;
	//class VertexBuffer;
	class VertexData;
	class Shader;
	class TextureUnits;

	/// <summary>
	/// A graphics context is something we can render to and includes a window
	/// </summary>
	class GraphicsContext : public events::EventListener
	{
	private:
		TextureUnits* _TexUnits; //private copy of textures bound to device units
	public:
		GLFWwindow* window;
		FT_Library ft; //freetype text library
		//buffer for texture
		Shader* _FontShader;
		VertexData* _FontVertexData;

		GraphicsContext(GLFWwindow* pWindow);
		~GraphicsContext(void);

		void Clear(); //clear before rendering
		void ClearZ();
		void Render(const DrawObject& obj, const SceneDataObject& sceneobj);
		void RenderFallback(const DrawObject& obj, const SceneDataObject& sceneobj);
		void SwapBuffers(void);
		float ReadZBuffer(float X, float Y);

		//font handling
		FT_Face LoadFont(const std::string& FontName, const unsigned int FontSize);
		void RenderText(const FT_Face FontFace, const glm::vec3 vColour, const char *text, float x, float y, float sx, float sy);

		virtual void WindowSizeCallback(GLFWwindow* window, int w, int h);

	private:
	};

} //namespace gengine
