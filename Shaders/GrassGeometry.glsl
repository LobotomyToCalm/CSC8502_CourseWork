#version 330 core
layout(triangles) in;
layout(triangle_strip,max_vertices =24)out;

const float		PI = 3.14159265358979323846;

uniform float time;
uniform sampler2D grassNoiseTex;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
}IN[];

out Vertex{
	vec4 colour;
	vec3 normal;
	vec3 worldPos;
	vec2 texCoord;
}OUT;

void emitVertex(vec4 point,vec2 texCoord,vec2 worldTex){
		gl_Position = point;
		OUT.colour =texture(grassNoiseTex,worldTex);
		OUT.normal = IN[0].normal;
		OUT.worldPos = gl_Position.xyz;
		OUT.texCoord = texCoord;
		EmitVertex();
}

float DegToRad(const float rad)	{
	return rad * PI / 180.0f;
};

mat3 Rotation(float degrees)	 {
	mat3 m;

	vec3 axis = normalize(vec3(1.0,0.0,0.0));

	float c = cos(DegToRad(degrees));
	float s = sin(DegToRad(degrees));

	m=mat3( 
	(axis.x * axis.x) * (1.0f - c) + c,
	(axis.y * axis.x) * (1.0f - c) + (axis.z * s),
	(axis.z * axis.x) * (1.0f - c) - (axis.y * s), 
	

	(axis.x * axis.y) * (1.0f - c) - (axis.z * s),
	(axis.y * axis.y) * (1.0f - c) + c,
	(axis.z * axis.y) * (1.0f - c) + (axis.x * s), 
	

	(axis.x * axis.z) * (1.0f - c) + (axis.y * s),
	(axis.y * axis.z) * (1.0f - c) - (axis.x * s),
	(axis.z * axis.z) * (1.0f - c) + c
	
	);

	return m;
}

void main(){
	
	
	// Generate grass ( 3 grasses for 1triangle )
	if(IN[0].worldPos.y>68.0&&IN[0].worldPos.y<110.0){

		vec4 center = (gl_in[0].gl_Position+gl_in[1].gl_Position+gl_in[2].gl_Position)/3;
		vec4 ptNoise = texture(grassNoiseTex,IN[0].texCoord);
		center = center+vec4(3.0*ptNoise.r,5.0,3.0*ptNoise.g,0.0);
		vec3 grassHeight = vec3(0.0,43.0,0.0);
		vec3 grassWidth = 2*normalize(vec3(1.0,0.0,1.0));
		mat3 rotateMat = Rotation(ptNoise.r*90);
		grassWidth = (rotateMat*grassWidth).xyz;


		vec3 waveDir = normalize(cross(grassHeight,grassWidth));
		
		// left bottom
		vec4 leftBottom= center+vec4(grassWidth,1.0);
		// right bottom
		vec4 rightBottom = center-vec4(grassWidth,1.0);
		// left middle 2
		vec4 leftMiddle = center+vec4(grassWidth,1.0)/2+vec4(grassHeight,1.0)/2;
		// right middle 2
		vec4 rightMiddle = center-vec4(grassWidth,1.0)/2+vec4(grassHeight,1.0)/2;
		// top
		vec4 top = center+vec4(grassHeight,1.0)+vec4(sin(time*ptNoise.r)*2*waveDir,1.0);

		emitVertex(leftBottom,vec2(0.0,0.75),IN[0].texCoord);
		emitVertex(rightBottom,vec2(1.0,0.75),IN[0].texCoord);
		emitVertex(leftMiddle,vec2(0.0,0.25),IN[0].texCoord);
		emitVertex(rightMiddle,vec2(1.0,0.25),IN[0].texCoord);
		emitVertex(top,vec2(0.5,0.0),IN[0].texCoord);
	
	}

	EndPrimitive();
}