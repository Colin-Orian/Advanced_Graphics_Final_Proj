#version 330 core
uniform samplerCube tex;
in vec3 normal;
in vec3 f_position;
void main() {
	vec4 colour = texture(tex, f_position);
	gl_FragColor = colour;

}