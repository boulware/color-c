#ifndef FREETYPE_WRAPPER_H
#define FREETYPE_WRAPPER_H

#include "ft2build.h"
#include FT_FREETYPE_H

struct Font
{
	bool is_init;
	FT_Face face;
};

bool
InitFreetype(FT_Library *lib);

Font
LoadFontFromFile(const char *filename, FT_Library lib);

#endif