#ifndef DRAW_H
#define DRAW_H

#include "color.h"

void ActivateColorShader(Color color);
void ActivateUvShader(GLuint texture);

void DrawUnfilledRect(Rect rect, Color color);
void DrawFilledRect(Rect rect, Color color);
void DrawLine(Vec2f start, Vec2f end, Color color);

#endif
