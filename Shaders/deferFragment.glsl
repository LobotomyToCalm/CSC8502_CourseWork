#version 330 core

in Vertex{
	vec4 colour;
	vec3 normal;
}IN;

out vec4 fragColour[2];

void main(void) {

	fragColour[0] = IN.colour;
	fragColour[1] = vec4(IN.normal.xyz, 1.0);
}