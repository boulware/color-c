#include "draw.h"

#include "camera.h"

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

void
SetCameraPos(Camera *camera, Vec2f camera_pos)
{
    //game->prev_camera_pos = game->camera_pos;
    camera->pos = camera_pos;
    gl->ProgramUniform2fv(game->color_shader, 4, 1, (GLfloat*)&camera_pos);
    gl->ProgramUniform2fv(game->uv_shader, 4, 1, (GLfloat*)&camera_pos);
}

void
MoveCamera(Camera *camera, Vec2f move)
{
    camera->pos += move;
    SetCameraPos(camera, camera->pos);
}

void
SetCameraView(Camera *camera, Vec2f view)
{
    camera->view = view;
    gl->ProgramUniform2fv(game->color_shader, 0, 1, (GLfloat*)&view);
    gl->ProgramUniform2fv(game->uv_shader,    0, 1, (GLfloat*)&view);
}

void
MoveCameraToWorldRect(Camera *camera, Rect rect)
{
    float screen_AR = AspectRatio(game->window_size);
    float rect_AR   = AspectRatio(rect);

    Rect adjusted_rect = rect;
    if(rect_AR >= screen_AR)
    { // X is larger than it should be relative to Y (or equal), so scale by the x range
        float x_range = rect.size.x;
        adjusted_rect.size = {rect.size.x, screen_AR / rect_AR * rect.size.y};
    }
    else
    {
        float y_range = rect.size.y;
        adjusted_rect.size = {screen_AR / rect_AR * rect.size.x, rect.size.y};
    }
    SetCameraPos(camera, RectCenter(rect));
    SetCameraView(camera, adjusted_rect.size);
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
DrawUnfilledRect(Rect rect, Color color, bool ui)
{
    Vec2f initial_cam_pos = game->camera.pos;
    Vec2f initial_cam_view = game->camera.view;
    if(ui)
    {
        MoveCameraToWorldRect(&game->camera, {{0.f,0.f}, {1600.f,900.f}});
    }

    ActivateColorShader(color);

    GLfloat verts[] = {
        rect.pos.x, rect.pos.y,
        rect.pos.x + rect.size.x, rect.pos.y,
        rect.pos.x + rect.size.x, rect.pos.y + rect.size.y,
        rect.pos.x, rect.pos.y + rect.size.y
    };

    gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_LINE_LOOP, 0, 4);

    if(ui)
    {
        SetCameraPos( &game->camera, initial_cam_pos);
        SetCameraView(&game->camera, initial_cam_view);
    }
}

void
DrawUnfilledRect(Vec2f pos, Vec2f size, Color color, bool ui)
{
    return DrawUnfilledRect({pos,size}, color, ui);
}

void
DrawFilledRect(Rect rect, Color color, bool ui)
{
    Vec2f initial_cam_pos =  game->camera.pos;
    Vec2f initial_cam_view = game->camera.view;
    if(ui)
    {
        MoveCameraToWorldRect(&game->camera, {{0.f,0.f}, {1600.f,900.f}});
    }

    ActivateColorShader(color);

    GLfloat verts[] = {
        rect.pos.x, rect.pos.y,
        rect.pos.x + rect.size.x, rect.pos.y,
        rect.pos.x, rect.pos.y + rect.size.y,
        rect.pos.x + rect.size.x, rect.pos.y + rect.size.y,
    };

    gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if(ui)
    {
        SetCameraPos( &game->camera, initial_cam_pos);
        SetCameraView(&game->camera, initial_cam_view);
    }
}

void
DrawFilledRect(Vec2f pos, Vec2f size, Color color, bool ui)
{
    return DrawFilledRect({pos,size}, color, ui);
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

// A healthbar in this context is a rectangle, which scales in size proportionally to a value
// relative to a maximum value. i.e., a canonical healthbar
void
DrawHealthbar()
{

}

ButtonResponse
DrawButton(ButtonLayout layout, Rect rect, String label)
{
    ButtonResponse response = {};

    Rect aligned_button_rect = AlignRect(rect, layout.align);
    response.rect = aligned_button_rect;

    if(PointInRect(aligned_button_rect, MousePos()))
    {
        response.hovered = true;
        if(!PointInRect(aligned_button_rect, PrevMousePos()))
        {
            response.just_now_hovered = true;
        }
    }

    if(response.hovered)
    {
        // Button is being hovered
        DrawUnfilledRect(aligned_button_rect, layout.button_hover_color);
        TextLayout hovered_layout = layout.label_layout;
        hovered_layout.color = layout.label_hover_color;
        DrawText(hovered_layout, RectCenter(aligned_button_rect), label);

        response.hovered = true;
        if(Pressed(vk::LMB)) response.pressed = true;
    }
    else
    {
        // Button is NOT being hovered
        DrawUnfilledRect(aligned_button_rect, layout.button_color);
        DrawText(layout.label_layout, RectCenter(aligned_button_rect), label);
    }

    return response;
}

ButtonResponse
DrawButton(ButtonLayout layout, Rect rect, const char *c_string)
{
    return DrawButton(layout, rect, StringFromCString(c_string));
}