#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include "vec.h"
#include "strings.h"

namespace c
{
	const int max_text_render_length = 1024;
}

float LineSize(u32 pixel_size);

void RenderUtf32String(Utf32String text, Vec2f origin, u32 size);
void _RenderUtf32Char(u32 utf32_char, Vec2f *pen, u32 size, Color color, FT_Face face);
Vec2f SizeUtf8Line(u32 pixel_size, const char *string, ...);
Vec2f DrawText(u32 size, Vec2f origin, const char *string, ...);
void RenderS32AsString(u32 value, Vec2f origin, u32 size);

#endif