/*
 *  Simple vertex shader for lab 4
 */

#version 330 core

uniform mat4  view;
uniform mat4 projection;
uniform int isQuad;
uniform mat4 shadowMatrix;
uniform mat4 transMat;
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTex;

out vec3 normal;
out vec2 texCoords;
out vec3 f_position;
out vec4 sPosition;

void main() {
	
	vec4 temp = transMat * vPosition;
	f_position = temp.xyz;
	gl_Position = projection * view * transMat * vPosition;
	vec3 sphereNormal = (view * vec4(vPosition.xyz,1.0)).xyz;
	normal = vPosition.xyz;
	normal = vNormal;

}