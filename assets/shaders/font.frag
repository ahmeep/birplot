#version 460 core

in vec2 pos;
in vec2 texPos;
in vec4 color;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    fragColor = texture(tex, texPos) * color;
}
