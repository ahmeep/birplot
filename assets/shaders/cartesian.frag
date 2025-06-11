#version 460 core

in vec2 pos;
out vec4 fragColor;

uniform float time;
uniform int width;
uniform int height;

vec4 getColor(float f) {
    float thickness = 0.005;
    float alpha = 1.0 - smoothstep(0, thickness, f);

    vec3 bg = vec3(0.0);
    vec3 fg = vec3(1.0);
    return vec4(mix(bg, fg, alpha), alpha);
}

void main() {
    // axes are just functions y = 0 and x = 0
    float fx = abs(pos.x);
    float fy = abs(pos.y);

    vec4 xColor = getColor(fx);
    vec4 yColor = getColor(fy);
    fragColor = xColor + yColor;
}
