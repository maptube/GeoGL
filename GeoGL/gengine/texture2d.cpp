
#include "texture2d.h"
#include "gengine.h"
#include "gltexturetypes.h"

//////////////////
//You're going to have a problem with multitexturing as this only binds to texture 0!!!!

namespace gengine {

	Texture2D::Texture2D(const int Width, const int Height, const TextureTarget Target, const TexturePixelFormat PixelFormat)
	{
		_width=Width;
		_height=Height;
		_Target = Target;
		//both these three following values could be passed in
		_InternalFormat = TexInternalFormatRGBA; //font passes this as GL_ALPHA?
		_PixelFormat = PixelFormat; //TexPixelRGBA; //is this right, font is passing GL_ALPHA to this?
		_DataType = TexUByte;

		//TODO: why GL_TEXTURE0 - how do you make this controllable generally?
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &_texID);
		glBindTexture(_Target, _texID);
	}


	Texture2D::~Texture2D(void)
	{
		//if programmable shaders?
		glDeleteTextures(1, &_texID); // Delete our Texture Buffer Object
	}

	void Texture2D::SetWrapS(const TextureWrappingOptions Wrapping)
	{
		//should you set the active texture first?
		glTexParameteri(_Target, GL_TEXTURE_WRAP_S, Wrapping);
	}

	void Texture2D::SetWrapT(const TextureWrappingOptions Wrapping)
	{
		glTexParameteri(_Target, GL_TEXTURE_WRAP_T, Wrapping);
	}

	void Texture2D::SetWrapR(const TextureWrappingOptions Wrapping)
	{
		glTexParameteri(_Target, GL_TEXTURE_WRAP_R, Wrapping);
	}

	void Texture2D::SetMinFilter(const TextureMinFilterTypes MinFilter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
	}

	void Texture2D::SetMagFilter(const TextureMagFilterTypes MagFilter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
	}

	/// <summary>
	/// Copy data into the texture from memory. In this case, a pointer to an array of chars.
	/// </summary>
	void Texture2D::CopyFromMemory(unsigned char* mem_texturedata)
	{
		/*glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_ALPHA,
				g->bitmap.width,
				g->bitmap.rows,
				0,
				GL_ALPHA,
				GL_UNSIGNED_BYTE,
				g->bitmap.buffer
			);*/
		GLuint level = 0; //this could be optional for multi-level textures
		GLuint border = 0; //must be zero
		glTexImage2D(_Target,level,_InternalFormat,_width,_height,border,_PixelFormat,_DataType,mem_texturedata);
	}

} //namespace gengine
