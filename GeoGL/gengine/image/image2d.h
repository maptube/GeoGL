#pragma once

namespace gengine {
	namespace image {

		class Image2D
		{
		public:
			unsigned int _width;
			unsigned int _height;
			unsigned int* _raw_buffer;
			Image2D(void);
			~Image2D(void);
		protected:
			void InitBuffer(unsigned int w, unsigned int h);
		};

	} //namespace image
} //namespace gengine
