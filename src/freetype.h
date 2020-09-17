#ifndef FONT_H
#define FONT_H

#include "ft2build.h"
#include FT_FREETYPE_H

struct Font
{
	bool is_init;
	FT_Face face;
};

Font
LoadFontFromFile(const char *filename);

#endif