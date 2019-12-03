uniform int isEmiter;
layout(location = 0) out vec4 shaft_color;
void main() {
	shaft_color = isEmiter * vec4(1.0f, 1.0f, 1.0f, 1.0f) + (1 - isEmiter) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}