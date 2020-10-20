#version 430

// UNIFORM
layout(location=0) uniform sampler2D screen_tex;

// IN
layout(location=0) in vec2 pos_attr;
layout(location=0) in vec2 texcoords_attr;

// OUT
out vec2 texcoords;

void main()
{
    gl_Position = pos_attr;
    texcoords = texcoords_attr;
}