
#include "renderstate.h"

namespace gengine {

	RenderState::RenderState(void)
	{
	}


	RenderState::~RenderState(void)
	{
	}

	/// <summary>
	/// Set new state in OpenGL based on the current state and what needs to be changed.
	/// This is an optimisation as it's a lot quicker to compare states on only change those
	/// settings that need to be changed.
	/// </summary>
	void RenderState::SetState(const RenderState& OldState)
	{
		//PrimitiveRestart
		if (OldState._FaceCulling!=_FaceCulling) {
		}
		//RasterizationMode
		if (OldState._ScissorTest!=_ScissorTest) {
		}
		ScissorTest _ScissorTest;
		//StencilTest
		DepthTest _DepthTest;
		//DepthRange
		//Blending
		//ColorMask
		bool _DepthMask;
	}

} //namespace gengine
