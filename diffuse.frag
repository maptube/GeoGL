//4.8 diffuse lighting fragment shader
in vec3 worldPosition;
in vec3 positionToLight;
out vec3 fragmentColor;

void main()
{
	vec3 toLight = normalize(positionToLight);
	vec3 normal = normalize(worldPosition);
	float diffuse = max(dot(toLight,normal),0.0);
	fragmentColor = vec3(diffuse, diffuse, diffuse);
}