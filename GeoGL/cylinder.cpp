#include "cylinder.h"

/**
* Cylinder constructor, takes length, radius and number of segments and creates cylinder at origin looking along Z axis (TODO: is this the best orientation?)
* Circular faces are both in the XY plane.
* TODO: need to base this on a mesh to simplify
*/
/*Cylinder::Cylinder(float Length, float Radius, unsigned int NumSegments) {	
	//TODO: check NumSegments*2<256 as the indexes are bytes
	float hLen = Length/2; //half length

	unsigned int num_vertices = NumSegments*2*2; //top circle, bottom circle and side faces - check num_vertices<256
	float* vertices = new float[num_vertices*3];

	//create front circular face and back circular face
	float ang = 2*glm::pi<float>()/(float)NumSegments;
	for (unsigned int i=0; i<NumSegments; i++) {
		float x=Radius*sin(i*ang);
		float y=Radius*cos(i*ang);
		//front (top?) face (circular)
		vertices[i*3]=-x;
		vertices[i*3+1]=y;
		vertices[i*3+2]=hLen;
		//back (bottom?) face (circular)
		vertices[(i+NumSegments)*3]=-x;
		vertices[(i+NumSegments)*3+1]=y;
		vertices[(i+NumSegments)*3+2]=-hLen;
	}
	//the side vertices are just a copy of the top and bottom (although normals will be different)
	for (unsigned int i=0,j=NumSegments*2*3; i<NumSegments*2*3; i++,j++) {
		vertices[j]=vertices[i];
	}

	//colours
	float* colours = new float[num_vertices*3];
	//front
	for (unsigned int i=0; i<NumSegments*3; i+=3) {
		colours[i]=1.0; colours[i+1]=0.0; colours[i+2]=0.0f; //red
	}
	//back
	for (unsigned int i=NumSegments*3; i<NumSegments*3*2; i+=3) {
		colours[i]=0.0; colours[i+1]=1.0; colours[i+2]=0.0f; //green
	}
	//sides
	for (unsigned int i=NumSegments*3*2; i<NumSegments*3*4; i+=3) {
		colours[i]=0.0; colours[i+1]=0.0; colours[i+2]=1.0f; //blue
	}

	//create indexes
	unsigned int NumFaces = (NumSegments-2)*2 + NumSegments*2; //circular face has NumSegs-2, then NumSegments planar faces along the length
	GLuint* indices=new GLuint[NumFaces*3];

	//front face - triangle fan in indices
	unsigned int v=0;
	for (unsigned int i=1; i<NumSegments-1; i++) {
		indices[v++]=0;
		indices[v++]=i;
		indices[v++]=i+1;
	}

	//back face - should be the opposite way around to the first
	for (unsigned int i=1; i<NumSegments-1; i++) {
		indices[v++]=NumSegments+i+1;
		indices[v++]=NumSegments+i;
		indices[v++]=NumSegments;
	}

	//sides
	//NumSegments is the first point on the back face
	int v1=0, v2=NumSegments, v3=v2+1, v4=1; //these are the four vertices which we move around the shape
	int base_v = NumSegments*2; //all faces are formed from the second set of vertices after the front and back circles
	for (unsigned int i=0; i<NumSegments; i++) {
		//form two triangles out of v1 v2 v3 v4
		//tri 1
		indices[v++]=v1+base_v;
		indices[v++]=v2+base_v;
		indices[v++]=v3+base_v;
		//tri 2
		indices[v++]=v1+base_v;
		indices[v++]=v3+base_v;
		indices[v++]=v4+base_v;
		//now move v1234 around
		v1=(v1+1)%NumSegments; //front
		v4=(v4+1)%NumSegments; //front
		v2=(v2-NumSegments+1)%NumSegments+NumSegments; //back (not exactly elegant!)
		v3=(v3-NumSegments+1)%NumSegments+NumSegments; //back
	}

	//create the buffers here
	NumElements = NumFaces*3; //needed for object3d, otherwise it doesn't know how many elements to render

	glGenVertexArrays(1, &vaoID); // Create our Vertex Array Object

	glBindVertexArray(vaoID); // Bind our Vertex Array Object so we can use it
	
	glGenBuffers(1, &vboID); // Generate our Vertex Buffer Object
	
	//be careful with the sizes here, as the buffers need real memory sizes, not numbers of points or indices
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, num_vertices*3 * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
	glVertexAttribPointer((GLuint)Shader::v_inPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer

	//bind index array
	glGenBuffers(1, &iboID); //generate 1 buffer object name, returned in the array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID); //bind index buffer object - this links the name to the binding point of the specified buffer type
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumFaces*3 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW); //note: actually indices.size*sizeof(unsigned int) (=12*4?)

	//bind colour array
	glGenBuffers(1, &vcboID); //generate colour buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vcboID); //bind vertex colour buffer
	glBufferData(GL_ARRAY_BUFFER, num_vertices*3 * sizeof(GLfloat), &colours[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)Shader::v_inColor, 3, GL_FLOAT, GL_FALSE, 0, NULL); // (note enable later)

	glEnableVertexAttribArray(Shader::v_inPosition); // Enable our vertex buffer in our Vertex Array Object
	glEnableVertexAttribArray(Shader::v_inColor); // Enable our colour buffer in our Vertex Array Object

	glBindVertexArray(0); // Disable our Vertex Buffer Object

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //disable vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0); //and the other one

	delete vertices;
	delete colours;
	delete indices;
}*/


Cylinder::Cylinder(float Length, float Radius, unsigned int NumSegments) {
	//cylinder constructor based on a mesh object
	
	float hLen = Length/2; //half length

	float ang = 2*glm::pi<float>()/(float)NumSegments;
	glm::vec3 P0,P1,P2,P3; //P3 only used for sides
	glm::vec3 red(1.0,0.0,0.0), green(0.0,1.0,0.0), blue(0.0,0.0,1.0);
	
	//create front circular face and back circular face
	
	//front (top?) face (circular) - it's a trifan, but indexed
	P0.x=0; P0.y=Radius; P0.z=hLen;
	for (unsigned int i=0; i<=NumSegments; i++) {
		P2.x=-Radius*sin(i*ang);
		P2.y=Radius*cos(i*ang);
		P2.z=hLen;
		if (i>0) AddFace(P0,P1,P2,red,red,red);
		P1=P2; //move P1P2 points around circle
	}
	//back (bottom?) face (circular)
	P0.x=0; P0.y=Radius; P0.z=-hLen;
	for (unsigned int i=0; i<=NumSegments; i++) {
		P2.x=-Radius*sin(i*ang);
		P2.y=Radius*cos(i*ang);
		P2.z=-hLen;
		if (i>0) AddFace(P2,P1,P0,green,green,green);
		P1=P2; //move points around circle
	}

	//create sides as two triangles
	for (unsigned int i=0; i<=NumSegments; i++) {
		P0.x=Radius*sin(i*ang);		P0.y=Radius*cos(i*ang);			P0.z=hLen;
		P1.x=Radius*sin((i+1)*ang);	P1.y=Radius*cos((i+1)*ang);		P1.z=hLen;
		P2.x=Radius*sin((i+1)*ang);	P2.y=Radius*cos((i+1)*ang);		P2.z=-hLen;
		P3.x=Radius*sin(i*ang);		P3.y=Radius*cos(i*ang);			P3.z=-hLen;
		AddFace(P0,P1,P2,blue,blue,blue);
		AddFace(P0,P2,P3,blue,blue,blue);
	}

	CreateBuffers();
}