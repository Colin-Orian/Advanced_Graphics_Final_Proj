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
//Distribution for the BDRF
float normalDistrib(vec3 N, vec3 halfwayVec, float roughness){
	float temp = roughness * roughness;//Disney and Epic Games found squring the roughness is better
	float numerator = temp * temp; 
	float denominator =  max(dot(N,halfwayVec), 0.0f) * max(dot(N,halfwayVec), 0.0f) * (numerator-1);
	denominator += 1;
	denominator = denominator * denominator;
	denominator = PI * denominator;
	return (numerator / denominator);
}

float schlick(vec3 N, vec3 eyeDir, float roughness){
	float r = (roughness +1) * (roughness +1);
	float k = r * r; //Disney and Epic Games found squring the roughness is better
	k = k / 8;
	float numerator = max(dot(N, eyeDir), 0.0f);
	float demoniator = numerator * (1- k) + k;
	return numerator / demoniator;
}
//Geometry for the BDRF
float schlickFinal(vec3 N, vec3 eyeDir, vec3 lightDir, float roughness){
	return schlick(N, eyeDir, roughness) * schlick(N, lightDir, roughness);
}
//Fersnel for the BDRF
vec3 fersnel(vec3 halfwayVec, vec3 eyeDir, vec3 baseReflect){
	vec3 temp = (vec3(1.0f)- baseReflect) * pow(1 - max(dot(halfwayVec, eyeDir), 0.0f), 5.0);
	return baseReflect + temp;
}

vec3 bdrf(vec3 eyeDir, vec3 lightDir, vec3 halfwayVec, vec3 N, float roughness, vec3 baseColor, vec3 radience){
	float metalic = 0.2f;
	vec3 lambert = baseColor / PI;
	vec3 baseReflect = vec3(1.0f, 0.0f, 0.0f); //mix the albedo with the metalic
	baseReflect = mix(baseReflect, vec3(0.0f, 1.0f, 0.0f), metalic);


	//Cook torrance
	float ND = normalDistrib(N, halfwayVec, roughness) * schlickFinal(N, eyeDir, lightDir, roughness);
	vec3 F = fersnel(halfwayVec, eyeDir, baseReflect);
	vec3 numerator = ND * F;
	float denominator = 4 * max(dot(eyeDir, N), 0.0f) * max(dot(lightDir, N), 0.0f);
	denominator = max(denominator, 0.0001);

	vec3 reflectFactor = F;
	vec3 refractFactor = vec3(1.0f) - reflectFactor;
	refractFactor *= 1.0f - metalic; //metalic only affects reflected colours. Remove metalic from the refractFactor factor
	vec3 specular = (numerator / denominator);
	vec3 completedBdrf = refractFactor * lambert + F *  specular;

	vec3 L0 = (refractFactor * baseColor / PI + specular) * radience * max(dot(N, lightDir), 0.0f);
	

	return L0;
	
}


void main() {
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 eyeToPos = normalize(Eye - f_position);
	vec3 lightDir = normalize(lightPos[0] - f_position);
	vec3 result = vec3(0);
	vec3 N = normalize(normal);
	vec3 halfwayVec = normalize(lightDir + N);

	float attenuation = 1/ (length(lightPos[0] - f_position) * length(lightPos[0] - f_position));
	attenuation = normalize(attenuation);
	vec3 radience = attenuation * lightColor[0].xyz * max(dot(lightDir, N), 0.0f);
	
	result = bdrf(eyeToPos, lightDir, halfwayVec, N, 0.0, vec3(0.00, 0.89, 0.70), radience);
	
	vec3 ambient = vec3(0.03) * vec3(0.00, 0.89, 0.70) * 0.5f;	
	result = result / (result + vec3(1.0f));
	result = pow(result, vec3(1.0f/2.2));
	
	norm_color = vec4(result, 1.0f);
	norm_color.a = 1.0;

	bright_color = vec4(0.0f);
	if(result.x > 1.3f || result.y > 1.3f || result.z > 1.3f){
		bright_color = vec4(result,1.0f);
	}
	
	bright_color.a = 1.0f;
	
}