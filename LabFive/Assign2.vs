/*
 *  Simple vertex shader for lab 4
 */

#version 330 core

uniform mat4 modelView;
uniform mat4 projection;
uniform int isQuad;
uniform mat4 shadowMatrix;
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTex;

out vec3 normal;
out vec2 texCoords;
out vec3 f_position;
out vec4 sPosition;

void main() {
	vec2 temp = vTex;
	vec3 temp3 = vNormal;
	f_position = vPosition.xyz;
	gl_Position = projection * modelView * vPosition;
	vec3 sphereNormal = (modelView * vec4(vPosition.xyz,1.0)).xyz;
	normal = vPosition.xyz;

}