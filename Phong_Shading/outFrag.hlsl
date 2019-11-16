#version 330 core
uniform sampler2D tex;
in vec2 fTexCoords;

void main() {
	vec3 color = texture(tex, fTexCoords).xyz;
	gl_FragColor = vec4(color, 1.0f);
}