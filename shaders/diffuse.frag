#version 150

/*4.8 diffuse lighting fragment shader*/
in vec3 worldPosition;
//in vec3 positionToLight;
in vec3 passColor;
out vec4 fragmentColor;

void main()
{
	//vec3 toLight = normalize(positionToLight);
	//hacked light direction to be straight down (0,1,0)
	vec3 toLight = vec3(0,1.0,1.0);
	vec3 normal = normalize(worldPosition);
	float diffuse = max(dot(toLight,normal),0.0);
	fragmentColor = vec4(diffuse*passColor.x, diffuse*passColor.y, diffuse*passColor.z, 1.0);
}
