#version 430

// UNIFORM
layout(binding=0)          uniform sampler2D in_screen;
layout(binding=1, rgba32f) uniform image2D out_screen;

// IN
layout(local_size_x=1, local_size_y=1) in;

const float pi = 3.1415926535;

int
color_distance(vec4 start, vec4 end)
{
    int distance = 0;
    for(int i=0; i<4; ++i)
    { // for each component
        distance += int(abs(end[i] - start[i]));
    }

    return distance;
}

float x_kernel[] = {
    1, 0, -1,
    2, 0, -2,
    1, 0, -1
};

float y_kernel[] = {
     1,   2,  1,
     0,   0,  0,
    -1,  -2, -1
};

void main()
{
    ivec2 src_coords = ivec2(gl_GlobalInvocationID.xy);
    vec4 src_pixel = texelFetch(in_screen, src_coords, 0);

    float pixel_intensity[9];
    for(int y=0; y<=2; ++y)
    {
        for(int x=0; x<=2; ++x)
        {
            vec4 pixel = texelFetch(in_screen, src_coords + ivec2(x-1,y-1), 0);
            pixel_intensity[x + 3*y] = (pixel.r + pixel.g + pixel.b) / 3.0;
        }
    }

    // vec4 left_pixel  = texelFetch(in_screen, src_coords + ivec2(-1,0), 0);
    // vec4 right_pixel = texelFetch(in_screen, src_coords + ivec2(+1,0), 0);
    // vec4 up_pixel    = texelFetch(in_screen, src_coords + ivec2(0,-1), 0);
    // vec4 down_pixel  = texelFetch(in_screen, src_coords + ivec2(0,+1), 0);
    // //float d_color_x = right_pixel.a - left_pixel.a;
    // int d_color_x = color_distance(left_pixel, right_pixel);
    // int d_color_y = color_distance(up_pixel, down_pixel);
    float d_x = 0.0;
    for(int i=0; i<9; ++i)
    {
        d_x += x_kernel[i] * pixel_intensity[i];
    }

    float d_y = 0.0;
    for(int i=0; i<9; ++i)
    {
        d_y += y_kernel[i] * pixel_intensity[i];
    }

    float D = sqrt(pow(d_x,2) + pow(d_y,2)) / 10.0;

    if(D >= 0.2)
        imageStore(out_screen, src_coords, vec4(1.0, 0.0, 0.0, 1.0));
    else
        imageStore(out_screen, src_coords, src_pixel);
    //imageStore(out_screen, src_coords, vec4(0.0,1.0,0.0,1.0));

    // if()
    // {
    // }
    // else
    // {
    //     imageStore(out_screen, src_coords, src_pixel);
    // }

    // vec4 dst_pixel = 0.2 * src_pixel;
    // for(float a=0; a<2*pi; a += 0.5*pi)
    // {
    //     vec2 uv = vec2(float(src_coords.x + 5.0*cos(a))/1599.0 ,
    //                    float(src_coords.y + 5.0*sin(a))/899.0);
    //     vec4 other_pixel = texture(in_screen, uv);
    //     dst_pixel += 0.1 * other_pixel;
    // }
    //imageStore(out_screen, src_coords, dst_pixel);

}