#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include "vec.h"
#include "color.h"
#include "strings.h"
#include "freetype_wrapper.h"
#include "align.h"
#include "opengl.h"

struct Font
{
	//FT_Face face;
	bool is_init;
	int base_size;
	int ascender;
	//int units_per_EM;
	int height;

	// Metric tables
	int *advance_x;
	int *bitmap_left;
	int *bitmap_top;

	// Texture table
	GLuint *gl_texture;
	Vec2i *texture_size;
};

namespace text_render
{
	Font default_font;
};

struct TextLayout
{
	Font *font;
	Color color;
	int font_size;
	Align align;
	bool draw_debug;
};

float LineSize(TextLayout layout);

//void _RenderUtf32Char(u32 utf32_char, Vec2f *pen, u32 size, Color color, FT_Face face);
//Vec2f SizeUtf8Line(TextLayout layout, const char *string, ...);
Vec2f DrawText(TextLayout layout, Vec2f origin, const char *string, ...);
void EasyDrawText(const char *string, ...);

#endif