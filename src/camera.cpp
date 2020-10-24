#include "camera.h"

Camera
LerpCamera(Camera start, Camera end, float t)
{
    Camera lerped = {
        .pos = Lerp(start.pos, end.pos, t),
        .view = Lerp(start.view, end.view, t)
    };

    return lerped;
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
SetCamera(Camera *dst_camera, Camera src_camera)
{
    SetCameraPos(dst_camera, src_camera.pos);
    SetCameraView(dst_camera, src_camera.view);
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

Camera
PushUiCamera()
{
    Camera prev_camera = game->camera;
    MoveCameraToWorldRect(&game->camera, {{}, game->window_size});
    return prev_camera;
}

void
PopUiCamera(Camera pushed_camera)
{
    SetCamera(&game->camera, pushed_camera);
}