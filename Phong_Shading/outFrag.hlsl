#version 330 core
uniform sampler2D tex;
uniform int hdr_on;
in vec2 fTexCoords;

//HDR tutorial found here https://learnopengl.com/Advanced-Lighting/HDR
void main() {
	float exposure = 1.0f;
	const float gamma = 2.2;
	vec3 hdrColor = texture(tex, fTexCoords).xyz;
	vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
	mapped = pow(mapped, vec3(1.0f / gamma));
	gl_FragColor =  hdr_on * vec4(mapped, 1.0f) + (1- hdr_on) * vec4(hdrColor, 1.0f);
}