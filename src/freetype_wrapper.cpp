#include "freetype_wrapper.h"

bool
InitFreetype(FT_Library *lib)
{
	bool success = !FT_Init_FreeType(lib);
	if(success) {
		return true;
	}
	else {
		log("Failed to initialize FreeType.");
		return false;
	}
}

Font
LoadFontFromFile(const char *filename, FT_Library lib)
{
	Font loaded_font = {};
	FT_Error error = FT_New_Face(lib, filename, 0, &loaded_font.face);

	if(error) {
		log("Failed to load font Roboto-Regular.ttf");
		loaded_font.is_init = false;
	}
	else {
		loaded_font.is_init = true;
	}

	return loaded_font;
}