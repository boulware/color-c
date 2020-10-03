#ifndef FREETYPE_WRAPPER_H
#define FREETYPE_WRAPPER_H

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_MODULE_H

bool InitFreetype(FT_Library *lib);
FT_Face LoadFontFromFile(const char *filename, FT_Library lib);

#endif