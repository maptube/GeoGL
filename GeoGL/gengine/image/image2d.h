#pragma once

namespace gengine {
	namespace image {

		class Image2D
		{
		public:
			size_t _width;
			size_t _height;
			unsigned int* _raw_buffer;
			Image2D(void);
			~Image2D(void);
		protected:
			void InitBuffer(size_t w, size_t h);
		};

	} //namespace image
} //namespace gengine