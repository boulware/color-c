#include "align.h"

bool
operator==(Align a, Align b)
{
    return(a.x==b.x and a.y==b.y);
}

Rect
AlignRect(Rect rect, Align align)
{
    Rect aligned_rect = rect;

    // X alignment (horizontal)
    if(align.x == AlignX::left)
    {
        // Do nothing
    }
    else if(align.x == AlignX::center)
    {
        aligned_rect.pos.x -= 0.5f*aligned_rect.size.x;
    }
    else if(align.x == AlignX::right)
    {
        aligned_rect.pos.x -= aligned_rect.size.x;
    }

    // Y alignment (vertical)
    if(align.y == AlignY::top)
    {
        // Do nothing
    }
    else if(align.y == AlignY::center)
    {
        aligned_rect.pos.y -= 0.5f*aligned_rect.size.y;
    }
    else if(align.y == AlignY::bottom)
    {
        aligned_rect.pos.y -= aligned_rect.size.y;
    }

    return aligned_rect;
}

Vec2f
AlignSizeInRect(Vec2f child_size, Rect parent, Align align)
{
    Vec2f child_pos = {};

    // X alignment (horizontal)
    if(align.x == AlignX::left)
    {
        child_pos.x = parent.pos.x;
    }
    else if(align.x == AlignX::center)
    {
        child_pos.x = parent.pos.x + 0.5f*parent.size.x;
    }
    else if(align.x == AlignX::right)
    {
        child_pos.x = parent.pos.x + 1.f*parent.size.x;
    }

    // Y alignment (vertical)
    if(align.y == AlignY::top)
    {
        child_pos.y = parent.pos.y;
    }
    else if(align.y == AlignY::center)
    {
        child_pos.y = parent.pos.y + 0.5f*parent.size.y;
    }
    else if(align.y == AlignY::bottom)
    {
        child_pos.y = parent.pos.y + 1.f*parent.size.y;
    }

    return child_pos;
}