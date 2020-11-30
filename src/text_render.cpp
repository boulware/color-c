#include "text_render.h"

#include "opengl.h"
#include "math.h"
//#include "draw.h"
#include "memory.h"
#include "utf32string.h"
#include "better_text_parsing.h"
#include "bitmap.h"

float
TextLayoutScale(TextLayout layout)
{
    return (float)layout.font_size / (float)layout.font->base_size;
}

void
_RenderUtf32Char(u32 utf32_char, Vec2f *pen, u32 size, Font font, bool round_to_integer_pixel)
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

TextResponse
DrawText(TextLayout layout, Vec2f origin, String string)
{
    TIMED_BLOCK;

    TextResponse response = {};
    ActivateUvShader(layout.color);

    //StringBuffer buffer = CreateStringBuffer(string.data);
    u32 utf32_char;
    Buffer buffer = BufferFromString(string);
    Vec2f text_size = SizeText(layout, string);
    Rect text_rect = AlignRect({origin, text_size}, layout.align);

    origin = text_rect.pos;
    Vec2f pen = origin;

    Color text_color = layout.color;
    if(MouseInRect(text_rect))
    {
        if(layout.has_hover_color) text_color = layout.hover_color;
        response.hovered = true;
    }
    response.rect = text_rect;

    gl->ProgramUniform4f(game->uv_shader, 2, text_color.r, text_color.g, text_color.b, text_color.a);


    for(int i=0; i<string.length; i++)
    {
        u32 utf32_char;
        Utf8ToUtf32(string, i, &utf32_char);

        if(utf32_char == '`' and !(response.hovered and layout.has_hover_color))
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

            //if(response.hovered and layout.has_hover_color) color = layout.hover_color; // Hover overrides color codes
            gl->ProgramUniform4f(game->uv_shader, 2, color.r, color.g, color.b, color.a);
        }
        else if(utf32_char == '\n')
        {
            break;
        }
        else
        {
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, *layout.font);
        }
    }

    if(layout.draw_debug)
    {
        DrawUnfilledRect(text_rect, layout.color);
    }

    //return Rect{.pos = origin, .size = text_size};
    return response;
}

TextResponse
DrawText(TextLayout layout, Vec2f origin, const char *string, ...)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    char *formatted_string;
    mFormatString(formatted_string, string);

    return DrawText(layout, origin, StringFromCString(formatted_string));
}

Rect
DrawUiText(TextLayout layout, Vec2f origin, String string)
{
    Camera initial_cam = PushUiCamera();

    auto response = DrawText(layout, origin, string);

    PopUiCamera(initial_cam);

    return response.rect;
}

Rect
DrawUiText(TextLayout layout, Vec2f origin, const char *string, ...)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    char *formatted_string;
    mFormatString(formatted_string, string);

    return DrawUiText(layout, origin, StringFromCString(formatted_string));
}

Vec2f
DrawTextMultiline(TextLayout layout, Vec2f origin, String string)
{
    TIMED_BLOCK;

    ActivateUvShader(layout.color);

    Buffer buffer = BufferFromString(string);
    Vec2f text_size = SizeText(layout, string);

    origin = AlignRect({origin, text_size}, layout.align).pos;
    Vec2f pen = origin;

    u32 utf32_char;
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
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, *layout.font);
        }
    }

    if(layout.draw_debug)
    {
        DrawUnfilledRect({origin, text_size}, layout.color);
    }

    return text_size;
}

// @robustness: This doesn't actually return the width of the multiline text.
//              It just uses the width of the first line. The height should be correct.
//              I didn't fix this because I rarely use the width, but it's something
//              that will probably end up popping up at some point.
Vec2f
DrawTextMultiline(TextLayout layout, Vec2f origin, const char *string, ...)
{
    TIMED_BLOCK;

    char *formatted_string;
    mFormatString(formatted_string, string);

    return DrawTextMultiline(layout, origin, StringFromCString(formatted_string));

    #if 0
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
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, *layout.font);
        }
    }

    if(layout.draw_debug)
    {
        DrawUnfilledRect({origin, text_size}, layout.color);
    }

    return text_size;
    #endif
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
    u32 utf32_char;
    Buffer buffer = BufferFromString(string);
    while(NextAsUtf32Char(&buffer, &utf32_char))
    {
        if(utf32_char == '\n')
        {
            break;
        }
        else if(utf32_char == '`')
        { // Skip color codes
            while(NextAsUtf32Char(&buffer, &utf32_char))
            {
                if(utf32_char == '`') break;
            }
        }
        else
        {
            pen.x += scale*layout.font->advance_x[utf32_char];
        }
    }

    return Vec2f{pen.x, LineHeight(layout)};
}

Vec2f
SizeText(TextLayout layout, char *c_string, ...)
{
    TIMED_BLOCK;

    char *formatted_string;
    mFormatString(formatted_string, c_string);
    String string = StringFromCString(formatted_string);

    return SizeText(layout, string);
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
            _RenderUtf32Char(utf32_char, &pen, layout.font_size, *layout.font);
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

    Vec2f text_size = DrawText(layout, origin, formatted_string).rect.size;
    origin = AlignRect({origin, text_size}, layout.align).pos;

    Color flash_color = layout.color;
    flash_color.a = g::error_flash_counter;
    DrawFilledRect({origin, text_size}, flash_color);

    return text_size;
}

void
DrawDummyText(TextLayout layout, Vec2f pos)
{
    pos.y += DrawText(layout, pos, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Lorem ipsum dolor sit amet, consectetur adipiscing elit.").rect.size.y;
    pos.y += DrawText(layout, pos, "Nunc fermentum tellus non massa porta tristique. Nunc fermentum tellus non massa porta tristique. Nunc fermentum tellus non massa porta tristique.").rect.size.y;
    pos.y += DrawText(layout, pos, "Fusce faucibus leo ac nibh tincidunt, nec vestibulum erat fringilla. Fusce faucibus leo ac nibh tincidunt, nec vestibulum erat fringilla. Fusce faucibus leo ac nibh tincidunt, nec vestibulum erat fringilla.").rect.size.y;
    pos.y += DrawText(layout, pos, "Vestibulum hendrerit dui vel velit congue, non mattis mi blandit. Vestibulum hendrerit dui vel velit congue, non mattis mi blandit. Vestibulum hendrerit dui vel velit congue, non mattis mi blandit.").rect.size.y;
    pos.y += DrawText(layout, pos, "Suspendisse tincidunt augue eget volutpat rhoncus. Suspendisse tincidunt augue eget volutpat rhoncus. Suspendisse tincidunt augue eget volutpat rhoncus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Praesent nec ex dapibus, egestas est id, elementum purus. Praesent nec ex dapibus, egestas est id, elementum purus. Praesent nec ex dapibus, egestas est id, elementum purus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Vivamus eu lorem quis dolor lacinia molestie vel ac sem. Vivamus eu lorem quis dolor lacinia molestie vel ac sem. Vivamus eu lorem quis dolor lacinia molestie vel ac sem.").rect.size.y;
    pos.y += DrawText(layout, pos, "Nulla vestibulum leo ut turpis consectetur, et sagittis erat laoreet. Nulla vestibulum leo ut turpis consectetur, et sagittis erat laoreet. Nulla vestibulum leo ut turpis consectetur, et sagittis erat laoreet.").rect.size.y;
    pos.y += DrawText(layout, pos, "Vestibulum blandit nulla et mauris sodales, at congue neque rhoncus. Vestibulum blandit nulla et mauris sodales, at congue neque rhoncus. Vestibulum blandit nulla et mauris sodales, at congue neque rhoncus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Fusce ut nulla imperdiet, faucibus nibh non, ornare ante. Fusce ut nulla imperdiet, faucibus nibh non, ornare ante. Fusce ut nulla imperdiet, faucibus nibh non, ornare ante.").rect.size.y;
    pos.y += DrawText(layout, pos, "Fusce ullamcorper sapien at accumsan interdum. Fusce ullamcorper sapien at accumsan interdum. Fusce ullamcorper sapien at accumsan interdum.").rect.size.y;
    pos.y += DrawText(layout, pos, "Ut dignissim massa at felis aliquam rhoncus. Ut dignissim massa at felis aliquam rhoncus. Ut dignissim massa at felis aliquam rhoncus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Proin eget felis vitae enim efficitur feugiat sed id nibh. Proin eget felis vitae enim efficitur feugiat sed id nibh. Proin eget felis vitae enim efficitur feugiat sed id nibh.").rect.size.y;
    pos.y += DrawText(layout, pos, "Maecenas scelerisque urna quis nibh malesuada, id elementum dolor malesuada. Maecenas scelerisque urna quis nibh malesuada, id elementum dolor malesuada. Maecenas scelerisque urna quis nibh malesuada, id elementum dolor malesuada.").rect.size.y;
    pos.y += DrawText(layout, pos, "Donec laoreet arcu eget arcu cursus, convallis ullamcorper enim ornare. Donec laoreet arcu eget arcu cursus, convallis ullamcorper enim ornare. Donec laoreet arcu eget arcu cursus, convallis ullamcorper enim ornare.").rect.size.y;
    pos.y += DrawText(layout, pos, "Sed quis lacus ut eros luctus ultricies nec et mauris. Sed quis lacus ut eros luctus ultricies nec et mauris. Sed quis lacus ut eros luctus ultricies nec et mauris.").rect.size.y;
    pos.y += DrawText(layout, pos, "Integer eu elit faucibus, cursus turpis nec, bibendum tortor. Integer eu elit faucibus, cursus turpis nec, bibendum tortor. Integer eu elit faucibus, cursus turpis nec, bibendum tortor.").rect.size.y;
    pos.y += DrawText(layout, pos, "Morbi finibus tellus venenatis, consequat libero in, cursus diam. Morbi finibus tellus venenatis, consequat libero in, cursus diam. Morbi finibus tellus venenatis, consequat libero in, cursus diam.").rect.size.y;
    pos.y += DrawText(layout, pos, "Sed gravida dolor sit amet erat vestibulum auctor. Sed gravida dolor sit amet erat vestibulum auctor. Sed gravida dolor sit amet erat vestibulum auctor.").rect.size.y;
    pos.y += DrawText(layout, pos, "Phasellus nec risus vehicula, volutpat tellus et, posuere nunc. Phasellus nec risus vehicula, volutpat tellus et, posuere nunc. Phasellus nec risus vehicula, volutpat tellus et, posuere nunc.").rect.size.y;
    pos.y += DrawText(layout, pos, "Morbi ac ligula sagittis, volutpat odio eu, semper lacus. Morbi ac ligula sagittis, volutpat odio eu, semper lacus. Morbi ac ligula sagittis, volutpat odio eu, semper lacus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Suspendisse iaculis quam et facilisis aliquam. Suspendisse iaculis quam et facilisis aliquam. Suspendisse iaculis quam et facilisis aliquam.").rect.size.y;
    pos.y += DrawText(layout, pos, "Suspendisse ut ligula scelerisque, maximus nibh at, fermentum urna. Suspendisse ut ligula scelerisque, maximus nibh at, fermentum urna. Suspendisse ut ligula scelerisque, maximus nibh at, fermentum urna.").rect.size.y;
    pos.y += DrawText(layout, pos, "Pellentesque interdum felis a ipsum tempus, at vehicula massa aliquet. Pellentesque interdum felis a ipsum tempus, at vehicula massa aliquet. Pellentesque interdum felis a ipsum tempus, at vehicula massa aliquet.").rect.size.y;
    pos.y += DrawText(layout, pos, "Phasellus faucibus neque quis leo vestibulum, a condimentum nulla interdum. Phasellus faucibus neque quis leo vestibulum, a condimentum nulla interdum. Phasellus faucibus neque quis leo vestibulum, a condimentum nulla interdum.").rect.size.y;
    pos.y += DrawText(layout, pos, "Aenean feugiat augue eu neque volutpat auctor. Aenean feugiat augue eu neque volutpat auctor. Aenean feugiat augue eu neque volutpat auctor.").rect.size.y;
    pos.y += DrawText(layout, pos, "Vivamus sit amet tellus eu libero feugiat vulputate vitae non augue. Vivamus sit amet tellus eu libero feugiat vulputate vitae non augue. Vivamus sit amet tellus eu libero feugiat vulputate vitae non augue.").rect.size.y;
    pos.y += DrawText(layout, pos, "Quisque ut ipsum vel leo aliquet luctus sit amet vel neque. Quisque ut ipsum vel leo aliquet luctus sit amet vel neque. Quisque ut ipsum vel leo aliquet luctus sit amet vel neque.").rect.size.y;
    pos.y += DrawText(layout, pos, "Proin et ex eu diam ultricies ultrices laoreet at quam. Proin et ex eu diam ultricies ultrices laoreet at quam. Proin et ex eu diam ultricies ultrices laoreet at quam.").rect.size.y;
    pos.y += DrawText(layout, pos, "Duis ultricies augue sodales arcu vehicula, et malesuada dolor sagittis. Duis ultricies augue sodales arcu vehicula, et malesuada dolor sagittis. Duis ultricies augue sodales arcu vehicula, et malesuada dolor sagittis.").rect.size.y;
    pos.y += DrawText(layout, pos, "Suspendisse non felis sed felis congue dapibus. Suspendisse non felis sed felis congue dapibus. Suspendisse non felis sed felis congue dapibus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Phasellus a augue quis ipsum interdum fermentum. Phasellus a augue quis ipsum interdum fermentum. Phasellus a augue quis ipsum interdum fermentum.").rect.size.y;
    pos.y += DrawText(layout, pos, "Donec accumsan ex blandit odio aliquet vulputate. Donec accumsan ex blandit odio aliquet vulputate. Donec accumsan ex blandit odio aliquet vulputate.").rect.size.y;
    pos.y += DrawText(layout, pos, "Integer et odio nec sem sagittis tristique. Integer et odio nec sem sagittis tristique. Integer et odio nec sem sagittis tristique.").rect.size.y;
    pos.y += DrawText(layout, pos, "Fusce a lacus at est volutpat molestie eget quis nulla. Fusce a lacus at est volutpat molestie eget quis nulla. Fusce a lacus at est volutpat molestie eget quis nulla.").rect.size.y;
    pos.y += DrawText(layout, pos, "In nec ipsum consequat libero sagittis lobortis. In nec ipsum consequat libero sagittis lobortis. In nec ipsum consequat libero sagittis lobortis.").rect.size.y;
    pos.y += DrawText(layout, pos, "Aliquam sollicitudin augue id nisl pulvinar, id fermentum neque mattis. Aliquam sollicitudin augue id nisl pulvinar, id fermentum neque mattis. Aliquam sollicitudin augue id nisl pulvinar, id fermentum neque mattis.").rect.size.y;
    pos.y += DrawText(layout, pos, "Vivamus vel nibh tristique, luctus nibh sed, viverra diam. Vivamus vel nibh tristique, luctus nibh sed, viverra diam. Vivamus vel nibh tristique, luctus nibh sed, viverra diam.").rect.size.y;
    pos.y += DrawText(layout, pos, "Proin semper tortor eget massa condimentum, eget luctus lorem consectetur. Proin semper tortor eget massa condimentum, eget luctus lorem consectetur. Proin semper tortor eget massa condimentum, eget luctus lorem consectetur.").rect.size.y;
    pos.y += DrawText(layout, pos, "Vestibulum accumsan dolor ut porttitor mattis. Vestibulum accumsan dolor ut porttitor mattis. Vestibulum accumsan dolor ut porttitor mattis.").rect.size.y;
    pos.y += DrawText(layout, pos, "Pellentesque sit amet sem lobortis, interdum dolor non, pharetra sem. Pellentesque sit amet sem lobortis, interdum dolor non, pharetra sem. Pellentesque sit amet sem lobortis, interdum dolor non, pharetra sem.").rect.size.y;
    pos.y += DrawText(layout, pos, "Integer placerat eros vel lectus dapibus rutrum. Integer placerat eros vel lectus dapibus rutrum. Integer placerat eros vel lectus dapibus rutrum.").rect.size.y;
    pos.y += DrawText(layout, pos, "Nullam ut sem eu magna posuere cursus. Nullam ut sem eu magna posuere cursus. Nullam ut sem eu magna posuere cursus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Integer a dui ornare, hendrerit orci in, gravida ligula. Integer a dui ornare, hendrerit orci in, gravida ligula. Integer a dui ornare, hendrerit orci in, gravida ligula.").rect.size.y;
    pos.y += DrawText(layout, pos, "Aliquam a nulla eu nisi imperdiet interdum. Aliquam a nulla eu nisi imperdiet interdum. Aliquam a nulla eu nisi imperdiet interdum.").rect.size.y;
    pos.y += DrawText(layout, pos, "Morbi eget nulla id lorem pharetra dapibus ut eget nibh. Morbi eget nulla id lorem pharetra dapibus ut eget nibh. Morbi eget nulla id lorem pharetra dapibus ut eget nibh.").rect.size.y;
    pos.y += DrawText(layout, pos, "Integer scelerisque sapien at ligula lobortis dictum. Integer scelerisque sapien at ligula lobortis dictum. Integer scelerisque sapien at ligula lobortis dictum.").rect.size.y;
    pos.y += DrawText(layout, pos, "Quisque at ex eleifend, vestibulum urna non, congue metus. Quisque at ex eleifend, vestibulum urna non, congue metus. Quisque at ex eleifend, vestibulum urna non, congue metus.").rect.size.y;
    pos.y += DrawText(layout, pos, "Ut imperdiet tellus non metus mollis lacinia. Ut imperdiet tellus non metus mollis lacinia. Ut imperdiet tellus non metus mollis lacinia.").rect.size.y;
    pos.y += DrawText(layout, pos, "Aliquam quis neque at ex porta molestie. Aliquam quis neque at ex porta molestie. Aliquam quis neque at ex porta molestie.").rect.size.y;
    pos.y += DrawText(layout, pos, "Pellentesque et mi nec leo ornare convallis. Pellentesque et mi nec leo ornare convallis. Pellentesque et mi nec leo ornare convallis.").rect.size.y;
    pos.y += DrawText(layout, pos, "Proin facilisis magna ut quam posuere semper. Proin facilisis magna ut quam posuere semper. Proin facilisis magna ut quam posuere semper.").rect.size.y;
    pos.y += DrawText(layout, pos, "Nullam in augue sodales, consectetur tortor vitae, sodales arcu. Nullam in augue sodales, consectetur tortor vitae, sodales arcu. Nullam in augue sodales, consectetur tortor vitae, sodales arcu.").rect.size.y;
    pos.y += DrawText(layout, pos, "Cras malesuada elit vitae sem blandit, non mattis felis pretium. Cras malesuada elit vitae sem blandit, non mattis felis pretium. Cras malesuada elit vitae sem blandit, non mattis felis pretium.").rect.size.y;
    pos.y += DrawText(layout, pos, "Mauris mollis leo at mi scelerisque hendrerit. Mauris mollis leo at mi scelerisque hendrerit. Mauris mollis leo at mi scelerisque hendrerit.").rect.size.y;
    pos.y += DrawText(layout, pos, "Vestibulum id libero aliquam, cursus est ac, eleifend augue. Vestibulum id libero aliquam, cursus est ac, eleifend augue. Vestibulum id libero aliquam, cursus est ac, eleifend augue.").rect.size.y;
    pos.y += DrawText(layout, pos, "Mauris et ex ut quam volutpat eleifend vitae a sapien. Mauris et ex ut quam volutpat eleifend vitae a sapien. Mauris et ex ut quam volutpat eleifend vitae a sapien.").rect.size.y;
    pos.y += DrawText(layout, pos, "Donec eget massa ut nisl tempus pretium. Donec eget massa ut nisl tempus pretium. Donec eget massa ut nisl tempus pretium.").rect.size.y;
    pos.y += DrawText(layout, pos, "Mauris a erat tristique, vulputate velit eget, porta nisi. Mauris a erat tristique, vulputate velit eget, porta nisi. Mauris a erat tristique, vulputate velit eget, porta nisi.").rect.size.y;
    pos.y += DrawText(layout, pos, "Quisque cursus magna id ipsum viverra malesuada. Quisque cursus magna id ipsum viverra malesuada. Quisque cursus magna id ipsum viverra malesuada.").rect.size.y;
    pos.y += DrawText(layout, pos, "Sed vehicula lectus pretium lobortis placerat. Sed vehicula lectus pretium lobortis placerat. Sed vehicula lectus pretium lobortis placerat.").rect.size.y;
    pos.y += DrawText(layout, pos, "In sit amet mi id est cursus aliquam. In sit amet mi id est cursus aliquam. In sit amet mi id est cursus aliquam.").rect.size.y;
    pos.y += DrawText(layout, pos, "Phasellus ac nulla sit amet erat luctus interdum. Phasellus ac nulla sit amet erat luctus interdum. Phasellus ac nulla sit amet erat luctus interdum.").rect.size.y;
    pos.y += DrawText(layout, pos, "Curabitur id justo malesuada, tristique dui non, efficitur magna. Curabitur id justo malesuada, tristique dui non, efficitur magna. Curabitur id justo malesuada, tristique dui non, efficitur magna.").rect.size.y;
    pos.y += DrawText(layout, pos, "Curabitur sodales nibh a iaculis accumsan. Curabitur sodales nibh a iaculis accumsan. Curabitur sodales nibh a iaculis accumsan.").rect.size.y;
    pos.y += DrawText(layout, pos, "In ut turpis imperdiet massa bibendum placerat. In ut turpis imperdiet massa bibendum placerat. In ut turpis imperdiet massa bibendum placerat.").rect.size.y;
    pos.y += DrawText(layout, pos, "Sed id quam non velit accumsan facilisis in at ante. Sed id quam non velit accumsan facilisis in at ante. Sed id quam non velit accumsan facilisis in at ante.").rect.size.y;
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
//          //Log("newline");
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