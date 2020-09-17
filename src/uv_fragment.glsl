#version 430

// UNIFORM
layout(location=1) uniform sampler2D tex;

// IN
in vec2 texcoords;

// OUT
layout(location=0) out vec4 frag_color;

void main()
{
	vec4 tex_color = texture(tex, texcoords);
	if(tex_color.a < 0.1)
		discard;

	frag_color = texture(tex, texcoords);
}