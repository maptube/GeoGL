#pragma once
#include "gengine.h"

#include "gltexturetypes.h"

namespace gengine {

	class Texture2D
	{
	public:
		Texture2D(const int Width, const int Height, const TextureTarget Target, const TexturePixelFormat PixelFormat);
		~Texture2D(void);

		void SetWrapS(const TextureWrappingOptions Wrapping);
		void SetWrapT(const TextureWrappingOptions Wrapping);
		void SetWrapR(const TextureWrappingOptions Wrapping);
		void SetMinFilter(const TextureMinFilterTypes MinFilter);
		void SetMagFilter(const TextureMagFilterTypes MagFilter);

		void CopyFromMemory(unsigned char* mem_texturedata);
		//float* CopyToMemory(void);

		unsigned int _width, _height;
		//GLuint _SizeBytes;
		GLuint _texID; //our Texture object
		//GLfloat* _mem_vertexbuffer; //In memory buffer used if no support for hardware vertex buffers
		TextureTarget _Target;
		TextureInternalFormat _InternalFormat; //internal format of how data is stored
		TexturePixelFormat _PixelFormat; //i.e. RGBA or whatever
		TextureDataType _DataType; //i.e. whether we're storing bytes, ubytes, floats etc
	};

} //namespace gengine