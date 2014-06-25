
#include "textureunits.h"
#include "gltexturetypes.h"
#include "texture2d.h"

namespace gengine {
	const unsigned int TextureUnits::NumTexUnits = 16;

	TextureUnits::TextureUnits(void)
	{
		_TexUnits = new const Texture2D*[NumTexUnits];
		for (int i=0; i<NumTexUnits; ++i) {
			_TexUnits[i]=nullptr;
		}
	}


	TextureUnits::~TextureUnits(void)
	{
		delete [] _TexUnits;
	}

	void TextureUnits::Assign(unsigned int TexNumber, const Texture2D* Texture)
	{
		_TexUnits[TexNumber]=Texture;
	}

	/// <summary>Bind textures to the correct texture units</summary>
	void TextureUnits::Bind(void)
	{
		for (unsigned int i=0; i<NumTexUnits; ++i) {
			if (_TexUnits[i]!=nullptr) {
				glActiveTexture(GL_TEXTURE0+i); //this assumes all the texture constants are defined contiguously
				glBindTexture(TexTarget2D,_TexUnits[i]->_texID);
			}
		}
	}

	/// <summary>Unbind any textures that were bound previously</summary>
	void TextureUnits::Unbind(void)
	{
		for (unsigned int i=0; i<NumTexUnits; ++i) {
			if (_TexUnits[i]!=nullptr) {
				glActiveTexture(GL_TEXTURE0+i); //this assumes all the texture constants are defined contiguously
				glBindTexture(TexTarget2D,0);
			}
		}
	}

} //namespace gengine