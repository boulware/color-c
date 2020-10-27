#include "memory_draw.h"
#include "memory.h"
#include "draw.h"
#include "text_render.h"

Rect
DrawArenas(Pool<Arena> arena_pool, Vec2f pos)
{
    for(int i=0; i<arena_pool.entry_count; ++i)
    {
        PoolEntry<Arena> *entry = (game->arena_pool->entries + i);
        Arena arena = entry->data;

        //SetDrawDepth(c::debug_overlay_draw_depth);
        size_t total_bytes = (u8 *)arena.end - (u8 *)arena.start;
        size_t cur_bytes   = (u8 *)arena.current - (u8 *)arena.start;
        size_t max_bytes   = (u8 *)arena.max_current - (u8 *)arena.start;
        float filled_ratio = (float)cur_bytes / total_bytes;
        float max_ratio = (float)max_bytes / total_bytes;

        Rect rect = {pos + i*Vec2f{0.f,20.f}, {600.f, 20.f}};
        DrawFilledRect(rect.pos, rect.size, c::vdk_red);
        DrawFilledRect(rect.pos, {max_ratio * rect.size.x, rect.size.y}, c::grey);
        DrawFilledRect(rect.pos, {filled_ratio * rect.size.x, rect.size.y}, c::green);
        DrawUnfilledRect(rect.pos, {600.f, 20.f}, c::white);

        TextLayout layout = c::small_text_layout;
        layout.align = c::align_center;
        DrawUiText(layout, RectCenter(rect), "(%s) %d allocs; %zu/%zu",
                   arena.debug_name,
                   arena.allocs_since_reset, cur_bytes, total_bytes);
    }

    Rect window_rect = {
        .pos = pos,
        .size = Vec2f{600.f, arena_pool.entry_count * 20.f},
    };

    return window_rect;
}