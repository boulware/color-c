#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vec.h"

bool LineSegmentsInnerIntersect(Vec2f a0, Vec2f a1, Vec2f b0, Vec2f b1, float cutoff);

Vec2f TriangleCenter(Vec2f a, Vec2f b, Vec2f c);

#endif
