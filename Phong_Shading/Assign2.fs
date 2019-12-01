/*
 *  Simple fragment sharder for Lab 2
 */

#version 330 core
uniform vec3 lightPos[1];
uniform vec4 lightColor[1];
uniform int intensity[1];
uniform int isEmiter;
uniform vec3 Eye;
uniform vec4 base;
in vec3 normal;
in vec3 f_position;
in vec4 sPosition;
in vec2 texCoords;

layout (location = 0 ) out vec4 norm_color;
layout (location = 1) out vec4 bright_color;
layout (location = 2) out vec4 shaft_color;
//Phong shading
vec4 calcPhong(vec3 eyeToPos, vec4 curLightColor, vec3 curLightPos){

	vec3 L = normalize(curLightPos - f_position);
	vec3 R = normalize(reflect(-L,normal));
	float eyeAngle = dot(eyeToPos, R);
	vec3 N;
	float specular;
	N = normalize(normal);
	float diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;

	} else {
		specular = pow(max(0.0, eyeAngle), 16);
	}

	vec4 result = (0.5 * base + 0.6 * diffuse * base + 0.6 *specular * curLightColor);
	return result;
}

void main() {
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 eyeToPos = normalize(Eye - f_position);
	vec4 result = vec4(0);
	for(int i = 0; i < 1; i ++){
		result += intensity[i] * calcPhong(eyeToPos, lightColor[i], lightPos[i]);
	}
	
	norm_color = result;
	norm_color.a = 1.0;

	bright_color = vec4(0.0f);
	if(result.x > 1.3f || result.y > 1.3f || result.z > 1.3f){
		bright_color = result;
		
	}
	bright_color.a = 1.0f;

	shaft_color = isEmiter * lightColor[0] + (1- isEmiter) * vec4(0.0f);
	shaft_color.a = 1.0f;
}