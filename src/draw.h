#ifndef DRAW_H
#define DRAW_H

#include "color.h"
#include "imgui.h"
#include "camera.h"

struct Texture
{
    u32 id;
    int width, height;
};

struct Framebuffer
{
    u32 id;
    int width, height;
    Texture texture;
    u32 depth_stencil_rbo;
};

struct DirectedLineLayout
{
    int segment_count = 100;

    Color line_color = c::white;
    Color arrow_color = c::dk_red;
    float arrow_size = 35.f;
    float arrow_angle = 25.f;
};

void ActivateColorShader(Color color);
void ActivateUvShader(Color color);

void DrawUnfilledRect(Rect rect, Color color);
void DrawUnfilledRect(Vec2f pos, Vec2f size, Color color);

void DrawFilledRect(Rect rect, Color color);
void DrawFilledRect(Vec2f pos, Vec2f size, Color color);

void DrawLine(Vec2f start, Vec2f end, Color color);
void DrawDirectedLine(DirectedLineLayout layout,
                      Vec2f start_pos, Vec2f end_pos,
                      Vec2f start_vel, Vec2f end_vel,
                      String label);

ButtonResponse DrawButton(ButtonLayout layout, Rect rect, String label);
ButtonResponse DrawButton(ButtonLayout layout, Rect rect, const char *c_string);

void SetDrawDepth(float depth);

void SetCameraPos(Camera *camera, Vec2f camera_pos);
void MoveCamera(Camera *camera, Vec2f move);
void SetCameraView(Camera *camera, Vec2f view);
void MoveCameraToWorldRect(Camera *camera, Rect rect);

#endif
