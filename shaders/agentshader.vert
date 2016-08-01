#version 150

/*agent shader, copy of diffuse lighting with normals, but a colour mask and colour is passed in*/

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 u_colour;
uniform vec3 u_colourMask;

in vec3 in_Position;
in vec3 in_Color;
in vec3 in_Normal;

out vec3 passColor;
out vec3 positionToLight;
out vec3 normal;


void main()
{
	//vec3 cameraLightPosition = vec3(-400000,-400000,-4000000);
	vec3 cameraLightPosition = vec3(0,0,4000000);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
	normal = in_Normal; //vec3(projectionMatrix * viewMatrix * modelMatrix * vec4(in_Normal, 1.0));
	vec3 worldPosition = gl_Position.xyz;
	positionToLight = cameraLightPosition - worldPosition;
	/*passColor = in_Color;*/
	/*use the mask to remove colour from the colour buffer and do primitive mixing*/
	/* TODO: isn't this a scalar product?*/
	passColor = vec3(min((in_Color.x*u_colourMask.x) + u_colour.x,1.0), min((in_Color.y*u_colourMask.y) + u_colour.y,1.0), min((in_Color.z*u_colourMask.z) + u_colour.z,1.0));
}
