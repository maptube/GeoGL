#pragma once

/**
 *	@program		GeoGL: Geographic Graphics Engine, part of MapTube (http://www.maptube.org)
 *	@description	A tool for building 3D representations from geographic data, animations over time and a 3D GIS.
 *	@author			Richard Milton
 *	@organisation	Centre for Advanced Spatial Analysis (CASA), University College London
 *	@website		http://www.casa.ucl.ac.uk
 *	@date			9 May 2014
 *	@licence		Free for all commercial and academic use (including teaching). Not for commercial use. All rights are held by the author.
 */

#include "glbuffertypes.h"

namespace gengine {

	//An index buffer is similar to a vertex buffer, but can only store int values
	class IndexBuffer
	{
	public:
		IndexBuffer(void);
		IndexBuffer(BufferTarget Target, BufferUsage Usage, unsigned int NumBytes);
		~IndexBuffer(void);

		void CopyFromMemory(unsigned int* mem_indexdata);
		//void CopyToMemory();
		GLuint _SizeBytes;
		GLuint _iboID; //our Index Buffer Object
		GLuint* _mem_indexbuffer; //In memory buffer used if no support for hardware vertex buffers
		BufferTarget _Target;
		BufferUsage _Usage;
	};

} //namespace gengine
