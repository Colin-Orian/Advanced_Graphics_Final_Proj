#version 330 core
uniform sampler2D tex;
uniform sampler2D bloomTex;
uniform sampler2D shaftTex;
uniform int hdr_on;
uniform int bloom_on;
uniform vec3 lightPos;
in vec2 fTexCoords;

//HDR tutorial found here https://learnopengl.com/Advanced-Lighting/HDR
//Bloom tutorial found here https://learnopengl.com/Advanced-Lighting/Bloom
void main() {
	//I used the Gaussian Blur from here https://en.wikipedia.org/wiki/Kernel_(image_processing)
	float gaussKernel[9] = float[](
		1.0f, 2.0f, 1.0f,
		2.0f, 4.0f, 2.0f,
		1.0f, 2.0f, 1.0f);

	float blurFactor = 1.0f / 3.0f;
	//http://www.jhlabs.com/ip/blurring.html
	float radialKernel[9] = float[](
		0, 0, blurFactor,
		0, blurFactor, 0,
		blurFactor, 0, 0);

	float offSet = 1.0f / 300.0f;
	vec2 bloomOffset[9] = vec2[](
		vec2(fTexCoords.x - offSet, fTexCoords.y + offSet), //top left
		vec2(fTexCoords.x, fTexCoords.y + offSet), //top mid
		vec2(fTexCoords.x + offSet, fTexCoords.y), // top right
		vec2(fTexCoords.x - offSet, fTexCoords.y), //mid left
		vec2(fTexCoords.x, fTexCoords.y), //middle
		vec2(fTexCoords.x + offSet, fTexCoords.y), // mid right
		vec2(fTexCoords.x - offSet, fTexCoords.y - offSet),
		vec2(fTexCoords.x, fTexCoords.y - offSet),
		vec2(fTexCoords.x + offSet, fTexCoords.y - offSet));

	vec4 bloomColor = vec4(0.0f);
	for (int i = 0; i < 9; i++) {
		float x = clamp(bloomOffset[i].x, 0.0f, 0.99f); 
		float y = clamp(bloomOffset[i].y, 0.0f, 0.99f);
		bloomColor += (1.0f / 16.0f) * gaussKernel[i] * vec4(texture(bloomTex, vec2(x, y)).xyz, 1.0f);
	}
	bloomColor = bloom_on * bloomColor + (1 - bloom_on) * vec4(0.0f);


	vec4 shaftColor = vec4(0.0f);
	vec2 deltaTexCoord = fTexCoords - lightPos.xy;
	deltaTexCoord *= 1.0f / (100 * 10.0f);
	vec2 currentCoords = fTexCoords;
	shaftColor = texture(shaftTex, currentCoords);
	float illuminationDecay = 1.0f;
	float weight = 1.0f;
	for (int i = 0; i < 100; i++) {
		currentCoords -= deltaTexCoord;
		vec4 data = texture(shaftTex, currentCoords);
		data *= illuminationDecay * weight;
		shaftColor += data;
		illuminationDecay *= 0.70;
	}

	bloomColor += shaftColor * 0.5f;

	float exposure = 1.0f;
	const float gamma = 2.2;
	vec3 hdrColor = texture(tex, fTexCoords).xyz;
	hdrColor += bloomColor.xyz;
	vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
	mapped = pow(mapped, vec3(1.0f / gamma));
	vec4 preBloom = hdr_on * vec4(mapped, 1.0f) + (1- hdr_on) * vec4(hdrColor, 1.0f);

	


	gl_FragColor = preBloom;
	
	gl_FragColor = texture(shaftTex, fTexCoords);
	gl_FragColor.a = 1.0f;
}