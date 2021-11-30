#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 mapSize;
uniform float time;
uniform sampler2D waterNoiseTex;


in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;


const float waveSize = 3.4;
const float PI = 3.14159265358979323846264338328;
const float sizeScale = 5.0;

out Vertex{
	flat vec4 colour;	
	vec3 normal;
	vec3 worldPos;
} OUT;

vec3 calcNormal(vec3 v1,vec3 v2,vec3 v3){
	vec3 tang = v2-v1;
	vec3 biTan = v3-v1;
	return normalize(cross(tang,biTan));
}

vec3 setDistortion(vec3 v){
	vec2 texCoord = vec2(v.x / (sizeScale*mapSize.x), v.z / (sizeScale*mapSize.z));
	vec4 waveSpeed = texture(waterNoiseTex,texCoord);
	float offset = sin(time*7*waveSpeed.r)*waveSize;

	return vec3(v.x,offset,v.z);
}
void main(void){
	OUT.colour = colour;
	vec4 worldPos = modelMatrix * vec4(position,1);
	
	vec3 curVertex = setDistortion(vec3(position.x,0.0,position.z));
	vec3 corner1 = setDistortion(vec3(tangent.x,0.0,tangent.y));
	vec3 corner2 = setDistortion(vec3(tangent.z,0.0,tangent.w));

	//OUT.normal = normal;
	vec3 afterNormal = calcNormal(curVertex,corner1,corner2);
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 wNormal = normalize(normalMatrix * normalize(afterNormal));
	OUT.normal = wNormal;
	
	worldPos = modelMatrix * vec4(curVertex,1);
	worldPos.y = worldPos.y+mapSize.y*sin(time*2)*0.05-mapSize.y*0.1;
	OUT.worldPos = worldPos.xyz;

	
	gl_Position = (projMatrix * viewMatrix) * worldPos;
}