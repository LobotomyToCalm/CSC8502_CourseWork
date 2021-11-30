#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
	vec4 colour;
	vec3 normal;
} OUT;

void main(void) {
	OUT.colour = colour;
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	OUT.normal = wNormal;
	
	vec4 worldPos = (modelMatrix * vec4(position, 1));

	gl_Position = (projMatrix * viewMatrix) * worldPos;
}