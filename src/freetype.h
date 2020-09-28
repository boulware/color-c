#ifndef FONT_H
#define FONT_H

#include "ft2build.h"
#include FT_FREETYPE_H

Font
LoadFontFromFile(const char *filename);

#endif