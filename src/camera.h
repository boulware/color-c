#ifndef CAMERA_H
#define CAMERA_H

#include "vec.h"

struct Camera
{
    Vec2f pos;
    Vec2f view;
};

Camera LerpCamera(Camera start, Camera end, float t);

#endif