#ifndef ALIGN_H
#define ALIGN_H

#include "vec.h"

enum class AlignX
{
	left,
	center,
	right
};

enum class AlignY
{
	top,
	center,
	bottom
};

struct Align
{
	AlignX x;
	AlignY y;
};

bool operator==(Align a, Align b);

Rect AlignRect(Rect rect, Align align);
Vec2f AlignSizeInRect(Vec2f child_size, Rect parent, Align align);

#endif