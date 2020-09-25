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

FT_Face
LoadFontFaceFromFile(const char *filename, FT_Library lib)
{
	FT_Face face;
	FT_Error error = FT_New_Face(lib, filename, 0, &face);

	if(error) log("Failed to load font (file: \"%s\")", filename);

	return face;
}