#version 150

/*diffuse lighting with normals*/

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

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
	passColor = in_Color;
}
