#include "cuboid.h"

/**
* Cuboid constructor, takes size in 3 axes and creates cuboid at origin
* OLD CODE using direct buffer creation
*/
/*Cuboid::Cuboid(float SX, float SY, float SZ) {
	float hSX=SX/2, hSY=SY/2, hSZ=SZ/2; //half sizes

	//create an indexed 3d cuboid
	int num_vertices = 24; //6 faces, 4 vertices per face

	float vertices[] = {
		//front face
		 1, -1,  1,
		 1,  1,  1,
		-1,  1,  1,
		-1, -1,  1,
		//top face
		 1,  1,  1,
		 1,  1, -1,
		-1,  1, -1,
		-1,  1,  1,
		//right face
		 1, -1, -1,
		 1,  1, -1,
		 1,  1,  1,
		 1, -1,  1,
		//back face
		-1, -1, -1,
		-1,  1, -1,
		 1,  1, -1,
		 1, -1, -1,
		//left face
		-1, -1,  1,
		-1,  1,  1,
		-1,  1, -1,
		-1, -1, -1,
		//bottom face
		 1, -1, -1,
		 1, -1,  1,
		-1, -1,  1,
		-1, -1, -1
	};
	//rescale vertices
	for (int i=0; i<num_vertices*3; i+=3) {
		vertices[i]*=hSX; vertices[i+1]*=hSY; vertices[i+2]*=hSZ;
	}
	
	GLuint indices[] = {
		 0, 1, 2, //front
		 0, 2, 3,
		 4, 5, 6, //top
		 4, 6, 7,
		 8, 9,10, //right
		 8,10,11,
		12,13,14, //back
		12,14,15,
		16,17,18, //left
		16,18,19,
		20,21,22, //bottom
		20,22,23
	};

	//one colour per vertex (i.e. sizeof(vertices)==sizeof(colours)
	float colours[] = {
		1.0, 0.0, 0.0, //front
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0, //top
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0, //right
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 0.0, //back
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		0.0, 1.0, 1.0, //left
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, //bottom
		1.0, 0.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 0.0, 1.0
	};

	//int debug_size = sizeof(indices);
	//cout<<"Size="<<debug_size<<endl;
	NumElements = sizeof(indices); //they're bytes, so size=count - need to set this method property


	glGenVertexArrays(1, &vaoID); // Create our Vertex Array Object

	glBindVertexArray(vaoID); // Bind our Vertex Array Object so we can use it
	
	glGenBuffers(1, &vboID); // Generate our Vertex Buffer Object
	
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/ *num_vertices * 3 * sizeof(GLfloat)* /, vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
	glVertexAttribPointer((GLuint)Shader::v_inPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer (note enable later)

	//bind index array
	glGenBuffers(1, &iboID); //generate 1 buffer object name, returned in the array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID); //bind index buffer object - this links the name to the binding point of the specified buffer type
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(GLuint), &indices[0], GL_STATIC_DRAW); //note: actually indices.size*sizeof(unsigned int) (=12*4?)

	//bind colour array
	glGenBuffers(1, &vcboID); //generate colour buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vcboID); //bind vertex colour buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), &colours[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)Shader::v_inColor, 3, GL_FLOAT, GL_FALSE, 0, NULL); // (note enable later)


	glEnableVertexAttribArray(Shader::v_inPosition); //Enable our vertex buffer in our Vertex Array Object
	glEnableVertexAttribArray(Shader::v_inColor); //Enable our colour buffer in our Vertex Array Object
	
	glBindVertexArray(0); // Disable our Vertex Buffer Object

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //disable vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER,0); //and the other one

}
*/

/// <summary>
/// Cuboid constructor, takes size in 3 axes and creates cuboid at origin.
/// New cuboid constructor code based on a mesh object.
/// <summary>
/// <param name="SX">Size of cuboid in X direction</param>
/// <param name="SY">Size of cuboid in Y direction</param>
/// <param name="SZ">Size of cuboid in Z direction</param>
Cuboid::Cuboid(double SX, double SY, double SZ) {
	float hSX=(float)SX/2, hSY=(float)SY/2, hSZ=(float)SZ/2; //half sizes

	//create a unit cube using the mesh helpers
	glm::vec3 p[] = {
		//front face
		glm::vec3( hSX, -hSY,  hSZ), //0
		glm::vec3( hSX,  hSY,  hSZ), //1
		glm::vec3(-hSX,  hSY,  hSZ), //2
		glm::vec3(-hSX, -hSY,  hSZ), //3
		//back face
		glm::vec3( hSX, -hSY, -hSZ), //4
		glm::vec3( hSX,  hSY, -hSZ), //5
		glm::vec3(-hSX,  hSY, -hSZ), //6
		glm::vec3(-hSX, -hSY, -hSZ)  //7
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
	AddFace(p[4],p[5],p[1],blue,blue,blue); //right
	AddFace(p[4],p[1],p[0],blue,blue,blue);
	AddFace(p[1],p[5],p[6],green,green,green); //top
	AddFace(p[1],p[6],p[2],green,green,green);
	AddFace(p[3],p[2],p[6],cyan,cyan,cyan); //left
	AddFace(p[3],p[6],p[7],cyan,cyan,cyan);
	AddFace(p[4],p[0],p[3],magenta,magenta,magenta); //bottom
	AddFace(p[4],p[3],p[7],magenta,magenta,magenta);
	AddFace(p[7],p[6],p[5],yellow,yellow,yellow); //back
	AddFace(p[7],p[5],p[4],yellow,yellow,yellow);

	CreateBuffers();
}

/// <summary>
/// Destructor
/// TODO: make it virtual and free the buffers in the Mesh base class?
/// </summary>
Cuboid::~Cuboid() {
}
