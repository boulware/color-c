#include "memory_draw.h"
#include "memory.h"
#include "draw.h"
#include "text_render.h"

void
DrawArena(Arena arena, Vec2f pos)
{
    size_t total_bytes = (u8 *)arena.end - (u8 *)arena.start;
    size_t cur_bytes   = (u8 *)arena.current - (u8 *)arena.start;
    float filled_ratio = (float)cur_bytes / total_bytes;

    Rect rect = {pos, {600.f, 20.f}};
    DrawFilledRect(pos, rect.size, c::dk_red, true);
    DrawFilledRect(pos, {filled_ratio * rect.size.x, rect.size.y}, c::green, true);
    DrawUnfilledRect(pos, {600.f, 20.f}, c::white, true);

    TextLayout layout = c::small_text_layout;
    layout.align = c::align_center;
    DrawUiText(layout, RectCenter(rect), "(%s) %d allocs; %zu/%zu",
               arena.debug_name,
               arena.allocs_since_reset, cur_bytes, total_bytes);
}