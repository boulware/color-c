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