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

ButtonResponse DrawButton(ButtonLayout layout, Rect rect, const char *label, ...);

#endif
