#version 460 core

in vec2 inPos;

out vec2 pos;

void main() {
	pos = inPos;
	gl_Position = vec4(inPos, 0.0, 1.0);
}
