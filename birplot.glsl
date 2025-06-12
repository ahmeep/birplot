/*
This file is just going to be pasted into fragment shader
It is seperated for simplicity.

If you want to see what you can use from the shader just look
at assets/shaders/plot.frag.

`EvaluationResult evaluate(vec2 point)`, `vec2 evaluate_gradient(vec2 point)`,
`vec3 background()` and `const float thickness` have to be defined as they are
used in shader.
*/

// this is a special definition. birplot will render this much instances.
// this is optional so if you only want to plot 1 function you can omit this.
#define BIRPLOT_INSTANCE_COUNT 4

#define M_PI 3.1415926535897932384626433832795

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

float userFunction(vec2 point) {
    float x = point.x;
    float y = point.y;

    switch (instanceid) {
        case 0:
        default:
        return y - 0.5 * smoothstep(0.0, 0.7, sin(x + time));
        case 1:
        return y * y + x * x - 0.81;
        case 2:
        return y - 0.2 * sin(10.0 * x + time);
        case 3:
        return y - fbm(x * 1 + time) + 0.1 * sin(x * 10 + time) + 1.0;
    }
}

vec2 numericalGradient(vec2 point) {
    float h = 0.001;
    float dx = (userFunction(point + vec2(h, 0.0)) - userFunction(point - vec2(h, 0.0))) / (2.0 * h);
    float dy = (userFunction(point + vec2(0.0, h)) - userFunction(point - vec2(0.0, h))) / (2.0 * h);
    return vec2(dx, dy);
}

vec3 PLOT_COLORS[4] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.79, 0.02, 0.63));

EvaluationResult evaluate(vec2 point) {
    return EvaluationResult(userFunction(point), PLOT_COLORS[instanceid]);
}

vec2 evaluate_gradient(vec2 point) {
    // If you want to be precise you can define it according to userFunction
    // but it seems the numerical method works fine.
    return numericalGradient(point);
}

const float thickness = 0.03;

vec3 background() {
    return vec3(0.0);
}
