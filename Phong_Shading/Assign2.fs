/*
 *  Simple fragment sharder for Lab 2
 */

#version 330 core
uniform sampler2D tex;
uniform vec3 lightPos[1];
uniform vec4 lightColor[1];
uniform int intensity[1];
uniform vec3 Eye;
//uniform vec4 base;
in vec3 normal;
in vec3 f_position;
in vec4 sPosition;
in vec2 texCoords;

layout (location = 0 ) out vec4 norm_color;
layout (location = 1) out vec4 bright_color;
//Phong shading
const float PI = 3.14159265f; //https://www.gamedev.net/forums/topic/529611-pi-in-hlsl/

//https://learnopengl.com/PBR/Theory
float normalDistrib(vec3 N, vec3 halfwayVec, float roughness){
	float numerator = roughness * roughness;
	float denominator =  dot(N,halfwayVec) * dot(N,halfwayVec) * (numerator-1);
	denominator += 1;
	denominator = denominator * denominator;
	denominator = PI * denominator;
	return (numerator / denominator);
}

float schlick(vec3 N, vec3 eyeDir, float roughness){
	float k = (roughness+1) * (roughness+1);
	k = k / 8;
	float numerator = max(dot(N, eyeDir), 0.0f);
	float demoniator = numerator * (1- k) + k;
	return numerator / demoniator;
}

float schlickFinal(vec3 N, vec3 eyeDir, vec3 lightDir, float roughness){
	return schlick(N, eyeDir, roughness) * schlick(N, lightDir, roughness);
}

vec3 fersnel(vec3 halfwayVec, vec3 eyeDir, vec3 baseReflect){
	vec3 temp = (vec3(1.0f)- baseReflect) * pow(1 - max(dot(halfwayVec, eyeDir), 0.0f), 5.0);
	return baseReflect + temp;
}

vec3 cookTorrance(vec3 eyeDir, vec3 lightDir, vec3 halfwayVec, vec3 N, float roughness){
	vec3 baseReflect = vec3(1.00, 0.71, 0.29);
	vec3 numerator = normalDistrib(N, halfwayVec, roughness) * schlickFinal(N, eyeDir, lightDir, roughness) *
					  fersnel(halfwayVec, eyeDir, baseReflect);

	float denominator = 4 * max(dot(eyeDir, N), 0.0f) * max(dot(lightDir, N), 0.0f);
	return numerator / denominator;
	//return denominator;
}

float calcSpec(){
	return 0.5f;
}
vec3 bdrf(vec3 eyeDir, vec3 lightDir, vec3 halfwayVec, vec3 N, float roughness, vec3 baseColor){
	float reflectFactor = calcSpec();
	float refractFactor = 1.0f - reflectFactor;

	vec3 lambert = baseColor / PI;

	return refractFactor * lambert + reflectFactor * cookTorrance(eyeDir, lightDir, halfwayVec, N, roughness);

}


vec3 calcPhysical(vec3 eyeDir, vec3 lightDir, vec3 halfwayVec, vec3 N, vec3 roughness, vec3 baseColor){
	int steps =  100;
	vec3 result = vec3(0);
	float delta = 1.0f / 100;
	for(int i = 0; i < steps; i ++){
		//result += bdrf(eyeDir, lightDir, halfwayVec, N, roughness, baseColor);
	}

	return result;

}

vec4 calcPhong(vec3 eyeToPos, vec4 curLightColor, vec3 curLightPos){
	vec4 base = texture(tex, texCoords);
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
	vec3 lightDir = normalize(lightPos[0] - f_position);
	vec4 result = vec4(0);
	for(int i = 0; i < 1; i ++){
		result += intensity[i] * calcPhong(eyeToPos, lightColor[i], lightPos[i]);
	}
	
	vec3 N = normalize(normal);
	
	vec3 halfwayVec = normalize(lightDir + N);
	result = vec4(bdrf(eyeToPos, lightDir, halfwayVec, N, 0.5, vec3(1.00, 0.71, 0.29)), 1.0f);
	norm_color = result;
	norm_color.a = 1.0;

	bright_color = vec4(0.0f);
	if(result.x > 1.3f || result.y > 1.3f || result.z > 1.3f){
		bright_color = result;
		
	}
	
	bright_color.a = 1.0f;
	
}