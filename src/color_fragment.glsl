#version 430

// UNIFORM
layout(location=0) uniform vec2 camera_view; // Size of camera rect in world space
layout(location=1) uniform vec4 color;
layout(location=2) uniform float draw_depth;
layout(location=4) uniform vec2 camera_pos;
layout(location=5) uniform float camera_zoom;

// OUT
layout(location=0) out vec4 frag_color;

void main()
{
	frag_color = color.a*color;
}