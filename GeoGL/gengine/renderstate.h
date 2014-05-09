#pragma once

namespace gengine {

	//forward declarations
	class VertexArrayObject;

///////////////////////////////////////////////////////////////////////////////////////////////////
//Render states

	struct ScissorTest {
		bool _Enable;
		int _Left;
		int _Bottom;
		unsigned int _Width; //sizei
		unsigned int _Height;
	};

	enum DepthTestFunction {
		Never,
		Equal,
		LessThanOrEqual,
		Greater,
		NotEqual,
		GreaterThanOrEqual,
		Always
	};

	struct DepthTest {
		bool _Enabled; //=true;
		DepthTestFunction _Function; // = DepthTestFunction.Less;
	};

//end of render states
///////////////////////////////////////////////////////////////////////////////////////////////////


	class RenderState
	{
	public:
		RenderState(void);
		~RenderState(void);

		//PrimitiveRestart
		//FacetCulling
		//RasterizationMode
		ScissorTest _ScissorTest;
		//StencilTest
		DepthTest _DepthTest;
		//DepthRange
		//Blending
		//ColorMask
		bool _DepthMask;
		//Fog?
	};

	//this is what I was using for render state
	//enable z buffer, back face culling and set front faces to CCW
	//glEnable(GL_DEPTH_TEST);
	//glClearDepth(1.0);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);


	//also ClearState

} //namespace gengine
