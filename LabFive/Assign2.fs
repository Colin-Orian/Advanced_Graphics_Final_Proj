/*
 *  Simple fragment sharder for Lab 2
 */

#version 330 core

uniform vec3 light;
uniform vec3 Eye;
uniform samplerCube tex;
in vec3 normal;
in vec3 f_position;
in vec4 sPosition;
in vec2 texCoords;

void main() {
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


	//Schlick's Approximation
	float refractIndexAir = 1.0f; 
	float refractIndexMat = 4.0;
	float f0 = (refractIndexMat - 1) / (refractIndexMat + 1);
	f0 = f0 * f0;

	float thetaSchl = dot(L, normal) / (length(L) * length(normal));
	float schlick = f0 + (1 - f0) * pow(1 - thetaSchl,5);

	vec3 reflectCoords = reflect(-f_position, normal);
	vec3 refractCoords  = refract(-f_position, normal, refractIndexMat);


	
	vec3 tc = mix(refractCoords, reflectCoords, schlick);


	//Part two
	//tc = normal;

	vec3 tempNorm = vec3(normal.x, normal.z, normal.y);
	vec3 U = cross(tempNorm, normal);
	vec3 W = cross(U, normal);

	//Part Three
	float radius = 0.2;
	float step = 1.0;

	
	vec3 result = vec3(0.0);
	//Go around the circle
	vec3 V;
	for(float theta = 0.0; theta < 360.0; theta += step){
		float rad = radians(theta);
		vec3 V = radius * cos(rad) * U + radius * sin(rad) * W;
		result += V;
	}

	//tc = normalize(result);

	base = texture(tex, tc);
	gl_FragColor = (0.5 * base + 0.6 * diffuse * base + 0.6 *specular * white);
	//gl_FragColor = vec4(thetaSchl);
	gl_FragColor.a = 1.0;
}