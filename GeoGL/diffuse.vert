//4.7 diffuse lighting vertex shader

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_Position;
in vec3 in_Color;

out vec3 worldPosition;
out vec3 positionToLight;

out vec3 pass_Color;

//uniform mat4 modelViewPerspectiveMatrix;
//uniform vec3 cameraLightPosition;

void main()
{
	vec3 cameraLightPosition = vec3(0.0,0.0,1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
	worldPosition = gl_Position.xyz;
	positionToLight = cameraLightPosition - worldPosition;
	pass_Color = in_Color;
}