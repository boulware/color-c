#ifndef DRAW_H
#define DRAW_H

#include "color.h"
#include "imgui.h"
#include "camera.h"

void ActivateColorShader(Color color);
void ActivateUvShader(Color color);

void DrawUnfilledRect(Rect rect, Color color, bool ui=false);
void DrawUnfilledRect(Vec2f pos, Vec2f size, Color color, bool ui=false);

void DrawFilledRect(Rect rect, Color color, bool ui=false);
void DrawFilledRect(Vec2f pos, Vec2f size, Color color, bool ui=false);

void DrawLine(Vec2f start, Vec2f end, Color color);

ButtonResponse DrawButton(ButtonLayout layout, Rect rect, String label);
ButtonResponse DrawButton(ButtonLayout layout, Rect rect, const char *c_string);

void SetDrawDepth(float depth);

void SetCameraPos(Camera *camera, Vec2f camera_pos);
void MoveCamera(Camera *camera, Vec2f move);
void SetCameraView(Camera *camera, Vec2f view);
void MoveCameraToWorldRect(Camera *camera, Rect rect);

#endif
