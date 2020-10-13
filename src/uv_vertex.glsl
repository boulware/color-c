#version 430

// UNIFORM
layout(location=0) uniform vec2 camera_view;
layout(location=1) uniform sampler2D tex;
layout(location=2) uniform vec4 blend_color;
layout(location=3) uniform float draw_depth;
layout(location=4) uniform vec2 camera_pos;
layout(location=5) uniform float camera_zoom;

// IN
layout(location=0) in vec2 pos_attr;
layout(location=1) in vec2 texcoords_attr;

// OUT
out vec2 texcoords;

void main()
{
    // mat4 translate_to_origin = mat4(
    //      1.0, 0.0, 0.0, 0.0,
    //      0.0, 1.0, 0.0, 0.0,
    //      0.0, 0.0, 1.0, 0.0,
    //      -camera_pos.x, -camera_pos.y, 0.0, 1.0
    // );
    // mat4 translate_from_origin = mat4(
    //      1.0, 0.0, 0.0, 0.0,
    //      0.0, 1.0, 0.0, 0.0,
    //      0.0, 0.0, 1.0, 0.0,
    //      camera_pos.x, camera_pos.y, 0.0, 1.0
    // );
    // mat4 scale = mat4(
    //     camera_zoom, 0.0, 0.0, 0.0,
    //     0.0, camera_zoom, 0.0, 0.0,
    //     0.0, 0.0,         1.0, 0.0,
    //     0.0, 0.0,         0.0, 1.0
    // );
    // mat4 clipspace_transform = mat4(
    //     2.0/window_size.x, 0.0,  0.0, 0.0,
    //     0.0, -2.0/window_size.y, 0.0, 0.0,
    //     0.0, 0.0,                1.0, 0.0,
    //     -1.0, 1.0,               0.0, 1.0
    // );
    mat4 camera_transform = mat4(
         1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         -camera_pos.x, -camera_pos.y, 0.0, 1.0
    );
    mat4 clipspace_transform = mat4(
        2.0/camera_view.x, 0.0,  0.0, 0.0,
        0.0, -2.0/camera_view.y, 0.0, 0.0,
        0.0, 0.0,                1.0, 0.0,
        0.0, 0.0,                0.0, 1.0
    );
    vec4 pos = vec4(pos_attr.x, pos_attr.y, draw_depth, 1.0);
    gl_Position = clipspace_transform * camera_transform * pos;
    // vec4 pos2 = clipspace_transform * scale * translate_to_origin * pos;
    // gl_Position = vec4(pos2.x, pos2.y, draw_depth, 1.0);

	// gl_Position = vec4( 2.0/window_size.x*(pos_attr.x)-1,// - camera_pos.x)-1,
 //                       -2.0/window_size.y*(pos_attr.y)+1,// - camera_pos.y)+1,
 //                       draw_depth,
 //                       1.0);
	texcoords = texcoords_attr;
}