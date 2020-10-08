#include "geometry.h"

bool LineSegmentsInnerIntersect(Vec2f a0, Vec2f a1, Vec2f b0, Vec2f b1, float edge_percent)
{
    float x1 = a0.x; float y1 = a0.y;
    float x2 = a1.x; float y2 = a1.y;
    float x3 = b0.x; float y3 = b0.y;
    float x4 = b1.x; float y4 = b1.y;

    float denominator = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
    if(denominator == 0.f) return false;

    float t_numerator = (x1-x3)*(y3-y4)-(y1-y3)*(x3-x4);
    float u_numerator = -((x1-x2)*(y1-y3)-(y1-y2)*(x1-x3));

    float t = t_numerator / denominator;
    if(t < edge_percent or t > 1.f-edge_percent) return false;

    float u = u_numerator / denominator;
    if(u < edge_percent or u > 1.f-edge_percent) return false;

    return true;
}