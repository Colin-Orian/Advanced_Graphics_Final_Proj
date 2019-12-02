#version 330 core
uniform samplerCube tex;
in vec3 f_position;
void main() {
	vec4 colour = texture(tex, f_position);
	
	colour.r = min(colour.r, 1.0f);
	colour.g = min(colour.g, 1.0f);
	colour.b = min(colour.b, 1.0f);
	gl_FragColor = colour;
	gl_FragColor.a = 1.0f;

}