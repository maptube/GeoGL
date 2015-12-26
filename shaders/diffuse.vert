#version 150

/*4.7 diffuse lighting vertex shader*/

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_Position;
in vec3 in_Color;

out vec3 passColor;
out vec3 worldPosition;
out vec3 positionToLight;


void main()
{
	vec3 cameraLightPosition = vec3(0.0,0.0,1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
	worldPosition = gl_Position.xyz;
	positionToLight = cameraLightPosition - worldPosition;
	passColor = in_Color;
}
