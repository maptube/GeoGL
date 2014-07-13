#pragma once
//vertexformat.h
//enum for vertex data format when creating buffers via mesh2

namespace gengine {

	//look at the bit positions
	enum VertexFormat {
		Position=0x1,								//position buffer only
		PositionColour=0x3,							//position and colour buffers as two separate buffers
		PositionColourTexture=0x7,					//position, colour, texture as three separate buffers
		PositionColourTextureNormal=0xf,			//position, colour, texture, normals as four separate buffers
		PositionColourNormal=0xb,					//position, colour, normals as three separate buffers
		InterleavedPositionColour=0x13,				//position and colour as one buffer containing interleaved data
		InterleavedPositionColourTexture=0x17,		//position, colour and texture as one buffer
		InterleavedPositionColourNormal=0x1b,		//position, colour and normals as one buffer
		InterleavedPositionColourTextureNormal=0x1f	//position, colour, texture and normals as one buffer
	};
	const unsigned int VertexFormatPositionBit=1;
	const unsigned int VertexFormatColourBit=2;
	const unsigned int VertexFormatTextureBit=4;
	const unsigned int VertexFormatNormalBit=8;

} //namespace gengine
