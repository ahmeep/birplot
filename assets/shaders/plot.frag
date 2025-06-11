#version 460 core

in vec2 pos;
out vec4 fragColor;

uniform float time;

uniform int width;
uniform int height;

float hash(float n) {
    return fract(sin(n) * 1e4);
}
float hash(vec2 p) {
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

float noise(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}

#define NUM_OCTAVES 5

float fbm(float x) {
    float v = 0.0;
    float a = 0.5;
    float shift = float(100);
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(x);
        x = x * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

#define M_PI 3.1415926535897932384626433832795

float func(vec2 p) {
    float x = p.x;
    float y = p.y;
    //return x * x * x * x + 2 * x * x * y * y + y * y * y * y - x * x * x + 3 * x * y * y;
    return y - fbm(x * 1 + time) + 0.1 * sin(x * 10 + time) + 0.5;
}

vec2 calc_grad(vec2 p) {
    float h = 0.001;
    float dx = (func(p + vec2(h, 0.0)) - func(p - vec2(h, 0.0))) / (2.0 * h);
    float dy = (func(p + vec2(0.0, h)) - func(p - vec2(0.0, h))) / (2.0 * h);
    return vec2(dx, dy);
}

vec2 grad_func(vec2 p) {
    // you can define it precisely but not necessary.
    return calc_grad(p);
}

void main() {
    const float thickness = 0.03;
    float f = abs(func(pos)) / length(grad_func(pos));

    float alpha = 1.0 - smoothstep(0, thickness / 3, f) - smoothstep(2 * thickness / 3, thickness, f);
    vec3 bg = vec3(0.0);
    vec3 fg = vec3(202.0 / 255.0, 6.0 / 255.0, 161.0 / 255.0);
    vec3 color = mix(bg, fg, alpha);
    fragColor = vec4(color, alpha);
}
