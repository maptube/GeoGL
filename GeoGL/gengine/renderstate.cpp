
#include "renderstate.h"

namespace gengine {

	RenderState::RenderState(void)
	{
		//PrimitiveRestart
		_FaceCulling._Enabled=false;
		//RasterizationMode
		_ScissorTest._Enabled=false;
		//StencilTest
		_DepthTest._Enabled=false;
		//DepthRange
		//Blending
		//ColorMask
		_DepthMask=false;
		//Fog?
	}

	//copy constructor
	RenderState::RenderState(const RenderState& rs)
	{
		_FaceCulling=rs._FaceCulling;
		_ScissorTest=rs._ScissorTest;
		_DepthTest=rs._DepthTest;
		_DepthMask=rs._DepthMask;
	}

	RenderState::~RenderState(void)
	{
	}

	/// <summary>
	/// Assignment operator
	/// </summary>
	RenderState& RenderState::operator=(const RenderState& rs)
	{
		_FaceCulling=rs._FaceCulling;
		_ScissorTest=rs._ScissorTest;
		_DepthTest=rs._DepthTest;
		_DepthMask=rs._DepthMask;
		return *this;
	}

	/// <summary>
	/// Set new state in OpenGL based on the current state and what needs to be changed.
	/// This is an optimisation as it's a lot quicker to compare states on only change those
	/// settings that need to be changed.
	/// TODO: this doesn't actually affect the CurrentState - maybe it should? I leave that to the calling process.
	/// TODO: also, it should really be the device that manages these states
	/// </summary>
	/// <param name="CurrentState">Pass in the current state, so we only have to change the values we need to - much quicker</param>
	//void RenderState::SetState(const RenderState& CurrentState)
	//{
	//	//PrimitiveRestart?

	//	//face culling
	//	if (CurrentState._FaceCulling!=_FaceCulling) {
	//		if (_FaceCulling._Enabled) glEnable(GL_CULL_FACE);
	//		else glDisable(GL_CULL_FACE);
	//		glCullFace(_FaceCulling._FaceTest);
	//		glFrontFace(_FaceCulling._WindingOrder);
	//	}

	//	//RasterizationMode?

	//	//scissor test
	//	if (CurrentState._ScissorTest!=_ScissorTest) {
	//		if (_ScissorTest._Enabled) glEnable(GL_SCISSOR_TEST);
	//		else glDisable(GL_SCISSOR_TEST);
	//		glScissor(_ScissorTest._Left,_ScissorTest._Bottom,_ScissorTest._Width,_ScissorTest._Height);
	//	}

	//	//StencilTest
	//	
	//	//depth test
	//	if (CurrentState._DepthTest!=_DepthTest) {
	//		if (_DepthTest._Enabled) glEnable(GL_DEPTH_TEST);
	//		else glDisable(GL_DEPTH_TEST);
	//		glDepthFunc(_DepthTest._Function);
	//	}
	//	
	//	//DepthRange
	//	//Blending
	//	//ColorMask
	//	//bool _DepthMask;

	//	//set current state here
	//}

} //namespace gengine
