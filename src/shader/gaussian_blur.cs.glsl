#version 430

// UNIFORM
layout(binding=0)          uniform sampler2D in_screen;
layout(binding=1, rgba32f) uniform image2D out_screen;
layout(location=2) uniform float blur;

// IN
layout(local_size_x=1, local_size_y=1) in;

const float pi = 3.1415926535;

void main()
{
    ivec2 src_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dst_coords = ivec2(src_coords.x, src_coords.y);

    vec4 src_pixel = texelFetch(in_screen, src_coords, 0);

    //imageStore(out_screen, ivec2(0,0), vec4(0.2, 0.4, 0.6, 0.8));
    //imageStore(out_screen, src_coords, vec4(1.0,1.0,1.0,1.0));
    //memoryBarrier();
    #if 0
    if(src_coords.x == 0 || src_coords.y == 0 || src_coords.x == 1599 || src_coords.y == 899)
    {
        imageStore(out_screen, src_coords, vec4(0.0, 1.0, 0.0, 1.0));
    }
    else imageStore(out_screen, src_coords, vec4(0.0, 0.0, 0.0, 1.0));
    #else

    vec2 uv_origin = vec2(float(src_coords.x)/1599.0 ,
                          float(src_coords.y)/899.0);

    vec4 dst_pixel = 0.1 * src_pixel;
    float increment = 0.05*pi;
    float max = 2*pi;
    float step_count = increment/max;
    float blend_amount = 0.9 / step_count / 1000;
    for(float a=0; a<max; a += increment)
    {
        vec2 uv = vec2(float(src_coords.x + blur*cos(a))/1599.0 ,
                       float(src_coords.y + blur*sin(a))/899.0);
        vec4 other_pixel = texture(in_screen, uv);
        dst_pixel += blend_amount * other_pixel;
    }
    imageStore(out_screen, src_coords, dst_pixel);
    //imageStore(out_screen, dst_coords, src_pixel);
    #endif

    //frag_color = texture(screen, gl_FragCoord.xy + (100,0));
}

/*
#version 430
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;
The first layout qualifier defines the size of the local work group - note that this is handled behind the scenes - we don't need to adjust our shader if we make the local work group larger. We decided to start with a size of 1 pixel - 1x1. It is also possible to put 1d or 3d dimensions here if your work group has a different structure.

The second layout qualifier defines the internal format of the image that we set up. Note that we use an image2D uniform, rather than a texture sampler. This gives us the ability to write to any pixel that we want.

void main() {
  // base pixel colour for image
  vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
  // get index in global work group i.e x,y position
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

  //
  // interesting stuff happens here later
  //

  // output to a specific pixel in the image
  imageStore(img_output, pixel_coords, pixel);
}
*/