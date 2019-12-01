/*
 *  Simple vertex shader for lab 4
 */

#version 330 core

uniform mat4  view;
uniform mat4 projection;
uniform mat4 transMat;
in vec4 vPosition;
out vec3 f_position;

void main() {

	vec4 temp = transMat * vPosition;
	f_position = temp.xyz;
	gl_Position = projection * view * transMat * vPosition;

}