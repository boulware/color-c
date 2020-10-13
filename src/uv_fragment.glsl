#version 430

// UNIFORM
layout(location=0) uniform vec2 camera_view;
layout(location=1) uniform sampler2D tex;
layout(location=2) uniform vec4 blend_color;
layout(location=3) uniform float draw_depth;
layout(location=4) uniform vec2 camera_pos;
layout(location=5) uniform float camera_zoom;

// IN
in vec2 texcoords;

// OUT
layout(location=0) out vec4 frag_color;

void main()
{
	vec4 tex_color = texture(tex, texcoords);
	frag_color = blend_color.a*tex_color*blend_color;
}