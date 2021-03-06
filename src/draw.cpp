#include "draw.h"

#include "camera.h"
#include "geometry.h"

void
ActivateColorShader(Color color)
{
    gl->UseProgram(game->color_shader);
    gl->ProgramUniform4f(game->color_shader, 1, color.r, color.g, color.b, color.a);
    gl->BindVertexArray(game->color_vao);
    gl->BindBuffer(GL_ARRAY_BUFFER, game->color_vbo);
}

void
ActivateUvShader(Color color={1.f,1.f,1.f,0.f})
{
    gl->UseProgram(game->uv_shader);
    gl->BindVertexArray(game->uv_vao);
    gl->BindBuffer(GL_ARRAY_BUFFER, game->uv_vbo);
    gl->ActiveTexture(GL_TEXTURE0);
    gl->ProgramUniform4f(game->uv_shader, 2, color.r, color.g, color.b, color.a);
}

void
SetDrawDepth(float depth)
{
    float clip_space_depth = c::clip_A*depth+c::clip_B;
    gl->ProgramUniform1f(game->uv_shader, 3, clip_space_depth);
    gl->ProgramUniform1f(game->color_shader, 2, clip_space_depth);
}

// void
// SetCameraZoom(float z)
// {
//     game->prev_camera_zoom = game->camera_zoom;
//     game->camera_zoom = z;
//     gl->ProgramUniform1f(game->uv_shader,    5, z);
//     gl->ProgramUniform1f(game->color_shader, 5, z);
// }

// void
// ZoomCameraIntoPoint(float z, Vec2f point)
// {
//     Vec2f new_camera_pos = point - (1/z)*(point - game->camera_pos);
//     SetCameraPos(new_camera_pos);
//     game->camera_zoom *= z;
//     SetCameraZoom(game->camera_zoom);
// }

void
DrawUnfilledRect(Rect rect, Color color)
{
    ActivateColorShader(color);

    GLfloat verts[] = {
        rect.pos.x, rect.pos.y,
        rect.pos.x + rect.size.x, rect.pos.y,
        rect.pos.x + rect.size.x, rect.pos.y + rect.size.y,
        rect.pos.x, rect.pos.y + rect.size.y
    };

    gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_LINE_LOOP, 0, 4);
}

void
DrawUnfilledRect(Vec2f pos, Vec2f size, Color color)
{
    return DrawUnfilledRect({pos,size}, color);
}

void
DrawFilledRect(Rect rect, Color color)
{
    ActivateColorShader(color);

    GLfloat verts[] = {
        rect.pos.x, rect.pos.y,
        rect.pos.x + rect.size.x, rect.pos.y,
        rect.pos.x, rect.pos.y + rect.size.y,
        rect.pos.x + rect.size.x, rect.pos.y + rect.size.y,
    };

    gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void
DrawFilledRect(Vec2f pos, Vec2f size, Color color)
{
    return DrawFilledRect({pos,size}, color);
}

void
DrawLine(Vec2f start, Vec2f end, Color color=c::white)
{
    ActivateColorShader(color);

    GLfloat verts[] = {
        start.x, start.y,
        end.x, end.y
    };

    gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_LINES, 0, 2);
}

void
DrawDirectedLine(DirectedLineLayout layout,
                  Vec2f start_pos, Vec2f end_pos,
                  Vec2f start_vel, Vec2f end_vel,
                  String label)
{
    if(layout.segment_count <= 0) return;
    ActivateColorShader(layout.line_color);
    Vec2f average_vel = (end_pos - start_pos);

    // p(t) = At^3 + Bt^2 + Ct + D,
    // for p(t), A, B, C, D vectors
    Vec2f A = start_vel + end_vel - 2*average_vel;
    Vec2f B = -end_vel - 2*start_vel + 3*average_vel;
    Vec2f C = start_vel;
    Vec2f D = start_pos;

    Array<Vec2f> points = CreateTempArray<Vec2f>(layout.segment_count);
    for(int i=0; i<layout.segment_count; i++)
    {
        float t = (float)i / (float(layout.segment_count - 1));
        points += (t*t*t)*A + (t*t)*B + (t)*C + D;
    }

    gl->BufferData(GL_ARRAY_BUFFER, points.count * sizeof(Vec2f), points.data, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_LINE_STRIP, 0, points.count);

    // Arrowhead
    Vec2f arrow_verts[3] = {
        end_pos,
        end_pos - layout.arrow_size*Normalize(Rotate(end_vel,  layout.arrow_angle)),
        end_pos - layout.arrow_size*Normalize(Rotate(end_vel, -layout.arrow_angle))
    };

    ActivateColorShader(layout.arrow_color);
    gl->BufferData(GL_ARRAY_BUFFER, sizeof(arrow_verts), arrow_verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_TRIANGLES, 0, 3);

    ActivateColorShader(layout.line_color);
    gl->BufferData(GL_ARRAY_BUFFER, sizeof(arrow_verts), arrow_verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_LINE_LOOP, 0, 3);

    Vec2f label_pos = TriangleCenter(arrow_verts[0], arrow_verts[1], arrow_verts[2]);
    TextLayout label_layout = c::def_text_layout;
    label_layout.align = c::align_center;
    //layout.font_size = 24;
    DrawText(label_layout, label_pos, label);
}

// A healthbar in this context is a rectangle, which scales in size proportionally to a value
// relative to a maximum value. i.e., a canonical healthbar
void
DrawHealthbar()
{

}

// A vertical "healthbar" with notches for each healthpoint.
void
DrawVerticalNotchedHealthbar(Rect rect, Color color, Color bg_color, int cur, int max)
{
    if(max == 0) return;

    // background
    DrawFilledRect(rect, bg_color);

    // fill
    float fill_ratio = cur/(float)max;
    float fill_height = fill_ratio * rect.size.y;
    Rect fill_rect = {RectBottomLeft(rect), {rect.size.x, -fill_height}};
    DrawFilledRect(fill_rect, color);

    // outline
    DrawUnfilledRect(rect, c::white);

    // notches
    float pip_height = rect.size.y / max;
    for(int i=1; i<max; ++i)
    {
        DrawLine(rect.pos + Vec2f{0.f, i*pip_height}, rect.pos + Vec2f{rect.size.x-1, i*pip_height});
    }
}

ButtonResponse
DrawButton(ButtonLayout layout, Rect rect, String label)
{
    Camera initial_camera = game->camera;
    if(layout.is_ui)
    {
        MoveCameraToWorldRect(&game->camera, {{0.f,0.f}, game->window_size});
    }

    ButtonResponse response = {};

    Rect aligned_button_rect = AlignRect(rect, layout.align);

    if(PointInRect(aligned_button_rect, MousePos(layout.is_ui)))
    {
        response.hovered = true;
        if(!PointInRect(aligned_button_rect, PrevMousePos(layout.is_ui)))
        {
            response.just_now_hovered = true;
        }
    }

    bool has_focus = false;
    if(response.hovered and !MouseFocusIsTaken())
    {
        has_focus = TakeMouseFocus();
        // Button is being hovered
        DrawUnfilledRect(aligned_button_rect, layout.button_hover_color);
        TextLayout hovered_layout = layout.label_layout;
        hovered_layout.color = layout.label_hover_color;
        Vec2f text_size = SizeText(hovered_layout, label);
        Vec2f text_pos = AlignSizeInRect(text_size, aligned_button_rect, layout.label_layout.align);
        DrawText(hovered_layout, text_pos, label);

        response.hovered = true;
        if(Pressed(vk::LMB)) response.pressed = true;
    }
    else
    {
        // Button is NOT being hovered
        DrawUnfilledRect(aligned_button_rect, layout.button_color);
        Vec2f text_size = SizeText(layout.label_layout, label);
        Vec2f text_pos = AlignSizeInRect(text_size, aligned_button_rect, layout.label_layout.align);
        DrawText(layout.label_layout, text_pos, label);
    }

    if(!has_focus)
    { // Disregard most of the response values if the focus was already taken by something else.
        response = {};
    }

    if(layout.is_ui)
    {
        SetCameraPos(&game->camera, initial_camera.pos);
        SetCameraView(&game->camera, initial_camera.view);
    }

    response.rect = aligned_button_rect;
    return response;
}

ButtonResponse
DrawButton(ButtonLayout layout, Rect rect, const char *c_string)
{
    return DrawButton(layout, rect, StringFromCString(c_string));
}