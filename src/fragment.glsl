#version 430

// UNIFORM
layout(location=0) uniform vec2 window_size;
layout(location=1) uniform vec4 color;

// OUT
layout(location=0) out vec4 frag_color;

void main()
{
	frag_color = color;
}