#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 colour;

out Vertex{
	flat vec4 colour;	
	vec3 normal;
	vec3 worldPos;
} OUT;

void main(void) {
	OUT.colour = colour;
	OUT.normal = vec3(0.0,0.0,0.0);
	OUT.worldPos = vec3(0.0,0.0,0.0);
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec4 worldPos = (modelMatrix * vec4(position, 1));

	gl_Position = (projMatrix * viewMatrix) * worldPos;
}