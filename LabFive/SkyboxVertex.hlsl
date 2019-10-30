
#version 330 core
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transMat;
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTex;

out vec3 normal;
out vec3 f_position;

void main() {
	vec2 temp = vTex;
	vec3 temp3 = vNormal;
	f_position = vPosition.xyz;
	gl_Position = projection * view * transMat * vPosition;
	
	normal = temp3;


}