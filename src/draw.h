#ifndef DRAW_H
#define DRAW_H

#include "color.h"
#include "imgui.h"

void ActivateColorShader(Color color);
void ActivateUvShader(Color color);

void DrawUnfilledRect(Rect rect, Color color);
void DrawUnfilledRect(Vec2f pos, Vec2f size, Color color);

void DrawFilledRect(Rect rect, Color color);
void DrawFilledRect(Vec2f pos, Vec2f size, Color color);

void DrawLine(Vec2f start, Vec2f end, Color color);

ButtonResponse DrawButton(ButtonLayout layout, Rect rect, String label);
ButtonResponse DrawButton(ButtonLayout layout, Rect rect, const char *c_string);

void SetDrawDepth(float depth);

void SetCameraPos(Vec2f camera_pos);
void MoveCamera(Vec2f move);
void SetCameraZoom(float z);
void ZoomCamera(float dz);

#endif
