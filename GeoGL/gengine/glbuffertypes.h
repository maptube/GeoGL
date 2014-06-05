#pragma once
#include "gengine.h"

namespace gengine {

	//OpenGL type wrappers for buffering functions i.e. create vertex buffer and create index buffer

	//specifies the type of buffer we want to create
	enum BufferTarget {
		ArrayBuffer = GL_ARRAY_BUFFER,
		CopyReadBuffer = GL_COPY_READ_BUFFER,
		CopyWriteBuffer = GL_COPY_WRITE_BUFFER,
		ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER,
		PixelPackBuffer = GL_PIXEL_PACK_BUFFER,
		PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER,
		//TextureBuffer = GL_TEXTURE_BUFFER, //also defined in gltexturetypes.h
		TransformFeedbackBuffer = GL_TRANSFORM_FEEDBACK_BUFFER,
		UniformBuffer = GL_UNIFORM_BUFFER
	};

	//specifies how the buffer is going to be used (optimised)
	enum BufferUsage {
		StreamDraw = GL_STREAM_DRAW, //Stream=Data store contents modified once and used at most a few times
		StreamRead = GL_STREAM_READ,
		StreamCopy = GL_STREAM_COPY,
		StaticDraw = GL_STATIC_DRAW, //Static=Data store contents modified once and used many times
		StaticRead = GL_STATIC_READ,
		StaticCopy = GL_STATIC_COPY,
		DynamicDraw = GL_DYNAMIC_DRAW, //Dynamic=Data store contents modified repeatedly and used many times
		DynamicRead = GL_DYNAMIC_READ,
		DynamicCopy = GL_DYNAMIC_COPY
	};

} //namespace gengine
