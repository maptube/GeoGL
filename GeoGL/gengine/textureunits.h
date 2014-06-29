#pragma once

//class to handle binding textures for shaders
//This class only holds references to textures and is not responsible for creating or freeing the textures which need to
//be explicitly destroyed with delete.

#include <map>

#include "gengine.h"

//forward declarations
namespace gengine {
	class Texture2D;
}

namespace gengine {
	class TextureUnits
	{
	private:
		//DIRTY BITS!
		const Texture2D** _TexUnits; //array of pointers to const Texture2Ds
	public:
		static const unsigned int NumTexUnits;

		TextureUnits(void);
		~TextureUnits(void);

		void Assign(unsigned int TexNumber, const Texture2D* Texture);
		void Bind(void);
		void Bind(const std::map<unsigned int,Texture2D*>& TexturesMap);
		void Unbind(void);
	};

} //namespace gengine
