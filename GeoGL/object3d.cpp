//Object3D.cpp
#include "object3d.h"
//#include "opengl4.h"

using namespace std;

/// <summary>Constructor</summary>
Object3D::Object3D() {
	//modelMatrix = glm::scale(glm::mat4(0.1f), glm::vec3(0.5f));  // Create our model matrix
	//modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f));
	//model matrix gets initialised to identity anyway
}

/// <summary>Destructor</summary>
Object3D::~Object3D() {
	//cout<<"Destructor Object3D"<<endl;
	//tidy up the buffers?
	//glDeleteBuffers(1, &iboID[0]); //delete the buffer
	//and tidy up children?
}

/// <summary>
/// Get the absolute (no - relative to parent!) position of the object
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <param name="Z"></param>
void Object3D::GetPos(double& X, double& Y, double& Z)
{
	X = (double)modelMatrix[3][0];
	Y = (double)modelMatrix[3][1];
	Z = (double)modelMatrix[3][2];
}

/// <summary>
/// Set the absolute (no - relative to parent!) position of the object
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <param name="Z"></param>
void Object3D::SetPos(double X, double Y, double Z) {
	//TODO:
	//Also, you need to translate the bounding box as well
	glm::dvec3 Txyz(X, Y, Z);
	//modelMatrix = glm::translate(modelMatrix,Txyz); //TODO: check what this does!!!! it's a relative translation not absolute
	//direct manipulation of position
	modelMatrix[3][0]=(float)X;
	modelMatrix[3][1]=(float)Y;
	modelMatrix[3][2]=(float)Z;
	//need to recompute bounding box, so just need to translate it
	bounds.max = bounds.max + Txyz;
	bounds.min = bounds.min + Txyz;
}


/// <summary>Rotate model view matrix</summary>
/// <param name="Angle">Angle is in radians</param>
/// <param name="Vector">Vector is line to rotate around</param>
void Object3D::Rotate(float Angle,glm::vec3 V) {
	modelMatrix = glm::rotate(modelMatrix,Angle,V);
}

/// <summary>Add a child object to the scene graph</summary>
/// <param name="Child"></param>
void Object3D::AddChild(Object3D* Child) {
	Children.push_back(Child);
}

/// <summary>Remove child object from the scene graph. This does not delete the object, only remove it from the scene.</summary>
void Object3D::RemoveChild(Object3D* Child) {
	for (vector<Object3D*>::iterator it=Children.begin(); it!=Children.end(); ++it) {
		Object3D* o3d = *it;
		if (o3d==Child) {
			Children.erase(it);
			break;
		}
	}
}


/// <summary>Get bounding box for this object and all its children, but only for the geometry that you can actually see.
/// A pure Object3D returns nothing as you can't see it. A mesh and its subclasses return the bounds of their contained face vertices.</summary>
BBox Object3D::GetGeometryBounds() {
	BBox box;
	//box.Union(bounds); //initialise to parent
	for (vector<Object3D*>::iterator it=Children.begin(); it!=Children.end(); ++it) {
		Object3D* o3d = *it;
		//take child box, transform by model matrix and add on to this object's box
		BBox childBox = o3d->GetGeometryBounds(); //get bounds for child and all its children recursively
		childBox.min = glm::vec3( modelMatrix * glm::vec4(childBox.min,1) );
		childBox.max = glm::vec3( modelMatrix * glm::vec4(childBox.max,1) );
		box.Union(childBox); //this should return the bounds for the object and all its children
	}
	return box;
}

/// <summary>TODO: Compute Bounds</summary>
void Object3D::ComputeBounds() {
	//recalculate the bounding box for this object
	//TODO: needs to have the geometry
}


/// <summary>Render the object to the OpenGL context
/// NOTE: you could use a static stack for the parent matrix
/// </summary>
/// <param name="ShaderId">ShaderId is the id of the shader which render is going to need to set the modelview matrix of</param>
/// <param name="ParentMat">ParentMat is matrix of the object that is the parent of this object (i.e. we are relative to this matrix)</param>
//void Object3D::Render(unsigned int ShaderId, glm::mat4 ParentMat) {
//	
//	//shouldn't we be multiplying these?
//	//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader
//	//multiplying...
//	glm::mat4 mm = ParentMat * modelMatrix; //post multiply child matrix
//
//	if (OpenGLContext::hasProgrammableShaders) { //or ShaderId==-1 ?
//		int modelMatrixLocation = glGetUniformLocation(ShaderId, "modelMatrix"); // Get the location of our model matrix in the shader
//
//		//glm::mat4 save_modelMatrix;
//		//glGetFloatv(GL_MODELVIEW_MATRIX, &save_modelMatrix[0][0]); //apparently, there is a time penalty for this and is it deprecated?
//	
//		
//		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &mm[0][0]); // Send our model matrix to the shader
//
//		//render top level element if it has any geometry
//		if (NumElements>0) {
//			//generic indexed triangles buffer render 
//			glBindVertexArray(vaoID);
//			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,iboID);
//			//glBindBuffer(GL_ARRAY_BUFFER,vcboID);
//			glDrawElements(GL_TRIANGLES,NumElements,GL_UNSIGNED_INT/*GL_UNSIGNED_BYTE*/,(void*)0);
//			//glBindVertexArray(0);
//			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
//		}
//	}
//	else {
//		//fallback if no shaders supported
//		glMatrixMode(GL_MODELVIEW);
//		glLoadMatrixf(&mm[0][0]); //check this...
//		if (NumElements>0) {
//			glEnableClientState(GL_VERTEX_ARRAY);
//			glVertexPointer(3,GL_FLOAT,0,mem_vertexbuffer);
//			glEnableClientState(GL_COLOR_ARRAY);
//			glColorPointer(3,GL_FLOAT,0,mem_colourbuffer);
//			glDrawElements(GL_TRIANGLES,NumElements,GL_UNSIGNED_INT,mem_indexbuffer);
//		}
//	}
//	
//	//then go on to render all the children
//	for (vector<Object3D*>::iterator childIT=Children.begin(); childIT!=Children.end(); ++childIT) {
//		(*childIT)->Render(ShaderId,mm);
//	}
//
//}

//new render - not strictly necessary as we're using the GetDrawObject method
//void Object3D::Render(glm::mat4 ParentMat) {
//	//default implementation of an Object3D has no geometry, so you can't render anything
//	
//	glm::mat4 mm = ParentMat * modelMatrix; //post multiply child matrix
//
//	//then go on to render all the children
//	for (vector<Object3D*>::iterator childIT=Children.begin(); childIT!=Children.end(); ++childIT) {
//		(*childIT)->Render(mm);
//	}
//}

/// <summary>
/// Render the object to the graphics context. In this class, it does nothing as there is nothing to render.
/// NOTE: child objects don't get rendered, only the parent.
/// </summary>
void Object3D::Render(gengine::GraphicsContext* GC, const gengine::SceneDataObject& sdo) {
	//DO NOTHING
}

const gengine::DrawObject& Object3D::GetDrawObject() {
	//this is also a nasty NullDrawObject kludge that I would like to get rid of
	return (gengine::DrawObject &)drawObject; //this returns a reference to a null one
}

/// <summary>
/// Shaders are attached to each mesh as a pointer to a shader in a global collection. This way, we're not
/// using a separate shader for each mesh in the scene which would be very bad.
/// Also, objects are drawn so as to minimise the number of shader program switches for optimisation.
/// If recursive is set, then this object and all its children have their shader assigned. If recursive is
/// false, then only this object is set. Also, any object where HasGeometry==false does not have the shader
/// set as there is no draw object to set the shader on, only a null draw object.
/// </summary>
/// <param name="pShader">Pointer to the shader program to attach i.e. use for drawing this object</param>
/// <param name="Recursive">If true, set this object and all children recursively, otherwise only set this object</param>
void Object3D::AttachShader(gengine::Shader* pShader, bool Recursive) {
	//parent object not drawable, so no shader to attach to this
	if (Recursive) {
		//now set all the children - NOTE: you still need to call the attach virtual even if a child is an Object3D as
		//it still might have children which are drawable.
		for (vector<Object3D*>::iterator childIT=Children.begin(); childIT!=Children.end(); ++childIT) {
			(*childIT)->AttachShader(pShader,true);
		}
	}
}

/// <summary>
/// Set the model matrix. Virtual as an object with a mesh should also copy it to the draw object.
/// </summary>
/// <param name="matrix">The new matrix to copy into this obejct's model matrix</param>
void Object3D::SetMatrix(const glm::mat4& matrix) {
	modelMatrix = matrix;
}
