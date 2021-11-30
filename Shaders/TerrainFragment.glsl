#version 330 core

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex{
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
	vec3 incident = normalize(lightPos - IN.worldPos);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float lambert = max(dot(incident, IN.normal), 0.5f);
	float distance = length(lightPos - IN.worldPos);
	float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
	specFactor = pow(specFactor, 10.0);
	vec3 surface = (IN.colour.rgb * lightColour.rgb);
	fragColour.rgb = surface * lambert * attenuation;
	fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.63;
	fragColour.rgb += surface * 0.7f;
	fragColour.a = IN.colour.a;
}