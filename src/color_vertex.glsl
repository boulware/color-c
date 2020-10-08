#version 430

// UNIFORM
layout(location=0) uniform vec2 window_size;
layout(location=1) uniform vec4 color;
layout(location=2) uniform float draw_depth;
layout(location=4) uniform vec2 camera_pos;

// IN
layout(location=0) in vec2 pos_attr;

void main()
{
    gl_Position = vec4( 2.0/window_size.x*(window_size.x/2.0 + pos_attr.x - camera_pos.x)-1,
                       -2.0/window_size.y*(window_size.y/2.0 + pos_attr.y - camera_pos.y)+1,
                       draw_depth,
                       1.0);
}