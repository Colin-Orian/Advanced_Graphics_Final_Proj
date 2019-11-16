//Used https://learnopengl.com/Advanced-OpenGL/Framebuffers
#version 330 core
in vec2 vPosition;
in vec2 vTex;
out vec2 fTexCoords;
void main() {
	fTexCoords = vTex;
	gl_Position = vec4(vPosition.x, vPosition.y, 0.0f, 1.0f);
}