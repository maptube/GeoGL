
#include "sierpinskipyramid.h"

/// <summary>
/// Constructor
/// </summary>
/// <param name="Size">Pyramid size where height equals base side length (45 degree)</param>
/// <param name="Depth">Recursion depth</param>
SierpinskiPyramid::SierpinskiPyramid(float Size, unsigned int Depth)
{
	PyramidCount=0;
	glm::vec3 Pc(0,Size/2,0); //centre of starting pyramid
	MakePyramids(Size,Depth,Pc);
	CreateBuffers();
}

/// <summary>
/// Destructor
/// </summary>
SierpinskiPyramid::~SierpinskiPyramid(void)
{
}

/// <summary>
/// Make Geometry - recursively
/// </summary>
/// <param name="Size"></param>
/// <param name="Depth">Recursion depth</param>
/// <param name="Pc">Centre point of this pyramid</param>
void SierpinskiPyramid::MakePyramids(float Size,unsigned int Depth,glm::vec3 Pc)
{
	if (Depth==0) {
		//std::cout<<"PYRAMID "<<Size<<" "<<Depth<<" "<<Pc.x<<","<<Pc.y<<","<<Pc.z<<std::endl;
		//Guard Case. This is the maximum depth of recursion, so create geometry here.
		++PyramidCount;
		float L = Size/2.0f; //half side length
		glm::vec3 p[] = {
			Pc + glm::vec3(  0,  L,  0 ), //0 - top
			Pc + glm::vec3( -L, -L,  L ), //1
			Pc + glm::vec3( -L, -L, -L ), //2
			Pc + glm::vec3(  L, -L, -L ), //3
			Pc + glm::vec3(  L, -L,  L ), //4
		};
		//glm::vec3 red(1.0,0,0),green(0,1,0),blue(0,0,1),magenta(1,0,1),white(1,1,1);
		glm::vec3 red(1.0,0,0),green(0.8,0,0),blue(0.8,0,0),magenta(0.8,0,0),white(0.1,0,0);
		//add faces
		AddFace(p[0],p[2],p[1],red,red,red);
		AddFace(p[0],p[3],p[2],green,green,green);
		AddFace(p[0],p[4],p[3],blue,blue,blue);
		AddFace(p[0],p[1],p[4],magenta,magenta,magenta);
		//base
		AddFace(p[1],p[2],p[3],white,white,white);
		AddFace(p[1],p[3],p[4],white,white,white);
	}
	else {
		//Not yet at minimum depth required, so split into five smaller pyramids, four making the base and one resting on top
		float L=Size/2.0f, HL=L/2.0f; //L is length of side at this depth, HL is half L
		//centres of five new pyramids relative to Pc (which encloses them)
		glm::vec3 p[] = {
			Pc + glm::vec3(   0,  HL,   0), //top
			Pc + glm::vec3( -HL, -HL,  HL), //front left
			Pc + glm::vec3( -HL, -HL, -HL), //back left
			Pc + glm::vec3(  HL, -HL, -HL), //back right
			Pc + glm::vec3(  HL, -HL,  HL) //front right
		};
		for (int i=0; i<5; i++)
			MakePyramids(L,Depth-1,p[i]);

	}
}
