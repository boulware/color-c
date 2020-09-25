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