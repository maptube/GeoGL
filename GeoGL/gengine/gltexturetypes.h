#pragma once
#include "gengine.h"

namespace gengine {

	//OpenGL type wrappers for texture functions

	//specifies the type of texture we want to create
	enum TextureTarget {
		TexTarget1D = GL_TEXTURE_1D,
		TexTarget2D = GL_TEXTURE_2D,
		TexTarget1DArray = GL_TEXTURE_1D_ARRAY,
		TexTarget2DArray = GL_TEXTURE_2D_ARRAY,
		TexTarget3D = GL_TEXTURE_3D,
		TexTargetRectangle = GL_TEXTURE_RECTANGLE,
		TexTargetBuffer = GL_TEXTURE_BUFFER,
		TexTargetCubeMap = GL_TEXTURE_CUBE_MAP,
		TexTarget2DMultiSample = GL_TEXTURE_2D_MULTISAMPLE,
		TexTarget2DMultiSampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	};
	//GL_TEXTURE_2D,
	//GL_PROXY_TEXTURE_2D,
	//GL_TEXTURE_1D_ARRAY,
	//GL_PROXY_TEXTURE_1D_ARRAY,
	//GL_TEXTURE_RECTANGLE,
	//GL_PROXY_TEXTURE_RECTANGLE,
	//GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	//GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	//GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	//GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	//GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	//GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	//GL_PROXY_TEXTURE_CUBE_MAP

	//InternalFormat - taken from glTexImage2D manual page
	enum TextureInternalFormat {
		//base internal formats
		TexInternalFormatDepthComponent = GL_DEPTH_COMPONENT,
		TexInternalFormatDepthStencil = GL_DEPTH_STENCIL,
		TexInternalFormatRed = GL_RED,
		TexInternalFormatRG = GL_RG,
		TexInternalFormatRGB = GL_RGB,
		TexInternalFormatRGBA = GL_RGBA,
		TexInternalFormatALPHA = GL_ALPHA
		//sized internal formats
		//GL_R8
		//GL_R8_SNORM
		//GL_R16
		//GL_R16_SNORM
		//GL_RG8
		//GL_RG8_SNORM
		//GL_RG16
		//GL_RG16_SNORM
		//GL_R3_G3_B2
		//GL_RGB4
		//GL_RGB5
		//GL_RGB8
		//GL_RGB8_SNORM
		//GL_RGB10
		//GL_RGB12
		//GL_RGB16_SNORM
		//GL_RGBA2
		//GL_RGBA4
		//GL_RGB5_A1
		//GL_RGBA8
		//GL_RGBA8_SNORM
		//GL_RGB10_A2
		//GL_RGB10_A2UI
		//GL_RGBA12
		//GL_RGBA16
		//GL_SRGB8
		//GL_SRGB8_ALPHA8
		//GL_R16F
		//GL_RG16F
		//GL_RGB16F
		//GL_RGBA16F
		//GL_R32F
		//GL_RG32F
		//GL_RGB32F
		//GL_RGBA32F
		//GL_R11F_G11F_B10F
		//GL_RGB9_E5
		//GL_R8I
		//GL_R8UI
		//GL_R16I
		//GL_R16UI
		//GL_R32I
		//GL_R32UI
		//GL_RG8I
		//GL_RG8UI
		//GL_RG16I
		//GL_RG16UI
		//GL_RG32I
		//GL_RG32UI
		//GL_RGB8I
		//GL_RGB8UI
		//GL_RGB16I
		//GL_RGB16UI
		//GL_RGB32I
		//GL_RGB32UI
		//GL_RGBA8I
		//GL_RGBA8UI
		//GL_RGBA16I
		//GL_RGBA16UI
		//GL_RGBA32I
		//GL_RGBA32UI

		//compressed internal formats
		//GL_COMPRESSED_RED,
		//GL_COMPRESSED_RG,
		//GL_COMPRESSED_RGB,
		//GL_COMPRESSED_RGBA,
		//GL_COMPRESSED_SRGB,
		//GL_COMPRESSED_SRGB_ALPHA,
		//GL_COMPRESSED_RED_RGTC1,
		//GL_COMPRESSED_SIGNED_RED_RGTC1,
		//GL_COMPRESSED_RG_RGTC2,
		//GL_COMPRESSED_SIGNED_RG_RGTC2,
		//GL_COMPRESSED_RGBA_BPTC_UNORM,
		//GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
		//GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
		//GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
	};

	//PixelFormat
	enum TexturePixelFormat {
		TexPixelR = GL_R,
		TexPixelRG = GL_RG,
		TexPixelRGB = GL_RGB,
		TexPixelBGR = GL_BGR,
		TexPixelRGBA = GL_RGBA,
		TexPixelBGRA = GL_BGRA,
		TexPixelRInteger = GL_RED_INTEGER,
		TexPixelRGInteger = GL_RG_INTEGER,
		TexPixelRGBInteger = GL_RGB_INTEGER,
		TexPixelBGRInteger = GL_BGR_INTEGER,
		TexPixelRGBAInteger = GL_RGBA_INTEGER,
		TexPixelBGRAInteger = GL_BGRA_INTEGER,
		TexPixelStencilIndex = GL_STENCIL_INDEX,
		TexPixelDepthComponent = GL_DEPTH_COMPONENT,
		TexPixelDepthStencil = GL_DEPTH_STENCIL,
		TexPixelAlpha = GL_ALPHA //added this as the font rendering uses it, but it's not in any of the docs - it's just a single alpha channel
	};

	//Storage format for image data
	enum TextureDataType {
		TexUByte = GL_UNSIGNED_BYTE,	//<<<<<<<<<< This one is the one we're using
		TexByte = GL_BYTE,
		TexUShort = GL_UNSIGNED_SHORT,
		TexShort = GL_SHORT,
		TexUInt = GL_UNSIGNED_INT,
		TexInt = GL_INT,
		TexFloat = GL_FLOAT,
		TexUByte332 = GL_UNSIGNED_BYTE_3_3_2,
		TexUByte233REV = GL_UNSIGNED_BYTE_2_3_3_REV,
		TexUShort565 = GL_UNSIGNED_SHORT_5_6_5,
		TexUShort565REV = GL_UNSIGNED_SHORT_5_6_5_REV,
		TexUShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
		TexUShort4444REV = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		TexUShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
		TexUShort1555REV = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		TexUInt8888 = GL_UNSIGNED_INT_8_8_8_8,
		TexUInt8888REV = GL_UNSIGNED_INT_8_8_8_8_REV,
		TexUInt1010102 = GL_UNSIGNED_INT_10_10_10_2,
		TexUInt2101010REV = GL_UNSIGNED_INT_2_10_10_10_REV
	};

	//texture wrapping
	//enum TextureWrapChannel {
	//	TexWrapS = GL_TEXTURE_WRAP_S,
	//	TexWrapT = GL_TEXTURE_WRAP_T,
	//	TexWrapR = GL_TEXTURE_WRAP_R
	//};

	enum TextureWrappingOptions {
		TexClamp = GL_CLAMP,
		TexClampToEdge = GL_CLAMP_TO_EDGE,
		TexRepeat = GL_REPEAT
	};

	enum TextureMinFilterTypes {
		TexMinFilterNearest = GL_NEAREST,
		TexMinFilterLinear = GL_LINEAR,
		TexMinFilterNearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
		TexMinFilterLinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
		TexMinFilterNearestMipMapLinear = GL_NEAREST_MIPMAP_LINEAR,
		TexMinFilterLinearMipMapLinear = GL_LINEAR_MIPMAP_LINEAR
	};

	enum TextureMagFilterTypes {
		TexMagFilterNearest = GL_NEAREST,
		TexMagFilterLinear = GL_LINEAR
	};



} //namespace gengine