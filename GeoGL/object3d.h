#pragma once
#include "main.h"
#include "BBox.h"
#include <vector>

//base class of all 3D objects in the world
//TODO: maybe pass the shader into the object so you can have different parts of the tree rendered with different shaders?
class Object3D {
public:
	std::string Name;
	BBox bounds; //bounding box

	glm::mat4 modelMatrix; // Store the model matrix (promoted from protected)

	Object3D();
	~Object3D();
	void GetPos(float& X, float& Y, float& Z);
	void SetPos(float X, float Y, float Z);
	void Rotate(float Angle,glm::vec3 V);
	//void Render(unsigned int ShaderId, glm::mat4 ParentMat);
	virtual void Render(glm::mat4 ParentMat);
	void AddChild(Object3D* Child);
	virtual BBox GetGeometryBounds();
	virtual void ComputeBounds();

protected:
	//glm::mat4 modelMatrix; // Store the model matrix
	//GLuint vaoID; //Vertex Array Object (holds context for vertex type)
	//GLuint vboID; //Vertex Buffer Object
	//GLuint iboID; //Index Buffer Object
	//GLuint vcboID; //Vertex Colour Buffer Object
	//unsigned int NumElements; //number of elements passed to OGL drawelements function i.e. number of index elements

	//hierarchy
	std::vector<Object3D*> Children;

	//fallback for OpenGL<2.0, storage for vertex, colour and index buffers
	//GLfloat* mem_vertexbuffer;
	//GLfloat* mem_colourbuffer;
	//GLuint* mem_indexbuffer;
};