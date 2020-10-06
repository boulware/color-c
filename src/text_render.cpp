#include "text_render.h"

#include "opengl.h"
#include "math.h"
//#include "draw.h"
#include "memory.h"
#include "utf32string.h"
#include "better_text_parsing.h"

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

float
TextLayoutScale(TextLayout layout)
{
    return (float)layout.font_size / (float)layout.font->base_size;
}

void
_RenderUtf32Char(u32 utf32_char, Vec2f *pen, u32 size, Color color, Font font, bool round_to_integer_pixel)
{
    TIMED_BLOCK;

    //float scale = size / (float)font.face->units_per_EM;
    float scale = (float)size/(float)font.base_size;
    gl->BindTexture(GL_TEXTURE_2D, font.gl_texture[utf32_char]);

    // Drawing texture to screen
    Vec2i texture_size = font.texture_size[utf32_char];
    Vec2f char_size = scale*Vec2f{(float)texture_size.x, (float)texture_size.y};
    Vec2f char_pos = {pen->x + scale*font.bitmap_left[utf32_char],
                      pen->y - scale*font.bitmap_top[utf32_char] + scale*font.ascender};

    // Round to nearest integer pixel since we're not doing sub-pixel rendering.
    GLfloat char_verts[] = {
        (char_pos.x),             (char_pos.y), 0.f, 0.f,
        (char_pos.x+char_size.x), (char_pos.y), 1.f, 0.f,
        (char_pos.x),             (char_pos.y+char_size.y), 0.f, 1.f,
        (char_pos.x+char_size.x), (char_pos.y+char_size.y), 1.f, 1.f
    };

    if(round_to_integer_pixel)
    {
        char_verts[0]  = m::Round(char_verts[0]);
        char_verts[1]  = m::Round(char_verts[1]);
        char_verts[4]  = m::Round(char_verts[4]);
        char_verts[5]  = m::Round(char_verts[5]);
        char_verts[8]  = m::Round(char_verts[8]);
        char_verts[9]  = m::Round(char_verts[9]);
        char_verts[12] = m::Round(char_verts[12]);
        char_verts[13] = m::Round(char_verts[13]);
    }


    pen->x += scale*font.advance_x[utf32_char];

    gl->BufferData(GL_ARRAY_BUFFER, sizeof(char_verts), char_verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

float
LineHeight(TextLayout layout)
{
    TIMED_BLOCK;

    if(!layout.font->is_init) return 0.f;

    float scale = TextLayoutScale(layout);
    return TextLayoutScale(layout)*(layout.font->height);
}


// Renders characters of *string until a null byte or a newline character is reached,
// or until c::max_text_render_length is reached.
// Returns the size of the rendered line in pixels
// (Note: the size isn't exact to the pixel, but should be guaranteed to be at least larger than the rendered string,
// but usually will only be slightly larger; but may be consistently too small when using things like accented chars,
// because the size is calculated using TTF metrics)
Vec2f
SizeUtf8Line(TextLayout layout, const char *string)
{
    TIMED_BLOCK;

    Buffer buffer = BufferFromCString(string);
    u32 utf32_char;
    Vec2f pen = {0.f,0.f};
    float scale = TextLayoutScale(layout);

    while(NextAsUtf32Char(&buffer, &utf32_char))
    {
        if(utf32_char == '\n')
        {
            break;
        }
        else
        {
            pen.x += scale*layout.font->advance_x[utf32_char];
        }
    }

    return Vec2f{pen.x, LineHeight(layout)};
}

Rect
DrawText(TextLayout layout, Vec2f origin, String string)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    //StringBuffer buffer = CreateStringBuffer(string.data);
    u32 utf32_char;
    Vec2f text_size = SizeText(layout, string);

    origin = AlignRect({origin, text_size}, layout.align).pos;
    Vec2f pen = origin;

    for(int i=0; i<string.length; i++)
    {
        u32 utf32_char;
        Utf8ToUtf32(string, i, &utf32_char);
        if(utf32_char == '\n')
        {
            break;
        }
        else
        {
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, layout.color, *layout.font);
        }
    }

    if(layout.draw_debug)
    {
        DrawUnfilledRect({origin, text_size}, layout.color);
    }

    return Rect{.pos = origin, .size = text_size};
}

Rect
DrawText(TextLayout layout, Vec2f origin, const char *string, ...)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    char *formatted_string;
    mFormatString(formatted_string, string);

    return DrawText(layout, origin, StringFromCString(formatted_string));

    // Buffer buffer = BufferFromCString(formatted_string);
    // u32 utf32_char;
    // Vec2f text_size = SizeUtf8Line(layout, formatted_string);

    // origin = AlignRect({origin, text_size}, layout.align).pos;
    // Vec2f pen = origin;

    // while(NextAsUtf32Char(&buffer, &utf32_char))
    // {
    //     if(utf32_char == '`')
    //     { // beginning of color code
    //         ConfirmNextTokenType(&buffer, TokenType_::Backtick);
    //         Token color_name = NextToken(&buffer);
    //         ConfirmNextTokenType(&buffer, TokenType_::Backtick);

    //         Color color = layout.color;

    //         if(TokenMatchesString(color_name, "red")) color = c::red;

    //         gl->ProgramUniform4f(game->uv_shader, 2, color.r, color.g, color.b, color.a);
    //     }
    //     else if(utf32_char == '\n')
    //     {
    //         break;
    //     }
    //     else
    //     {
    //         _RenderUtf32Char(utf32_char, &pen, layout.font_size, layout.color, *layout.font);
    //     }
    // }

    // if(layout.draw_debug)
    // {
    //     DrawUnfilledRect({origin, text_size}, layout.color);
    // }

    // return text_size;
}

// @robustness: This doesn't actually return the width of the multiline text.
//              It just uses the width of the first line. The height should be correct.
//              I didn't fix this because I rarely use the width, but it's something
//              that will probably end up popping up at some point.
Vec2f
DrawTextMultiline(TextLayout layout, Vec2f origin, const char *string, ...)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    char *formatted_string;
    mFormatString(formatted_string, string);

    Buffer buffer = BufferFromCString(formatted_string);
    u32 utf32_char;
    Vec2f text_size = SizeUtf8Line(layout, formatted_string);

    origin = AlignRect({origin, text_size}, layout.align).pos;
    Vec2f pen = origin;

    while(NextAsUtf32Char(&buffer, &utf32_char))
    {
        if(utf32_char == '`')
        { // beginning of color code
            ConfirmNextTokenType(&buffer, TokenType_::Backtick);
            Token color_name = NextToken(&buffer);
            ConfirmNextTokenType(&buffer, TokenType_::Backtick);
            Color color = layout.color;

            if(     TokenMatchesString(color_name, "reset")) color = layout.color;
            else if(TokenMatchesString(color_name, "red")) color = c::red;
            else if(TokenMatchesString(color_name, "green")) color = c::green;
            else if(TokenMatchesString(color_name, "blue")) color = c::blue;
            else if(TokenMatchesString(color_name, "lt_blue")) color = c::lt_blue;
            else if(TokenMatchesString(color_name, "yellow")) color = c::yellow;
            else if(TokenMatchesString(color_name, "gold")) color = c::gold;

            gl->ProgramUniform4f(game->uv_shader, 2, color.r, color.g, color.b, color.a);
        }
        else if(utf32_char == '\n')
        {
            if(pen.x > text_size.x) text_size.x = pen.x;
            text_size.y += LineHeight(layout);
            pen.x = origin.x;
            pen.y += LineHeight(layout);
        }
        else
        {
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, layout.color, *layout.font);
        }
    }

    if(layout.draw_debug)
    {
        DrawUnfilledRect({origin, text_size}, layout.color);
    }

    return text_size;
}

Vec2f
DrawTextMultiline(TextLayout layout, Vec2f origin, String string)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    Vec2f text_size = SizeText(layout, string);

    origin = AlignRect({origin, text_size}, layout.align).pos;
    Vec2f pen = origin;

    for(int i=0; i<string.length; i++)
    {
        u32 utf32_char;
        Utf8ToUtf32(string, i, &utf32_char);
        if(utf32_char == '\n')
        {
            pen.x = origin.x;
            pen.y += LineHeight(layout);
        }
        else
        {
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, layout.color, *layout.font);
        }
    }

    if(layout.draw_debug)
    {
        DrawUnfilledRect({origin, text_size}, layout.color);
    }

    return text_size;
}

Vec2f
SizeText(TextLayout layout, String string, int char_count)
{
    TIMED_BLOCK;

    Vec2f pen = {0.f,0.f};
    float scale = TextLayoutScale(layout);

    // Consider the whole string.
    if(char_count < 0)
    {
        char_count = string.length;
    }
    else
    {
        int a =0;
    }

    // Iterate over each char, gets its x-advance, and add it to the size.
    for(int i=0; i<char_count; i++)
    {
        u32 utf32_char;
        Utf8ToUtf32(string, i, &utf32_char);
        if(utf32_char == '\n')
        {
            break;
        }
        else
        {
            pen.x += scale*layout.font->advance_x[utf32_char];
        }
    }

    return Vec2f{pen.x, LineHeight(layout)};
}

// [char_count] is an optional parameter which will count only that number of chars
// in the size, rather than the entire string. A negative char_count will use the
// whole string.
Vec2f
SizeTextUtf32(TextLayout layout, Utf32String string, int char_count=-1)
{
    TIMED_BLOCK;

    Vec2f pen = {0.f,0.f};
    float scale = TextLayoutScale(layout);

    // Consider the whole string.
    if(char_count < 0)
    {
        char_count = string.length;
    }
    else
    {
        int a =0;
    }

    // Iterate over each char, gets its x-advance, and add it to the size.
    for(int i=0; i<char_count; i++)
    {
        u32 utf32_char = CharAt(&string, i);
        if(utf32_char == '\n')
        {
            break;
        }
        else
        {
            pen.x += scale*layout.font->advance_x[utf32_char];
        }
    }

    return Vec2f{pen.x, LineHeight(layout)};
}

Vec2f
DrawTextUtf32(TextLayout layout, Vec2f origin, Utf32String string)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    Vec2f text_size = SizeTextUtf32(layout, string);

    origin = AlignRect({origin, text_size}, layout.align).pos;
    Vec2f pen = origin;

    for(u32 utf32_char : string)
    {
        if(utf32_char == '\n')
        {
            break;
        }
        else
        {
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, layout.color, *layout.font);
        }
    }

    if(layout.draw_debug)
    {
        DrawUnfilledRect({origin, text_size}, layout.color);
    }

    return text_size;
}

// Just draw text as easily as possible (default text layout, at (0,0))
void EasyDrawText(const char *string, ...)
{
    char *formatted_string;
    mFormatString(formatted_string, string);

    DrawText(c::def_text_layout, {0.f,0.f}, formatted_string);
}

Vec2f
ErrorDrawText(Vec2f origin, const char *string, ...)
{
    TextLayout layout = c::error_text_layout;

    char *formatted_string;
    mFormatString(formatted_string, string);

    Vec2f text_size = DrawText(layout, origin, formatted_string).size;
    origin = AlignRect({origin, text_size}, layout.align).pos;

    Color flash_color = layout.color;
    flash_color.a = g::error_flash_counter;
    DrawFilledRect({origin, text_size}, flash_color);

    return text_size;
}

void
DrawDummyText(TextLayout layout, Vec2f pos)
{
    pos.y += DrawText(layout, pos, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Lorem ipsum dolor sit amet, consectetur adipiscing elit.").size.y;
    pos.y += DrawText(layout, pos, "Nunc fermentum tellus non massa porta tristique. Nunc fermentum tellus non massa porta tristique. Nunc fermentum tellus non massa porta tristique.").size.y;
    pos.y += DrawText(layout, pos, "Fusce faucibus leo ac nibh tincidunt, nec vestibulum erat fringilla. Fusce faucibus leo ac nibh tincidunt, nec vestibulum erat fringilla. Fusce faucibus leo ac nibh tincidunt, nec vestibulum erat fringilla.").size.y;
    pos.y += DrawText(layout, pos, "Vestibulum hendrerit dui vel velit congue, non mattis mi blandit. Vestibulum hendrerit dui vel velit congue, non mattis mi blandit. Vestibulum hendrerit dui vel velit congue, non mattis mi blandit.").size.y;
    pos.y += DrawText(layout, pos, "Suspendisse tincidunt augue eget volutpat rhoncus. Suspendisse tincidunt augue eget volutpat rhoncus. Suspendisse tincidunt augue eget volutpat rhoncus.").size.y;
    pos.y += DrawText(layout, pos, "Praesent nec ex dapibus, egestas est id, elementum purus. Praesent nec ex dapibus, egestas est id, elementum purus. Praesent nec ex dapibus, egestas est id, elementum purus.").size.y;
    pos.y += DrawText(layout, pos, "Vivamus eu lorem quis dolor lacinia molestie vel ac sem. Vivamus eu lorem quis dolor lacinia molestie vel ac sem. Vivamus eu lorem quis dolor lacinia molestie vel ac sem.").size.y;
    pos.y += DrawText(layout, pos, "Nulla vestibulum leo ut turpis consectetur, et sagittis erat laoreet. Nulla vestibulum leo ut turpis consectetur, et sagittis erat laoreet. Nulla vestibulum leo ut turpis consectetur, et sagittis erat laoreet.").size.y;
    pos.y += DrawText(layout, pos, "Vestibulum blandit nulla et mauris sodales, at congue neque rhoncus. Vestibulum blandit nulla et mauris sodales, at congue neque rhoncus. Vestibulum blandit nulla et mauris sodales, at congue neque rhoncus.").size.y;
    pos.y += DrawText(layout, pos, "Fusce ut nulla imperdiet, faucibus nibh non, ornare ante. Fusce ut nulla imperdiet, faucibus nibh non, ornare ante. Fusce ut nulla imperdiet, faucibus nibh non, ornare ante.").size.y;
    pos.y += DrawText(layout, pos, "Fusce ullamcorper sapien at accumsan interdum. Fusce ullamcorper sapien at accumsan interdum. Fusce ullamcorper sapien at accumsan interdum.").size.y;
    pos.y += DrawText(layout, pos, "Ut dignissim massa at felis aliquam rhoncus. Ut dignissim massa at felis aliquam rhoncus. Ut dignissim massa at felis aliquam rhoncus.").size.y;
    pos.y += DrawText(layout, pos, "Proin eget felis vitae enim efficitur feugiat sed id nibh. Proin eget felis vitae enim efficitur feugiat sed id nibh. Proin eget felis vitae enim efficitur feugiat sed id nibh.").size.y;
    pos.y += DrawText(layout, pos, "Maecenas scelerisque urna quis nibh malesuada, id elementum dolor malesuada. Maecenas scelerisque urna quis nibh malesuada, id elementum dolor malesuada. Maecenas scelerisque urna quis nibh malesuada, id elementum dolor malesuada.").size.y;
    pos.y += DrawText(layout, pos, "Donec laoreet arcu eget arcu cursus, convallis ullamcorper enim ornare. Donec laoreet arcu eget arcu cursus, convallis ullamcorper enim ornare. Donec laoreet arcu eget arcu cursus, convallis ullamcorper enim ornare.").size.y;
    pos.y += DrawText(layout, pos, "Sed quis lacus ut eros luctus ultricies nec et mauris. Sed quis lacus ut eros luctus ultricies nec et mauris. Sed quis lacus ut eros luctus ultricies nec et mauris.").size.y;
    pos.y += DrawText(layout, pos, "Integer eu elit faucibus, cursus turpis nec, bibendum tortor. Integer eu elit faucibus, cursus turpis nec, bibendum tortor. Integer eu elit faucibus, cursus turpis nec, bibendum tortor.").size.y;
    pos.y += DrawText(layout, pos, "Morbi finibus tellus venenatis, consequat libero in, cursus diam. Morbi finibus tellus venenatis, consequat libero in, cursus diam. Morbi finibus tellus venenatis, consequat libero in, cursus diam.").size.y;
    pos.y += DrawText(layout, pos, "Sed gravida dolor sit amet erat vestibulum auctor. Sed gravida dolor sit amet erat vestibulum auctor. Sed gravida dolor sit amet erat vestibulum auctor.").size.y;
    pos.y += DrawText(layout, pos, "Phasellus nec risus vehicula, volutpat tellus et, posuere nunc. Phasellus nec risus vehicula, volutpat tellus et, posuere nunc. Phasellus nec risus vehicula, volutpat tellus et, posuere nunc.").size.y;
    pos.y += DrawText(layout, pos, "Morbi ac ligula sagittis, volutpat odio eu, semper lacus. Morbi ac ligula sagittis, volutpat odio eu, semper lacus. Morbi ac ligula sagittis, volutpat odio eu, semper lacus.").size.y;
    pos.y += DrawText(layout, pos, "Suspendisse iaculis quam et facilisis aliquam. Suspendisse iaculis quam et facilisis aliquam. Suspendisse iaculis quam et facilisis aliquam.").size.y;
    pos.y += DrawText(layout, pos, "Suspendisse ut ligula scelerisque, maximus nibh at, fermentum urna. Suspendisse ut ligula scelerisque, maximus nibh at, fermentum urna. Suspendisse ut ligula scelerisque, maximus nibh at, fermentum urna.").size.y;
    pos.y += DrawText(layout, pos, "Pellentesque interdum felis a ipsum tempus, at vehicula massa aliquet. Pellentesque interdum felis a ipsum tempus, at vehicula massa aliquet. Pellentesque interdum felis a ipsum tempus, at vehicula massa aliquet.").size.y;
    pos.y += DrawText(layout, pos, "Phasellus faucibus neque quis leo vestibulum, a condimentum nulla interdum. Phasellus faucibus neque quis leo vestibulum, a condimentum nulla interdum. Phasellus faucibus neque quis leo vestibulum, a condimentum nulla interdum.").size.y;
    pos.y += DrawText(layout, pos, "Aenean feugiat augue eu neque volutpat auctor. Aenean feugiat augue eu neque volutpat auctor. Aenean feugiat augue eu neque volutpat auctor.").size.y;
    pos.y += DrawText(layout, pos, "Vivamus sit amet tellus eu libero feugiat vulputate vitae non augue. Vivamus sit amet tellus eu libero feugiat vulputate vitae non augue. Vivamus sit amet tellus eu libero feugiat vulputate vitae non augue.").size.y;
    pos.y += DrawText(layout, pos, "Quisque ut ipsum vel leo aliquet luctus sit amet vel neque. Quisque ut ipsum vel leo aliquet luctus sit amet vel neque. Quisque ut ipsum vel leo aliquet luctus sit amet vel neque.").size.y;
    pos.y += DrawText(layout, pos, "Proin et ex eu diam ultricies ultrices laoreet at quam. Proin et ex eu diam ultricies ultrices laoreet at quam. Proin et ex eu diam ultricies ultrices laoreet at quam.").size.y;
    pos.y += DrawText(layout, pos, "Duis ultricies augue sodales arcu vehicula, et malesuada dolor sagittis. Duis ultricies augue sodales arcu vehicula, et malesuada dolor sagittis. Duis ultricies augue sodales arcu vehicula, et malesuada dolor sagittis.").size.y;
    pos.y += DrawText(layout, pos, "Suspendisse non felis sed felis congue dapibus. Suspendisse non felis sed felis congue dapibus. Suspendisse non felis sed felis congue dapibus.").size.y;
    pos.y += DrawText(layout, pos, "Phasellus a augue quis ipsum interdum fermentum. Phasellus a augue quis ipsum interdum fermentum. Phasellus a augue quis ipsum interdum fermentum.").size.y;
    pos.y += DrawText(layout, pos, "Donec accumsan ex blandit odio aliquet vulputate. Donec accumsan ex blandit odio aliquet vulputate. Donec accumsan ex blandit odio aliquet vulputate.").size.y;
    pos.y += DrawText(layout, pos, "Integer et odio nec sem sagittis tristique. Integer et odio nec sem sagittis tristique. Integer et odio nec sem sagittis tristique.").size.y;
    pos.y += DrawText(layout, pos, "Fusce a lacus at est volutpat molestie eget quis nulla. Fusce a lacus at est volutpat molestie eget quis nulla. Fusce a lacus at est volutpat molestie eget quis nulla.").size.y;
    pos.y += DrawText(layout, pos, "In nec ipsum consequat libero sagittis lobortis. In nec ipsum consequat libero sagittis lobortis. In nec ipsum consequat libero sagittis lobortis.").size.y;
    pos.y += DrawText(layout, pos, "Aliquam sollicitudin augue id nisl pulvinar, id fermentum neque mattis. Aliquam sollicitudin augue id nisl pulvinar, id fermentum neque mattis. Aliquam sollicitudin augue id nisl pulvinar, id fermentum neque mattis.").size.y;
    pos.y += DrawText(layout, pos, "Vivamus vel nibh tristique, luctus nibh sed, viverra diam. Vivamus vel nibh tristique, luctus nibh sed, viverra diam. Vivamus vel nibh tristique, luctus nibh sed, viverra diam.").size.y;
    pos.y += DrawText(layout, pos, "Proin semper tortor eget massa condimentum, eget luctus lorem consectetur. Proin semper tortor eget massa condimentum, eget luctus lorem consectetur. Proin semper tortor eget massa condimentum, eget luctus lorem consectetur.").size.y;
    pos.y += DrawText(layout, pos, "Vestibulum accumsan dolor ut porttitor mattis. Vestibulum accumsan dolor ut porttitor mattis. Vestibulum accumsan dolor ut porttitor mattis.").size.y;
    pos.y += DrawText(layout, pos, "Pellentesque sit amet sem lobortis, interdum dolor non, pharetra sem. Pellentesque sit amet sem lobortis, interdum dolor non, pharetra sem. Pellentesque sit amet sem lobortis, interdum dolor non, pharetra sem.").size.y;
    pos.y += DrawText(layout, pos, "Integer placerat eros vel lectus dapibus rutrum. Integer placerat eros vel lectus dapibus rutrum. Integer placerat eros vel lectus dapibus rutrum.").size.y;
    pos.y += DrawText(layout, pos, "Nullam ut sem eu magna posuere cursus. Nullam ut sem eu magna posuere cursus. Nullam ut sem eu magna posuere cursus.").size.y;
    pos.y += DrawText(layout, pos, "Integer a dui ornare, hendrerit orci in, gravida ligula. Integer a dui ornare, hendrerit orci in, gravida ligula. Integer a dui ornare, hendrerit orci in, gravida ligula.").size.y;
    pos.y += DrawText(layout, pos, "Aliquam a nulla eu nisi imperdiet interdum. Aliquam a nulla eu nisi imperdiet interdum. Aliquam a nulla eu nisi imperdiet interdum.").size.y;
    pos.y += DrawText(layout, pos, "Morbi eget nulla id lorem pharetra dapibus ut eget nibh. Morbi eget nulla id lorem pharetra dapibus ut eget nibh. Morbi eget nulla id lorem pharetra dapibus ut eget nibh.").size.y;
    pos.y += DrawText(layout, pos, "Integer scelerisque sapien at ligula lobortis dictum. Integer scelerisque sapien at ligula lobortis dictum. Integer scelerisque sapien at ligula lobortis dictum.").size.y;
    pos.y += DrawText(layout, pos, "Quisque at ex eleifend, vestibulum urna non, congue metus. Quisque at ex eleifend, vestibulum urna non, congue metus. Quisque at ex eleifend, vestibulum urna non, congue metus.").size.y;
    pos.y += DrawText(layout, pos, "Ut imperdiet tellus non metus mollis lacinia. Ut imperdiet tellus non metus mollis lacinia. Ut imperdiet tellus non metus mollis lacinia.").size.y;
    pos.y += DrawText(layout, pos, "Aliquam quis neque at ex porta molestie. Aliquam quis neque at ex porta molestie. Aliquam quis neque at ex porta molestie.").size.y;
    pos.y += DrawText(layout, pos, "Pellentesque et mi nec leo ornare convallis. Pellentesque et mi nec leo ornare convallis. Pellentesque et mi nec leo ornare convallis.").size.y;
    pos.y += DrawText(layout, pos, "Proin facilisis magna ut quam posuere semper. Proin facilisis magna ut quam posuere semper. Proin facilisis magna ut quam posuere semper.").size.y;
    pos.y += DrawText(layout, pos, "Nullam in augue sodales, consectetur tortor vitae, sodales arcu. Nullam in augue sodales, consectetur tortor vitae, sodales arcu. Nullam in augue sodales, consectetur tortor vitae, sodales arcu.").size.y;
    pos.y += DrawText(layout, pos, "Cras malesuada elit vitae sem blandit, non mattis felis pretium. Cras malesuada elit vitae sem blandit, non mattis felis pretium. Cras malesuada elit vitae sem blandit, non mattis felis pretium.").size.y;
    pos.y += DrawText(layout, pos, "Mauris mollis leo at mi scelerisque hendrerit. Mauris mollis leo at mi scelerisque hendrerit. Mauris mollis leo at mi scelerisque hendrerit.").size.y;
    pos.y += DrawText(layout, pos, "Vestibulum id libero aliquam, cursus est ac, eleifend augue. Vestibulum id libero aliquam, cursus est ac, eleifend augue. Vestibulum id libero aliquam, cursus est ac, eleifend augue.").size.y;
    pos.y += DrawText(layout, pos, "Mauris et ex ut quam volutpat eleifend vitae a sapien. Mauris et ex ut quam volutpat eleifend vitae a sapien. Mauris et ex ut quam volutpat eleifend vitae a sapien.").size.y;
    pos.y += DrawText(layout, pos, "Donec eget massa ut nisl tempus pretium. Donec eget massa ut nisl tempus pretium. Donec eget massa ut nisl tempus pretium.").size.y;
    pos.y += DrawText(layout, pos, "Mauris a erat tristique, vulputate velit eget, porta nisi. Mauris a erat tristique, vulputate velit eget, porta nisi. Mauris a erat tristique, vulputate velit eget, porta nisi.").size.y;
    pos.y += DrawText(layout, pos, "Quisque cursus magna id ipsum viverra malesuada. Quisque cursus magna id ipsum viverra malesuada. Quisque cursus magna id ipsum viverra malesuada.").size.y;
    pos.y += DrawText(layout, pos, "Sed vehicula lectus pretium lobortis placerat. Sed vehicula lectus pretium lobortis placerat. Sed vehicula lectus pretium lobortis placerat.").size.y;
    pos.y += DrawText(layout, pos, "In sit amet mi id est cursus aliquam. In sit amet mi id est cursus aliquam. In sit amet mi id est cursus aliquam.").size.y;
    pos.y += DrawText(layout, pos, "Phasellus ac nulla sit amet erat luctus interdum. Phasellus ac nulla sit amet erat luctus interdum. Phasellus ac nulla sit amet erat luctus interdum.").size.y;
    pos.y += DrawText(layout, pos, "Curabitur id justo malesuada, tristique dui non, efficitur magna. Curabitur id justo malesuada, tristique dui non, efficitur magna. Curabitur id justo malesuada, tristique dui non, efficitur magna.").size.y;
    pos.y += DrawText(layout, pos, "Curabitur sodales nibh a iaculis accumsan. Curabitur sodales nibh a iaculis accumsan. Curabitur sodales nibh a iaculis accumsan.").size.y;
    pos.y += DrawText(layout, pos, "In ut turpis imperdiet massa bibendum placerat. In ut turpis imperdiet massa bibendum placerat. In ut turpis imperdiet massa bibendum placerat.").size.y;
    pos.y += DrawText(layout, pos, "Sed id quam non velit accumsan facilisis in at ante. Sed id quam non velit accumsan facilisis in at ante. Sed id quam non velit accumsan facilisis in at ante.").size.y;
}

// void
// RenderS32AsString(u32 value, Vec2f origin, u32 size)
// {
//  if(value < 0) return; // Negative not implemented yet

//  Vec2f pen = origin;
//  FT_Face face = game->default_font.face;
//  FT_Set_Pixel_Sizes(face, 0, size);
//  float scale = size / (float)face->units_per_EM;

//  u32 temp_value = value;
//  int digit_count = 1;
//  while(temp_value > 9)
//  {
//      temp_value /= 10;
//      digit_count += 1;
//  }

//  for(int digit=0; digit<digit_count; digit++)
//  {
//      u32 factor = m::Pow(10, digit_count-digit-1);
//      temp_value = value / factor;
//      value -= temp_value * factor;

//      u32 utf32_char = DigitToUtf32Char(temp_value);
//      if(utf32_char == '\n')
//      {
//          pen.y += scale*(face->height);
//          pen.x = origin.x;
//      }
//      else
//      {
//          _RenderUtf32Char(utf32_char, &pen, size, face);
//      }
//  }
// }

// void
// RenderUtf32String(Utf32String text, Vec2f origin, u32 size)
// {
//  FT_Face face = game->default_font.face;
//  FT_Set_Pixel_Sizes(face, 0, size);
//  float scale = size / (float)face->units_per_EM;
//  Vec2f pen = origin;
//  gl->BindTexture(GL_TEXTURE_2D, game->temp_texture);

//  pen.y += scale*(face->bbox.yMax);

//  for(int i=0; i<text.char_count; i++)
//  {
//      if(text.chars[i] == '\n') {
//          pen.y += scale*(face->height);
//          pen.x = origin.x;
//          //log("newline");
//          continue;
//      }

//      // Generate character texture
//      FT_Load_Char(face, text.chars[i], FT_LOAD_RENDER);
//      //FT_Render_Glyph(0, FT_RENDER_MODE_NORMAL);

//      Bitmap char_bmp = AllocBitmap(face->glyph->bitmap.width,
//                                    face->glyph->bitmap.rows);
//      auto &alpha_map = face->glyph->bitmap;

//      u32 pixel_count = char_bmp.width*char_bmp.height;
//      for(int i=0; i<pixel_count; i++)
//      {
//          char_bmp.pixels[i].r = alpha_map.buffer[i];
//          char_bmp.pixels[i].g = alpha_map.buffer[i];
//          char_bmp.pixels[i].b = alpha_map.buffer[i];
//          char_bmp.pixels[i].a = alpha_map.buffer[i];
//      }

//      gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, char_bmp.width, char_bmp.height,
//                     0, GL_RGBA, GL_UNSIGNED_BYTE, char_bmp.pixels);
//      DeallocBitmap(char_bmp);

//      // Drawing texture to screen

//      // First char, so let's account for (e.g.) negative horizontal bearing X
//      //if(i==0) pen.x = -(face->glyph->metrics.horiBearingX >> 6);

//      Vec2f char_pos = {pen.x + (face->glyph->bitmap_left),
//                        pen.y - (face->glyph->bitmap_top)};

//      GLfloat verts[] = {
//          char_pos.x, char_pos.y, 0.f, 0.f,
//          char_pos.x+char_bmp.width, char_pos.y, 1.f, 0.f,
//          char_pos.x, char_pos.y+char_bmp.height, 0.f, 1.f,
//          char_pos.x+char_bmp.width, char_pos.y+char_bmp.height, 1.f, 1.f
//      };

//      pen.x += (face->glyph->advance.x >> 6);

//      gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
//      gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//  }
// }

bool
ValidFont(Font *font)
{
    return(font and font->is_init);
}