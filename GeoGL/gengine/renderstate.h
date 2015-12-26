#pragma once

#include "gengine.h"

namespace gengine {

	//forward declarations
	//class VertexArrayObject;

///////////////////////////////////////////////////////////////////////////////////////////////////
//Render states

/////////////////////////////
//Face Culling

	//void glCullFace(GLenum mode);
	//GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
	enum FaceCullingTest {
		CullFrontFace = GL_FRONT, CullBackFace = GL_BACK, CullFrontAndBackFace = GL_FRONT_AND_BACK
	};

	enum WindingOrder {
		CCW = GL_CCW,
		CW = GL_CW
	};

	struct FaceCulling {
		bool _Enabled; //glEnable(GL_CULL_FACE);
		FaceCullingTest _FaceTest;
		WindingOrder _WindingOrder;
		bool operator==(const FaceCulling& test) const {
			if ((!_Enabled)&&(!test._Enabled)) return true; //if both disabled don't compare values
			return (_FaceTest==test._FaceTest)
				&& (_WindingOrder==test._WindingOrder);
		};
		bool operator!=(const FaceCulling& test) const {
			return !(*this==test);
		}
	};

/////////////////////////////////

	struct RasterisationMode {
		//There are lots of these and I've only implemented UnpackAlignment which is used for the font rendering
		//PixelStorei or f
		//GL_PACK_SWAP_BYTES,
		//GL_PACK_LSB_FIRST,
		//GL_PACK_ROW_LENGTH,
		//GL_PACK_IMAGE_HEIGHT,
		//GL_PACK_SKIP_PIXELS,
		//GL_PACK_SKIP_ROWS,
		//GL_PACK_SKIP_IMAGES,
		//GL_PACK_ALIGNMENT,
		//GL_UNPACK_SWAP_BYTES,
		//GL_UNPACK_LSB_FIRST,
		//GL_UNPACK_ROW_LENGTH,
		//GL_UNPACK_IMAGE_HEIGHT,
		//GL_UNPACK_SKIP_PIXELS,
		//GL_UNPACK_SKIP_ROWS,
		//GL_UNPACK_SKIP_IMAGES,
		//GL_UNPACK_ALIGNMENT

		unsigned int _UnpackAlignment; //1,2,4,8 allowable

		bool operator==(const RasterisationMode& test) const {
			return (_UnpackAlignment==test._UnpackAlignment);
		};
		bool operator!=(const RasterisationMode& test) const {
			return !(*this==test);
		};
	};


/////////////////////////////////
	struct ScissorTest {
		bool _Enabled;
		int _Left;
		int _Bottom;
		unsigned int _Width; //sizei
		unsigned int _Height;
		bool operator==(const ScissorTest& test) const {
			if ((!_Enabled)&&(!test._Enabled)) return true; //if both disabled don't compare values
			return (_Left==test._Left)
				&& (_Bottom==test._Bottom)
				&& (_Width==test._Width)
				&& (_Height==test._Height);
		}
		bool operator!=(const ScissorTest& test) const {
			return !(*this==test);
		}
	};

/////////////////////////////////

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
		bool operator==(const DepthTest& test) const {
			if ((!_Enabled)&&(!test._Enabled)) return true;
			return _Function==test._Function;
		};
		bool operator!=(const DepthTest& test) const {
			return !(*this==test);
		}
	};

/////////////////////////////////

	enum BlendFunction {
		BlendFuncZero = GL_ZERO,
		BlendFuncOne = GL_ONE,
		BlendFuncSrcColour = GL_SRC_COLOR,
		BlendFuncOneMinusSrcColour = GL_ONE_MINUS_SRC_COLOR,
		BlendFuncDstColour = GL_DST_COLOR,
		BlendFuncOneMinusDstColour = GL_ONE_MINUS_DST_COLOR,
		BlendFuncSrcAlpha = GL_SRC_ALPHA,
		BlendFuncOneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
		BlendFuncDstAlpha = GL_DST_ALPHA,
		BlendFuncOneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
		BlendFuncConstantColour = GL_CONSTANT_COLOR,
		BlendFuncOneMinusConstantColour = GL_ONE_MINUS_CONSTANT_COLOR,
		BlendFuncConstantAlpha = GL_CONSTANT_ALPHA,
		BlendFuncOneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
		BlendFuncSrcAlphaSaturate = GL_SRC_ALPHA_SATURATE
	};

	struct BlendingMode {
		bool _Enabled; //you can enable on individual buffers, but this is all of them
		BlendFunction _SFactor;
		BlendFunction _DFactor;
		bool operator==(const BlendingMode& test) const {
			if ((!_Enabled)&&(!test._Enabled)) return true;
			return (_SFactor==test._SFactor)&&(_DFactor==test._DFactor);
		};
		bool operator!=(const BlendingMode& test) const {
			return !(*this==test);
		}
	};

//end of render states
///////////////////////////////////////////////////////////////////////////////////////////////////


	class RenderState
	{
	public:
		RenderState(void);
		~RenderState(void);

		RenderState(const RenderState& rs);

		//PrimitiveRestart
		FaceCulling _FaceCulling;
		RasterisationMode _RasterisationMode;
		ScissorTest _ScissorTest;
		//StencilTest
		DepthTest _DepthTest;
		//DepthRange
		BlendingMode _BlendingMode;
		//ColorMask
		bool _DepthMask;
		//Fog?

		//void SetState(const RenderState& CurrentState);
		RenderState& operator=(const RenderState& rs);
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
