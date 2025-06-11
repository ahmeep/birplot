#version 460 core

in vec2 pos;
out vec4 fragColor;

uniform float time;
uniform int width;
uniform int height;

vec4 plot(float f, vec3 bg, vec3 fg, float thickness) {
    float alpha = 1.0 - smoothstep(0, thickness, f);

    return vec4(mix(bg, fg, alpha), alpha);
}

void main() {
    // axes are just functions y = 0 and x = 0
    vec3 axebg = vec3(0.0);
    vec3 axefg = vec3(0.6);
    float axeThickness = 0.005;
    float fx = abs(pos.x);
    float fy = abs(pos.y);
    vec4 xColor = plot(fx, axebg, axefg, axeThickness);
    vec4 yColor = plot(fy, axebg, axefg, axeThickness);

    vec3 gridbg = axebg;
    vec3 gridfg = vec3(0.2);
    vec4 gridColorSum = vec4(0.0);
    float gridThickness = 0.003;
    for (float x = -1.0; x < 1.0; x += 0.2) {
	gridColorSum += plot(abs(pos.x - x), gridbg, gridfg, gridThickness);
    }
    for (float y = -1.0; y < 1.0; y += 0.2) {
	gridColorSum += plot(abs(pos.y - y), gridbg, gridfg, gridThickness);
    }

    fragColor = xColor + yColor + gridColorSum;
}
