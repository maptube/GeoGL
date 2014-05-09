//4.7 diffuse lighting vertex shader
in vec4 position;
out vec3 worldPosition;
out vec3 positionToLight;

uniform mat4 og_modelViewPerspectiveMatrix;
uniform vec3 og_cameraLightPosition;

void main()
{
	glPosition = og_modelViewPrespectiveMatrix * position;
	worldPosition = position.xyz;
	positionToLight = og_cameraLightPosition - worldPosition;
}