#ifndef CAMERA_H
#define CAMERA_H

#include "vec.h"

struct Camera
{
    Vec2f pos;
    Vec2f view;
};

Camera LerpCamera(Camera start, Camera end, float t);
void SetCameraPos(Camera *camera, Vec2f camera_pos);
void MoveCamera(Camera *camera, Vec2f move);
void SetCameraView(Camera *camera, Vec2f view);
void SetCamera(Camera *dst_camera, Camera src_camera);
void MoveCameraToWorldRect(Camera *camera, Rect rect);

Camera PushUiCamera();
void PopUiCamera(Camera pushed_camera);

#endif