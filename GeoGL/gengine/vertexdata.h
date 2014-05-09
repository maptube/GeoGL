#pragma once

#include <vector>

namespace gengine {

	//forward references
	class ShaderAttributeCollection;
	class VertexBuffer;
	class IndexBuffer;

	//VertexData class is a container to hold buffers are bound to for rendering.
	//Basically it holds 1 or more vertex buffers and 0 or 1 index buffers.
	//bind requires the shader's attribute collection in order to bind the buffers to the correct locations.
	//TODO: a more elegant solution might be to put the location binding onto the buffer after shader compilation?
	class VertexData
	{
	public:
		VertexData(void);
		~VertexData(void);

		std::vector<VertexBuffer*> _vb; //1 or more vertex buffers
		IndexBuffer* _ib; //optional index buffer if used
		unsigned int _NumElements; //number of elements passed to OGL drawelements function i.e. number of index elements

		void bind(ShaderAttributeCollection& attributes);
		void unbind(ShaderAttributeCollection& attributes);
	};

} //namespace gengine
