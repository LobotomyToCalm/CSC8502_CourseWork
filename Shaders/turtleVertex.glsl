#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
//uniform mat4 shadowMatrix;

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec4 tangent;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints[128];
uniform vec3 lightPos;

out Vertex{
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	//vec4 shadowProj;
}OUT;

void main(void) {
	vec4 localPos = vec4(position, 1.0f);
	vec4 skelPos = vec4(0, 0, 0, 0);

	for (int i = 0; i < 4; ++i) {
		int jointIndex = jointIndices[i];
		float jointWeight = jointWeights[i];

		skelPos += joints[jointIndex] * localPos * jointWeight;
	}

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wNormal, wTangent) * tangent.w;

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(skelPos.xyz, 1.0);
	OUT.worldPos = (modelMatrix*vec4(skelPos.xyz,1.0)).xyz;
	OUT.texCoord = texCoord;

	vec4 worldPos = vec4(OUT.worldPos,1.0);
	vec3 viewDir = normalize(lightPos - worldPos.xyz);
	vec4 pushVal = vec4(OUT.normal, 0) * dot(viewDir, OUT.normal);
	//OUT.shadowProj = shadowMatrix * (worldPos + pushVal);
}