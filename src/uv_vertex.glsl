#version 430

// UNIFORM
layout(location=0) uniform vec2 window_size;
layout(location=1) uniform sampler2D tex;
layout(location=2) uniform vec4 blend_color;
layout(location=3) uniform float draw_depth;

// IN
layout(location=0) in vec2 pos_attr;
layout(location=1) in vec2 texcoords_attr;

// OUT
out vec2 texcoords;

void main()
{
	gl_Position = vec4(2.0/window_size.x*pos_attr.x-1, -2.0/window_size.y*pos_attr.y+1, draw_depth, 1.0);
	texcoords = texcoords_attr;
}