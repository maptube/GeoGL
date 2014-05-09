
#include "meshcube.h"

//Surely this class is redundant as it's replaced by the cuboid class?

MeshCube::MeshCube(void)
{
	//create a unit cube using the mesh helpers
	glm::vec3 p[] = {
		//front face
		glm::vec3( 0.5, -0.5,  0.5), //0
		glm::vec3( 0.5,  0.5,  0.5), //1
		glm::vec3(-0.5,  0.5,  0.5), //2
		glm::vec3(-0.5, -0.5,  0.5), //3
		//back face
		glm::vec3( 0.5, -0.5, -0.5), //4
		glm::vec3( 0.5,  0.5, -0.5), //5
		glm::vec3(-0.5,  0.5, -0.5), //6
		glm::vec3(-0.5, -0.5, -0.5)  //7
	};

	//add faces one at a time here
	glm::vec3 red(1.0,0.0,0.0);
	glm::vec3 green(0.0,1.0,0.0);
	glm::vec3 blue(0.0,0.0,1.0);
	glm::vec3 yellow(1.0,1.0,0.0);
	glm::vec3 cyan(0.0,1.0,1.0);
	glm::vec3 magenta(1.0,0.0,1.0);
	AddFace(p[0],p[1],p[2],red,red,red); //front
	AddFace(p[0],p[2],p[3],red,red,red);
	AddFace(p[4],p[5],p[1],green,green,green); //right
	AddFace(p[4],p[1],p[0],green,green,green);
	AddFace(p[1],p[5],p[6],blue,blue,blue); //top
	AddFace(p[1],p[6],p[2],blue,blue,blue);
	AddFace(p[3],p[2],p[6],yellow,yellow,yellow); //left
	AddFace(p[3],p[6],p[7],yellow,yellow,yellow);
	AddFace(p[4],p[0],p[3],cyan,cyan,cyan); //bottom
	AddFace(p[4],p[3],p[7],cyan,cyan,cyan);
	AddFace(p[7],p[6],p[5],magenta,magenta,magenta); //back
	AddFace(p[7],p[5],p[4],magenta,magenta,magenta);
	
	CreateBuffers();
}


MeshCube::~MeshCube(void)
{
}
