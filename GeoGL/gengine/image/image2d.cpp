
#include "image2d.h"

namespace gengine {
	namespace image {

		/// <summary>Base class of images e.g. jpeg and png</summary>
		Image2D::Image2D(void)
		{
			_width=0;
			_height=0;
			_raw_buffer = nullptr;
		}

		/// <summary>Initialise the buffer ready to take image data</summary>
		/// <param name="w">Width in pixels</param>
		/// <param name="h">Height in pixels</param>
		void Image2D::InitBuffer(unsigned int w,unsigned int h)
		{
			_width=w;
			_height=h;
			_raw_buffer = new unsigned int[_width*_height]; //ARGB
		}

		Image2D::~Image2D(void)
		{
			if ((_width>0)&&(_height>0))
				delete [] _raw_buffer;
		}

	} //namespace image
} //namespace gengine
