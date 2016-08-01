#pragma once
/*
 * agentobject.h
 *
 *  Created on: 3 Jan 2016
 *      Author: richard
 */

#include "gengine/gengine.h"
#include "gengine/vertexformat.h"
//#include "gengine/vertexbuffer.h"
//#include "gengine/indexbuffer.h"
//#include "gengine/shader.h"
#include "gengine/drawobject.h"
#include "object3d.h"
#include "BBox.h"

//forward declarations
namespace gengine {
	class GraphicsContext;
	class SceneDataObject;
	//class VertexBuffer;
	//class IndexBuffer;
	//class Shader;
	//class Texture2D;
	//class Shader;
}

class Mesh2;
//end of forward declarations


/// <summary>
/// Placeholder for an agent in the scene. Uses a shared mesh for this shape and a custom renderer for the colour.
/// </summary>
class AgentObject : public Object3D {
public:
	//enum gengine::VertexFormat _VertexFormat; //Format of buffers used e.g. separate or interleaved, position, colour, texture, normal
	//bool _HasIndexBuffer; //if false, then don't create the index buffer in CreateBuffers

	glm::vec3 Colour;
	glm::vec3 ColourMask;

	AgentObject(Mesh2* pShapeMesh);
	virtual ~AgentObject();

	void SetMesh(Mesh2* pShapeMesh);
	void SetColour(glm::vec3 new_colour);
	void SetSize(double Sx,double Sy,double Sz);

	virtual void AttachShader(gengine::Shader* pShader, bool Recursive);
	virtual BBox GetGeometryBounds();
	virtual void Render(gengine::GraphicsContext* GC, const gengine::SceneDataObject& sdo);
	virtual bool HasGeometry() { return true; } //yes, this object has geometry associated with it and can be rendered
	virtual const gengine::DrawObject& GetDrawObject();
	virtual void SetMatrix(const glm::mat4& matrix);
protected:
	Mesh2* pMesh;
	glm::mat4 SizeMatrix; //this is the scaling applied to the unit size mesh when calling SetSize()
	//gengine::VertexData* vertexData; //collection of vertex and index buffers
	//gengine::RenderState* renderState;
	//gengine::DrawObject drawObject; //contains textures
};

