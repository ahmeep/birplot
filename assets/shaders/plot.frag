#version 460 core

in vec2 pos;
flat in int instanceid;
out vec4 fragColor;

uniform float time;

uniform int width;
uniform int height;

struct EvaluationResult {
    float value;
    vec3 color;
};

#USERDEFINEDHERE

void main() {
    EvaluationResult res = evaluate(pos);
    vec2 gradient = evaluate_gradient(pos);

    float f = abs(res.value) / length(gradient);
    float alpha = 1.0 - smoothstep(0, thickness / 3, f) - smoothstep(2 * thickness / 3, thickness, f);

    vec3 color = mix(background(), res.color, alpha);
    fragColor = vec4(color, alpha);
}
