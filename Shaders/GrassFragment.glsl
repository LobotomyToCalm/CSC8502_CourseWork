#version 330 core

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform sampler2D grassTex;

in Vertex{
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
	vec4 texColour = texture(grassTex,IN.texCoord);
	fragColour.rgba =texColour;
}