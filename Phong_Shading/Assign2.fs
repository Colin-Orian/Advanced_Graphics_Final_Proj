/*
 *  Simple fragment sharder for Lab 2
 */

#version 330 core
uniform vec3 lightPos[2];
uniform vec4 lightColor[2];
uniform int intensity[2];
uniform vec3 Eye;
in vec3 normal;
in vec3 f_position;
in vec4 sPosition;
in vec2 texCoords;

//Phong shading
vec4 calcPhong(vec4 base, vec3 eyeToPos, vec4 curLightColor, vec3 curLightPos){

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
	vec4 base = vec4(0.5, 0.0, 0.0, 1.0);
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 eyeToPos = normalize(Eye - f_position);
	vec4 result = vec4(0);
	for(int i = 0; i < 2; i ++){
		result += intensity[i] * calcPhong(base, eyeToPos, lightColor[i], lightPos[i]);
	}
	gl_FragColor = result;
	if(gl_FragColor.r <= 1.0f){
		gl_FragColor = vec4(0.0f);
	}
	gl_FragColor.a = 1.0;
}