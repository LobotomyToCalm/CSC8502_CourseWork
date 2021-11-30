#version 330 core
layout(triangles) in;
layout(line_strip,max_vertices =6)out;

in Vertex{
	flat vec4 colour;	
	vec3 normal;
	vec3 worldPos;
}IN[];

out Vertex{
	flat vec4 colour;	
	vec3 normal;
	vec3 worldPos;
}OUT;

void main(){

	for (int i=0;i<gl_in.length();++i){
		gl_Position = gl_in[i].gl_Position;

		OUT.colour =IN[i].colour;
		OUT.normal = IN[i].normal;
		OUT.worldPos = IN[i].worldPos;

		EmitVertex();
	}

	EndPrimitive();
}