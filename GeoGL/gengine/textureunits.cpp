
#include "textureunits.h"

#include <map>

#include "gltexturetypes.h"
#include "texture2d.h"

using namespace std;

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
		//set dirty
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

	/// <summary>
	/// Bind textures to texture units using a map of texture unit number and texture 2d object.
	/// This is the format used to store textures in DrawObject classes, so is used before drawing the object.
	/// </summary>
	void TextureUnits::Bind(const std::map<unsigned int,Texture2D*>& TexturesMap)
	{
		for (map<unsigned int,Texture2D*>::const_iterator it = TexturesMap.begin(); it!=TexturesMap.end(); ++it) {
			unsigned int UnitNum = it->first;
			Texture2D* tex = it->second;
//TODO: check why this isn't working - is it the shader rebinding?
			//if (_TexUnits[UnitNum]!=tex) {
				_TexUnits[UnitNum]=tex;
				//set dirty, but not if we bind it here immediately
				glActiveTexture(GL_TEXTURE0+UnitNum);
				glBindTexture(TexTarget2D,_TexUnits[UnitNum]->_texID);
			//}
		}

	}

	/// <summary>Unbind any textures that were bound previously</summary>
	void TextureUnits::Unbind(void)
	{
		for (unsigned int i=0; i<NumTexUnits; ++i) {
			if (_TexUnits[i]!=nullptr) {
				glActiveTexture(GL_TEXTURE0+i); //this assumes all the texture constants are defined contiguously
				glBindTexture(TexTarget2D,0);
				_TexUnits[i]=nullptr;
			}
		}
	}

} //namespace gengine
