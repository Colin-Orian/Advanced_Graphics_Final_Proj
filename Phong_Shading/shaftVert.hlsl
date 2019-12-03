#version 330 core
uniform mat4  view;
uniform mat4 projection;
uniform mat4 transMat;
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTex;

void main() {
	gl_Position = projection * view * transMat * vPosition;
}
