#version 430

// UNIFORM
layout(location=0) uniform vec2 window_size;
layout(location=1) uniform vec4 color;
layout(location=2) uniform float draw_depth;
layout(location=4) uniform vec2 camera_pos;
layout(location=5) uniform float camera_zoom;

// IN
layout(location=0) in vec2 pos_attr;

void main()
{
    mat4 translate_to_origin = mat4(
         1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         -camera_pos.x, -camera_pos.y, 0.0, 1.0
    );
    mat4 translate_from_origin = mat4(
         1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         camera_pos.x, camera_pos.y, 0.0, 1.0
    );
    mat4 scale = mat4(
        camera_zoom, 0.0, 0.0, 0.0,
        0.0, camera_zoom, 0.0, 0.0,
        0.0, 0.0,         1.0, 0.0,
        0.0, 0.0,         0.0, 1.0
    );
    mat4 clipspace_transform = mat4(
        2.0/window_size.x, 0.0, 0.0, -0.0,
        0.0, -2.0/window_size.y, 0.0,  0.0,
        0.0, 0.0,               1.0,  0.0,
        -1.0, 1.0,               0.0,  1.0
    );

    vec4 pos = vec4(pos_attr.x, pos_attr.y, draw_depth, 1.0);
    gl_Position = clipspace_transform * scale * translate_to_origin * pos;
    //gl_Position = clipspace_transform * pos;
    //gl_Position = vec4( 2.0/window_size.x*(pos_attr.x)-1, -2.0/window_size.y*(pos_attr.y)+1, draw_depth, 1.0);
    // gl_Position = vec4( 2.0/window_size.x*(window_size.x/2.0 + pos_attr.x - camera_pos.x)-1,
    //                    -2.0/window_size.y*(window_size.y/2.0 + pos_attr.y - camera_pos.y)+1,
    //                    draw_depth,
    //                    1.0);
}