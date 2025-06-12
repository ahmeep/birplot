#version 460 core

in vec2 inPos;

out vec2 pos;
flat out int instanceid;

void main() {
	pos = inPos;
	instanceid = gl_InstanceID;
	gl_Position = vec4(inPos, 0.0, 1.0);
}
