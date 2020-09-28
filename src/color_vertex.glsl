#version 430

// UNIFORM
layout(location=0) uniform vec2 window_size;
layout(location=1) uniform vec4 color;
layout(location=2) uniform float draw_depth;

// IN
layout(location=0) in vec2 pos_attr;

void main()
{
	gl_Position = vec4(2.0/window_size.x*pos_attr.x-1, -2.0/window_size.y*pos_attr.y+1, draw_depth, 1.f);
}