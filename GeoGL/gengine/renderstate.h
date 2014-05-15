#pragma once

#include "gengine.h"

namespace gengine {

	//forward declarations
	//class VertexArrayObject;

///////////////////////////////////////////////////////////////////////////////////////////////////
//Render states

/////////////////////////////
//Face Culling

	//void glCullFace​(GLenum mode​);
	enum FaceCullingTest {
		CullFrontFace = GL_FRONT​,
		CullBackFace = GL_BACK​,
		CullFrontAndBackFace = GL_FRONT_AND_BACK​
	};

	enum WindingOrder {
		CCW = GL_CCW,
		CW = GL_CW
	};

	struct FaceCulling {
		bool _Enable; //glEnable(GL_CULL_FACE);
		FaceCullingTest _FaceTest;
		WindingOrder _WindingOrder;
		bool operator==(FaceCulling& test) {
			if ((!_Enable)&&(!test._Enable)) return true; //if both disabled don't compare values
			return (_FaceTest==test._FaceTest)
				&& (_WindingOrder==test._WindingOrder);
		}
		//bool operator!=(FaceCulling& test) {
		//	return !(*this==test);
		//}
	};

/////////////////////////////////	

	struct ScissorTest {
		bool _Enable;
		int _Left;
		int _Bottom;
		unsigned int _Width; //sizei
		unsigned int _Height;
		bool operator==(ScissorTest& test) {
			if ((!_Enable)&&(!test._Enable)) return true; //if both disabled don't compare values
			return (_Left==test._Left)
				&& (_Bottom==test._Bottom)
				&& (_Width==test._Width)
				&& (_Height==test._Height);
		}
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
		bool operator==(DepthTest& test) {
			if ((!_Enabled)&&(!test._Enabled)) return true;
			return _Function==test._Function;
		};
	};

//end of render states
///////////////////////////////////////////////////////////////////////////////////////////////////


	class RenderState
	{
	public:
		RenderState(void);
		~RenderState(void);

		//PrimitiveRestart
		FaceCulling _FaceCulling;
		//RasterizationMode
		ScissorTest _ScissorTest;
		//StencilTest
		DepthTest _DepthTest;
		//DepthRange
		//Blending
		//ColorMask
		bool _DepthMask;
		//Fog?

		void SetState(const RenderState& OldState);
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
