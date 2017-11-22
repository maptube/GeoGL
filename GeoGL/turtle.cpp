#include "turtle.h"

/// <summary>
/// Create a unit sized turtle at the origin. The turtle fits inside a unit box as follows:
/// (missing centroid at + as I can't draw the shallow angle)
///
///  ______c______
/// |     /|\     |
/// |    / | \    |
/// |   /  |  \   |
/// |  /   +   \  |
/// | /         \ |
/// |/___________\|
/// a            b
/// The base is moved up into a chevron by connecting to the centroid point 1/3(a+b+c) which is placed at the origin so the turtle rotates about this point
/// Vertex a=(-0.5,-0.5), b=(0.5,-0.5), c=(0,0.5)
/// So centroid=(0,1/6)
/// </summary>
/// <param name="Size"></param>
Turtle::Turtle(float TurtleSize)
{
	init(TurtleSize,_VertexFormat);
}

Turtle::Turtle(float TurtleSize, gengine::VertexFormat VF)
{
	init(TurtleSize,VF);
}

/// <summary>
/// Destructor
/// </summary>
Turtle::~Turtle(void)
{
}

void Turtle::init(float TurtleSize, gengine::VertexFormat VF)
{
	_VertexFormat = VF;
	Size = TurtleSize;
	Colour=glm::vec3(0,0,0); //black turtle

	//create a unit cube using the mesh helpers
	glm::vec3 p[] = {
		glm::vec3( -0.5, -0.1,   0.5), //0, a
		glm::vec3(  0.5, -0.1,   0.5), //1, b
		glm::vec3(  0.0, -0.1,  -0.5), //2, c

		glm::vec3(  0.0, -0.1,  1.0/6.0), //3, + centroid bottom
		glm::vec3(  0.0,  0.1,  1.0/6.0)  //4, + centroid top
	};

	//normals
	glm::vec3 n[] = {
		glm::vec3( -0.707, 0, 0.707),
		glm::vec3( 0.707, 0, 0.707),
		glm::vec3( 0, 0, -1.0),

		glm::vec3( 0, -1.0, 0),
		glm::vec3( 0, 1.0, 0)
	};

	//add faces - don't actually need all these colours, but useful for testing
	glm::vec3 red(1.0,0.0,0.0);
	glm::vec3 green(0.0,1.0,0.0);
	glm::vec3 blue(0.0,0.0,1.0);
	glm::vec3 yellow(1.0,1.0,0.0);
	glm::vec3 cyan(0.0,1.0,1.0);
	glm::vec3 magenta(1.0,0.0,1.0);

	//top
	AddFace(p[4],p[2],p[0],green,green,green,n[4],n[2],n[0]);
	AddFace(p[4],p[1],p[2],green,green,green,n[4],n[1],n[2]);
	//base
	AddFace(p[3],p[0],p[2],red,red,red,n[3],n[0],n[2]);
	AddFace(p[3],p[2],p[1],red,red,red,n[3],n[2],n[1]);
	//back
	AddFace(p[0],p[3],p[4],blue,blue,blue,n[0],n[3],n[4]);
	AddFace(p[1],p[4],p[3],blue,blue,blue,n[1],n[4],n[3]);

	//multiply vertices up by Size
	ScaleVertices(Size,Size,Size);

	CreateBuffers();
}
