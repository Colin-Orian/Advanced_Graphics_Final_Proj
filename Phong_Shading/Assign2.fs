/*
 *  Simple fragment sharder for Lab 2
 */

#version 330 core

uniform vec3 light;
uniform vec3 Eye;
in vec3 normal;
in vec3 f_position;
in vec4 sPosition;
in vec2 texCoords;

void main() {
	float PI = 3.14;
	vec4 base = vec4(1.0, 0.0, 0.0, 1.0);

	
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 N;
	vec3 L = normalize(light - f_position);
	vec3 R = normalize(reflect(-L,normal));
	vec3 eyeToPos = normalize(Eye - f_position);
	float eyeAngle = dot(eyeToPos, R);
	float specular;
	N = normalize(normal);
	float diffuse = dot(N,L);
	bool isZero = false;
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
		isZero = true;
	} else {
		specular = pow(max(0.0, eyeAngle), 16);
	}
	
	gl_FragColor = (0.5 * base + 0.6 * diffuse * base + 0.6 *specular * white);
	
	gl_FragColor.a = 1.0;
}