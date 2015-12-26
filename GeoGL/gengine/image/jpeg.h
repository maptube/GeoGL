#pragma once

#include "image2d.h"

#include <string>

namespace gengine {
	namespace image {

		class Jpeg : public Image2D
		{
		public:
			Jpeg(void);
			Jpeg(const std::string& Filename);
			~Jpeg(void);
		};
	
	} //namespace image
} //namespace gengine