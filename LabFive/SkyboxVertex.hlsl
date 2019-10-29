
#version 330 core
uniform mat4 modelView;
uniform mat4 projection;
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTex;

out vec3 normal;
out vec3 f_position;

void main() {
	vec2 temp = vTex;
	vec3 temp3 = vNormal;
	f_position = vPosition.xyz;
	gl_Position = projection * modelView * vPosition;
	
	normal = temp3;


}