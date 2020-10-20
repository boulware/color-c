#version 430

// UNIFORM
layout(location=0) uniform sampler2D screen;

// IN
layout(location=0) in vec2 texcoords;

// OUT
layout(location=0) out vec4 frag_color;

void main()
{
    frag_color = texture(screen, gl_FragCoord.xy + (100,0));
}