/*
 * agentobject.cpp
 *
 *  Created on: 3 Jan 2016
 *      Author: richard
 */

#include "agentobject.h"

#include "gengine/graphicscontext.h"
#include "gengine/scenedataobject.h"
//#include "gengine/ogldevice.h"
//#include "gengine/glbuffertypes.h"
//#include "gengine/vertexbuffer.h"
//#include "gengine/indexbuffer.h"
//#include "gengine/vertexformat.h"
//#include "gengine/vertexdata.h"
//#include "gengine/renderstate.h"
//#include "gengine/primitivetypes.h"
#include "gengine/shader.h"
#include "gengine/shaderuniform.h"
#include "gengine/shaderuniformcollection.h"
#include "mesh2.h"

using namespace gengine;

/// <summary>
/// Create an agent object wrapper for an agent shape mesh. This object exists in the scene and is marked as renderable, but it owns a pointer to a shared mesh
/// for the agent's shape, which is re-used by all agents to do the actual rendering.
/// </summary>
/// <param name="pShapeMesh">Pointer to the Mesh2 which contains the shape for this object for rendering in the scene</param>
AgentObject::AgentObject(Mesh2* pShapeMesh) {
	// TODO Auto-generated constructor stub
	pMesh = pShapeMesh;
	//NOTE: if you had mask=1 and colour=0, then it would default to the buffer colours until you changed it
	Colour=glm::vec3(0,0,0); //default null colour, so it uses the mesh data until set explicitly
	ColourMask=glm::vec3(1,1,1); //default mask of one so colour uses the mesh data (no blending)
}

AgentObject::~AgentObject() {
	// TODO Auto-generated destructor stub
}

/// <summary>
/// Change the shape which is rendered for this agent by changing the mesh pointer to a different mesh.
/// </summary>
void AgentObject::SetMesh(Mesh2* pShapeMesh) {
	pMesh = pShapeMesh;
}

/// <summary>
/// Change the colour of this agent. Using the agent shader, the colour and a mask are passed at the point of render, so a single
/// template agent mesh can be used for all agents of this breed and shape. The colours of each agent can be controlled without having
/// to re-create the mesh by using the shader. The colour here is used with a mask to alter the render colour on the fly. The final colour
/// is calculated as follows:
/// render_colour = colour_buffer * mask + colour
/// Where mask and colour are from this object and colour_buffer is the colour buffer associated with the agent mesh.
/// In other words, if the mask is (0,0,0), then the colour is whatever colour you set here. If it's (0.5,0.5,0.5) then you
/// get half of the original plus half of the colour here, but clamped to 1.0 so you have to make sure you don't saturate it.
/// TODO: there is currently no way of setting the mask.
/// </summary>
/// <param name="new_colour">The new colour for this agent mesh. Passed to agent shader on render.</param>
void AgentObject::SetColour(glm::vec3 new_colour) {
	Colour = new_colour;
	ColourMask = glm::vec3(0,0,0); //this is a bit of a hack, but set the mask to zero once a colour on this object has been explicitly set, so it uses that colour 100%, NOT the mesh colours
}

/// <summary>
/// Set the size of the agent mesh. This sets the scaling on the matrix, which is used with a unit sized mesh. Scaling can be set on each axis individually.
/// </summary>
/// <param name="Sx">Scale on X axis</param>
/// <param name="Sy">Scale on Y axis</param>
/// <param name="Sz">Scale on Z axis</param>
void AgentObject::SetSize(double Sx,double Sy,double Sz) {
	SizeMatrix = glm::scale(glm::mat4(1.0f),glm::vec3((float)Sx,(float)Sy,(float)Sz));
}

/// <summary>
/// Shaders are attached to each mesh as a pointer to a shader in a global collection. This way, we're not
/// using a separate shader for each mesh in the scene which would be very bad.
/// Also, objects are drawn so as to minimise the number of shader program switches for optimisation.
/// If recursive is set, then this object and all its children have their shader assigned. If recursive is
/// false, then only this object is set. Also, any object where HasGeometry==false does not have the shader
/// set as there is no draw object to set the shader on, only a null draw object.
/// The AgentObject is slightly different from a Mesh, as it's a wrapper for a mesh to allow for a special shader
/// to be used for the agents. For this reason, it only needs to pass on the AttachShader call to the mesh that
/// it is wrapping.
/// </summary>
/// <param name="pShader">Pointer to the shader program to attach i.e. use for drawing this object</param>
/// <param name="Recursive">If true, set this object and all children recursively, otherwise only set this object</param>
void AgentObject::AttachShader(gengine::Shader* pShader, bool Recursive)
{
	pMesh->AttachShader(pShader,Recursive);
}

/// <summary>
/// Return a bounding box for the geometry contained by this mesh wrapper object. Slightly different from Object3D or Mesh2 as we
/// are wrapping a mesh in the AgentObject due to the special way in which agents can change their colours using a custom agent shader.
/// </summary>
/// <returns>The bounding box of the geometry that this agent wraps in its pMesh property</returns>
BBox AgentObject::GetGeometryBounds() {
	/*BBox box;
	box.Union(bounds); //initialise to parent
	for (vector<Object3D*>::iterator it=Children.begin(); it!=Children.end(); ++it) {
		Object3D* o3d = *it;
		//take child box, transform by model matrix and add on to this object's box
		BBox childBox = o3d->GetGeometryBounds(); //get bounds for child and all its children recursively
		childBox.min = glm::vec3( modelMatrix * glm::vec4(childBox.min,1) );
		childBox.max = glm::vec3( modelMatrix * glm::vec4(childBox.max,1) );
		box.Union(childBox); //this should return the bounds for the object and all its children
	}
	return box;*/
	return pMesh->GetGeometryBounds(); //assuming this object has no children
}

void AgentObject::Render(gengine::GraphicsContext* GC,const gengine::SceneDataObject& sdo) {
	//from Mesh2
	//const DrawObject& dobj=GetDrawObject();
	//GC->Render(dobj,sdo);

	//AgentObject code
	const DrawObject& dobj=GetDrawObject();
	//ADDED COLOUR CODE
	ShaderUniformCollection* uniforms=dobj._ShaderProgram->_shaderUniforms;
	(*uniforms)["u_colour"]=Colour;
	(*uniforms)["u_colourMask"]=ColourMask;
	GC->Render(dobj,sdo);
}

const gengine::DrawObject& AgentObject::GetDrawObject() {
	//from Mesh2
	//we need to get the modelMatrix from the parent class property and put it into the draw object
	//TODO: you could pass in a parent modelMatrix and do the pre-multiply here?
	//drawObject._ModelMatrix = this->modelMatrix;
	//return drawObject;

	//AgentObject code
	//TODO: need to change some things here
	//gengine::DrawObject dobj = pMesh->GetDrawObject();
	//dobj._ModelMatrix = this->modelMatrix;
	//return dobj;

	pMesh->SetMatrix(modelMatrix * SizeMatrix);
	//dobj._ModelMatrix = this->modelMatrix;
	return pMesh->GetDrawObject();
}

/// <summary>
/// Set the model matrix. Virtual as an object with a mesh should also copy it to the draw object.
/// </summary>
/// <param name="matrix">The new matrix to copy into this obejct's model matrix</param>
void AgentObject::SetMatrix(const glm::mat4& matrix) {
	this->modelMatrix=matrix;
	pMesh->SetMatrix(matrix); //set the matrix on the mesh so its draw object matrix gets set and we can just return its DrawObject
	modelMatrix = matrix;
}

