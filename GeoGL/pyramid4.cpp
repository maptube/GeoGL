
#include "pyramid4.h"


//Pyramid4::Pyramid4(void)
//{
//}

/// <summary>
/// Construct a four sided pyramid with the origin in the centre
/// </summary>
/// <param name="SX">Length of side in X axis</param>
/// <param name="SY">Length of side in Y axis (height)</param>
/// <param name="SZ">Length of side in Z axis</param>
Pyramid4::Pyramid4(double SX, double SY, double SZ)
{
	//four sided pyramid constructor based on a mesh object
	//constructs a pyramid with side lengths in the three axes of SX, SY, SZ

	glm::vec3 P0,P1,P2,P3; //P3 only used for sides
	glm::vec3 red(1.0,0.0,0.0), green(0.0,1.0,0.0), blue(0.0,0.0,1.0), cyan(0.0,1.0,1.0), yellow(1.0,1.0,0.0);
	
	glm::vec3 P[] = {
		glm::vec3( -SX/2, -SY/2,  SZ/2), //base
		glm::vec3( -SX/2, -SY/2, -SZ/2),
		glm::vec3(  SX/2, -SY/2, -SZ/2),
		glm::vec3(  SX/2, -SY/2,  SZ/2),
		glm::vec3(     0,  SY/2,     0) //top point
	};

	AddFace(P[0],P[4],P[1],red,red,red);
	AddFace(P[1],P[4],P[2],green,green,green);
	AddFace(P[2],P[4],P[3],blue,blue,blue);
	AddFace(P[3],P[4],P[0],cyan,cyan,cyan);
	AddFace(P[0],P[1],P[2],yellow,yellow,yellow);
	AddFace(P[0],P[2],P[3],yellow,yellow,yellow);

	CreateBuffers();

}


//Pyramid4::~Pyramid4(void)
//{
//}
