#version 150

/*agent shader, copy of diffuse lighting with normals, but a colour mask and colour is passed in (to vertex shader)*/
in vec3 positionToLight;
in vec3 passColor;
in vec3 normal;
out vec4 fragmentColor;


void main()
{
	vec3 toLight = normalize(positionToLight);
	float diffuse = max(dot(toLight,normal),0.0);
	fragmentColor = vec4(diffuse*passColor.x, diffuse*passColor.y, diffuse*passColor.z, 1.0);
}
