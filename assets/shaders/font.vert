#version 460 core
layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TexPos;
layout(location = 2) in vec4 a_Color;

out vec2 pos;
out vec2 texPos;
out vec4 color;

uniform mat4 projection;

void main()
{
    pos = a_Pos;
    texPos = a_TexPos;
    color = a_Color;

    gl_Position = projection * vec4(pos, 0.0, 1.0);
}
