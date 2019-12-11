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

void main() {
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

	//Material info
	vec3 albedo = vec3(0.00, 0.89, 0.70);
	float metalic = 0.2f;
	float roughness = 0.0f;
	vec3 baseReflect = vec3(1.0f, 0.0f, 0.0f);
	baseReflect = mix(baseReflect, albedo, metalic);


	vec3 eyeToPos = normalize(Eye - f_position);
	
	vec3 N = normalize(normal);
	float eyeDotNorm = max(dot(eyeToPos, N), 0.0f);

	vec3 result = vec3(0);
	//Compute the BDRF for each light source
	vec3 L0 = vec3(0.0f);
	for(int i = 0; i < 1; i ++){ 
		//Info needed about each light source
		vec3 lightDir = normalize(lightPos[0] - f_position);	
		vec3 halfwayVec = normalize(lightDir + N);
		float attenuation = 1/ (length(lightPos[0] - f_position) * length(lightPos[0] - f_position));
		attenuation = normalize(attenuation);
		vec3 radience = attenuation * lightColor[0].xyz;
		float lightDotNorm = max(dot(lightDir, N), 0.0f);


		float ND = normalDistrib(N, halfwayVec, roughness) * schlickFinal(N, eyeToPos, lightDir, roughness);
		vec3 F = fersnel(halfwayVec, eyeToPos, baseReflect);
		vec3 numerator = ND * F;
		float denominator = 4 * eyeDotNorm * lightDotNorm;


		vec3 reflectFactor = F;
		vec3 refractFactor = vec3(1.0f) - reflectFactor;
		refractFactor *= 1.0f - metalic; //metalic only affects reflected colours. Remove metalic from the refractFactor factor
		vec3 specular = (numerator / denominator);
		L0 += (refractFactor * albedo / PI + specular) * radience * lightDotNorm;
	}

	//result = bdrf(eyeToPos, lightDir, halfwayVec, N, 0.0, baseColor , radience);
	result = L0;
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