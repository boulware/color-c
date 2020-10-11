#include "font_loading.h"

#include "bitmap.h"

Font
LoadFontData(FT_Face face, int size)
{
    Font font;
    float scale = (float)size/(float)(face->units_per_EM >> 6);
    font.ascender = scale*(float)(face->ascender >> 6);
    font.base_size = size;
    font.height = scale*(float)(face->height >> 6);

    // Metric tables
    font.advance_x = (int*)malloc(256*sizeof(int));
    font.bitmap_left = (int*)malloc(256*sizeof(int));
    font.bitmap_top = (int*)malloc(256*sizeof(int));

    // Texture tables
    font.gl_texture = (GLuint*)malloc(256*sizeof(int));
    font.texture_size = (Vec2i*)malloc(256*sizeof(Vec2i));
    for(int i=0; i<256; i++)
    {
        font.gl_texture[i] = GenerateAndBindTexture();
    }

    FT_Set_Pixel_Sizes(face, 0, size);
    for(int i=0; i<256; i++)
    {
        FT_Load_Char(face, i, FT_LOAD_RENDER);
        font.advance_x[i] = face->glyph->advance.x >> 6;
        font.bitmap_left[i] = face->glyph->bitmap_left;
        font.bitmap_top[i] = face->glyph->bitmap_top;
        font.texture_size[i] = {(s32)face->glyph->bitmap.width, (s32)face->glyph->bitmap.rows};


        Bitmap char_bmp = AllocBitmap(face->glyph->bitmap.width,
                                      face->glyph->bitmap.rows);
        auto &alpha_map = face->glyph->bitmap;

        u32 pixel_count = char_bmp.width*char_bmp.height;
        for(int i=0; i<pixel_count; i++)
        {
            char_bmp.pixels[i].r = u8(alpha_map.buffer[i]);
            char_bmp.pixels[i].g = u8(alpha_map.buffer[i]);
            char_bmp.pixels[i].b = u8(alpha_map.buffer[i]);
            char_bmp.pixels[i].a = u8(alpha_map.buffer[i]);
        }

        gl->BindTexture(GL_TEXTURE_2D, font.gl_texture[i]);
        gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, char_bmp.width, char_bmp.height,
                       0, GL_BGRA, GL_UNSIGNED_BYTE, char_bmp.pixels);

        DeallocBitmap(&char_bmp);
    }

    font.is_init = true;
    return font;
}