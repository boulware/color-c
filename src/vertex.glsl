#version 430

// UNIFORM
layout(location=0) uniform vec2 window_size;
layout(location=1) uniform vec4 color;

// IN
layout(location=0) in vec2 pos_attr;

// OUT
out vec2 texcoords;

void main()
{
	gl_Position = vec4(2.0/window_size.x*pos_attr.x-1, -2.0/window_size.y*pos_attr.y+1, 0.f, 1.f);
}