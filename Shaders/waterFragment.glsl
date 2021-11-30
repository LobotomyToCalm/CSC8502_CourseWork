#version 330 core

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;


in Vertex {
    flat vec4 colour;
    vec3 normal;
    vec3 worldPos;
}IN;

out vec4 fragColour;

void main(void){
    vec3 incident = normalize(lightPos - IN.worldPos);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);
    float distance = length(lightPos - IN.worldPos);
    //float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);
    float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 0.7);
	specFactor = pow(specFactor, 16.0);
    float lambert = max(dot(incident, IN.normal), 0.0f);
    vec3 surface = IN.colour.rgb*lightColour.rgb;

    fragColour.rgb = surface * lambert; //* attenuation;
    fragColour.rgb += (lightColour.rgb * specFactor)  * 0.8;
    fragColour.rgb += surface * 0.8f;

    fragColour.a = IN.colour.a;
}