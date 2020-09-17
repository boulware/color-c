#include "text_render.h"

#include "opengl.h"
#include "math.h"
#include "draw.h"

void
_RenderUtf32Char(u32 utf32_char, Vec2f *pen, u32 size, Color color, FT_Face face)
{
	float scale = size / (float)face->units_per_EM;

	// Generate character texture
	FT_Load_Char(face, utf32_char, FT_LOAD_RENDER);

	Bitmap char_bmp = AllocBitmap(face->glyph->bitmap.width,
								  face->glyph->bitmap.rows);
	auto &alpha_map = face->glyph->bitmap;

	u32 pixel_count = char_bmp.width*char_bmp.height;
	for(int i=0; i<pixel_count; i++)
	{
		char_bmp.pixels[i].r = u8(alpha_map.buffer[i]*color.r);
		char_bmp.pixels[i].g = u8(alpha_map.buffer[i]*color.g);
		char_bmp.pixels[i].b = u8(alpha_map.buffer[i]*color.b);
		char_bmp.pixels[i].a = u8(alpha_map.buffer[i]);
	}

	gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, char_bmp.width, char_bmp.height,
				   0, GL_BGRA, GL_UNSIGNED_BYTE, char_bmp.pixels);

	// Drawing texture to screen
	Vec2f char_pos = {pen->x + (face->glyph->bitmap_left),
					  pen->y - (face->glyph->bitmap_top) + scale*(face->ascender)};

	GLfloat verts[] = {
		char_pos.x, char_pos.y, 0.f, 0.f,
		char_pos.x+char_bmp.width, char_pos.y, 1.f, 0.f,
		char_pos.x, char_pos.y+char_bmp.height, 0.f, 1.f,
		char_pos.x+char_bmp.width, char_pos.y+char_bmp.height, 1.f, 1.f
	};

	pen->x += (face->glyph->advance.x >> 6);
	//pen->y -= scale*(face->glyph->metrics.horiBearingY >> 6);

	gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	DeallocBitmap(&char_bmp);
}

float
LineSize(u32 pixel_size)
{
	FT_Face face = game->default_font.face;
	FT_Set_Pixel_Sizes(face, 0, pixel_size);
	float scale = pixel_size / (float)face->units_per_EM;
	return scale*(game->default_font.face->height);
}

// void
// RenderUtf8String(const char *string, Vec2f origin, u32 size)
// {
// 	gl->BindVertexArray(game->uv_vao);
// 	gl->BindBuffer(GL_ARRAY_BUFFER, game->uv_vbo);

// 	StringBuffer buffer = CreateStringBuffer(string);
// 	u32 utf32_char;
// 	Vec2f pen = origin;

// 	FT_Face face = game->default_font.face;
// 	FT_Set_Pixel_Sizes(face, 0, size);
// 	float scale = size / (float)face->units_per_EM;

// 	while(NextAsUtf32Char(&buffer, &utf32_char))
// 	{
// 		if(utf32_char == '\n')
// 		{
// 			pen.y += LineSize(size);
// 			pen.x = origin.x;
// 		}
// 		else
// 		{
// 			_RenderUtf32Char(utf32_char, &pen, size, face);
// 		}
// 	}
// }

// Renders characters of *string until a null byte or a newline character is reached,
// or until c::max_text_render_length is reached.
// Returns the size of the rendered line in pixels
// (Note: the size isn't exact to the pixel, but should be guaranteed to be at least larger than the rendered string,
// but usually will only be slightly larger; but may be consistently too small when using things like accented chars,
// because the size is calculated using TTF metrics)

Vec2f
SizeUtf8Line(u32 pixel_size, const char *string, ...)
{
	va_list args;
	va_start(args, string);

	char formatted_string[c::max_text_render_length];
	int formatted_length = vsprintf(formatted_string, string, args);
	if(formatted_length > c::max_text_render_length)
	{
		log("Utf8LineSize() received a string longer than c::max_text_render_length (%d). "
			"The string is still sized, but clipped to the max render length.",
			c::max_text_render_length);

		formatted_string[c::max_text_render_length-1] = '\0'; // vsprintf doesn't null append if the string is too long.
	}

	StringBuffer buffer = CreateStringBuffer(formatted_string);
	u32 utf32_char;
	Vec2f pen = {0.f,0.f};

	FT_Face face = game->default_font.face;
	FT_Set_Pixel_Sizes(face, 0, pixel_size);

	while(NextAsUtf32Char(&buffer, &utf32_char))
	{
		if(utf32_char == '\n')
		{
			break;
		}
		else
		{
			//_RenderUtf32Char(utf32_char, &pen, size, face);
			FT_Load_Char(face, utf32_char, FT_LOAD_DEFAULT);
			pen.x += (face->glyph->advance.x >> 6);
		}
	}


	return Vec2f{pen.x, LineSize(pixel_size)};
}

Vec2f
DrawTextColor(u32 size, Vec2f origin, Color color, const char *string, ...)
{
	ActivateUvShader(game->temp_texture);

	va_list args;
	va_start(args, string);

	char formatted_string[c::max_text_render_length];
	int formatted_length = vsprintf(formatted_string, string, args);
	if(formatted_length > c::max_text_render_length)
	{
		log("DrawText() received a string longer than c::max_text_render_length (%d). "
			"The string is still rendered, but clipped.",
			c::max_text_render_length);

		formatted_string[c::max_text_render_length-1] = '\0'; // vsprintf doesn't null append if the string is too long.
	}

	StringBuffer buffer = CreateStringBuffer(formatted_string);
	u32 utf32_char;
	Vec2f pen = origin;

	FT_Face face = game->default_font.face;
	FT_Set_Pixel_Sizes(face, 0, size);
	float scale = size / (float)face->units_per_EM;

	while(NextAsUtf32Char(&buffer, &utf32_char))
	{
		if(utf32_char == '\n')
		{
			break;
		}
		else
		{
			_RenderUtf32Char(utf32_char, &pen, size, color, face);
		}
	}

	return Vec2f{pen.x-origin.x, LineSize(size)};
}

Vec2f
DrawText(u32 size, Vec2f origin, const char *string, ...)
{
	va_list args;
	va_start(args, string);

	char formatted_string[c::max_text_render_length];
	int formatted_length = vsprintf(formatted_string, string, args);
	if(formatted_length > c::max_text_render_length)
	{
		log("DrawText() received a string longer than c::max_text_render_length (%d). "
			"The string is still rendered, but clipped.",
			c::max_text_render_length);

		formatted_string[c::max_text_render_length-1] = '\0'; // vsprintf doesn't null append if the string is too long.
	}

	return DrawTextColor(size, origin, c::white, formatted_string);
}



// void
// RenderS32AsString(u32 value, Vec2f origin, u32 size)
// {
// 	if(value < 0) return; // Negative not implemented yet

// 	Vec2f pen = origin;
// 	FT_Face face = game->default_font.face;
// 	FT_Set_Pixel_Sizes(face, 0, size);
// 	float scale = size / (float)face->units_per_EM;

// 	u32 temp_value = value;
// 	int digit_count = 1;
// 	while(temp_value > 9)
// 	{
// 		temp_value /= 10;
// 		digit_count += 1;
// 	}

// 	for(int digit=0; digit<digit_count; digit++)
// 	{
// 		u32 factor = m::Pow(10, digit_count-digit-1);
// 		temp_value = value / factor;
// 		value -= temp_value * factor;

// 		u32 utf32_char = DigitToUtf32Char(temp_value);
// 		if(utf32_char == '\n')
// 		{
// 			pen.y += scale*(face->height);
// 			pen.x = origin.x;
// 		}
// 		else
// 		{
// 			_RenderUtf32Char(utf32_char, &pen, size, face);
// 		}
// 	}
// }

// void
// RenderUtf32String(Utf32String text, Vec2f origin, u32 size)
// {
// 	FT_Face face = game->default_font.face;
// 	FT_Set_Pixel_Sizes(face, 0, size);
// 	float scale = size / (float)face->units_per_EM;
// 	Vec2f pen = origin;
// 	gl->BindTexture(GL_TEXTURE_2D, game->temp_texture);

// 	pen.y += scale*(face->bbox.yMax);

// 	for(int i=0; i<text.char_count; i++)
// 	{
// 		if(text.chars[i] == '\n') {
// 			pen.y += scale*(face->height);
// 			pen.x = origin.x;
// 			//log("newline");
// 			continue;
// 		}

// 		// Generate character texture
// 		FT_Load_Char(face, text.chars[i], FT_LOAD_RENDER);
// 		//FT_Render_Glyph(0, FT_RENDER_MODE_NORMAL);

// 		Bitmap char_bmp = AllocBitmap(face->glyph->bitmap.width,
// 									  face->glyph->bitmap.rows);
// 		auto &alpha_map = face->glyph->bitmap;

// 		u32 pixel_count = char_bmp.width*char_bmp.height;
// 		for(int i=0; i<pixel_count; i++)
// 		{
// 			char_bmp.pixels[i].r = alpha_map.buffer[i];
// 			char_bmp.pixels[i].g = alpha_map.buffer[i];
// 			char_bmp.pixels[i].b = alpha_map.buffer[i];
// 			char_bmp.pixels[i].a = alpha_map.buffer[i];
// 		}

// 		gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, char_bmp.width, char_bmp.height,
// 					   0, GL_RGBA, GL_UNSIGNED_BYTE, char_bmp.pixels);
// 		DeallocBitmap(char_bmp);

// 		// Drawing texture to screen

// 		// First char, so let's account for (e.g.) negative horizontal bearing X
// 		//if(i==0) pen.x = -(face->glyph->metrics.horiBearingX >> 6);

// 		Vec2f char_pos = {pen.x + (face->glyph->bitmap_left),
// 						  pen.y - (face->glyph->bitmap_top)};

// 		GLfloat verts[] = {
// 			char_pos.x, char_pos.y, 0.f, 0.f,
// 			char_pos.x+char_bmp.width, char_pos.y, 1.f, 0.f,
// 			char_pos.x, char_pos.y+char_bmp.height, 0.f, 1.f,
// 			char_pos.x+char_bmp.width, char_pos.y+char_bmp.height, 1.f, 1.f
// 		};

// 		pen.x += (face->glyph->advance.x >> 6);

// 		gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
// 		gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// 	}
// }