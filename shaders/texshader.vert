#version 150

/* texture vertex shader for globe */
/* this is just a simple multiplication of position as no lights to consider */

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_Position;

out vec3 worldPosition;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
	//this is the position on the globe i.e. with globe centre at origin and the correct ellipsoid size
	worldPosition = in_Position.xyz; //gl_Position.xyz;
}